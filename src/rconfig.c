/**
 * @file  rconfig.c
 * @copyright Copyright (C) 2019-2026 Michal Babik
 *
 * This file is part of Small File Renamer.
 *
 * Small File Renamer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Small File Renamer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Small File Renamer.  If not, see <https://www.gnu.org/licenses/>.
 *
 * @brief  Save/restore RenData settings to/from a local JSON config file.
 *
 * Hand-rolled minimal JSON reader/writer: the schema is fixed (a flat
 * object with a handful of scalar fields plus one small nested object per
 * rename-operation settings struct, no arrays), so a general-purpose JSON
 * library is not needed. The writer always produces well-formed output;
 * the parser is defensive against a hand-edited or corrupted file and
 * never partially applies a bad file (parse-then-apply).
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#include <string.h>
#include <glib/gstdio.h>
#include "rconfig.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Append a JSON-escaped, quoted string to gstr.
 *
 * @param[out] gstr String buffer to append to
 * @param[in]  val  String to escape and append
 * @return     none
 */
static void
rconfig_append_json_string (GString *gstr, const char *val)
{
    const unsigned char *p = (const unsigned char *) val;

    g_string_append_c (gstr, '"');
    for (; *p != '\0'; p++) {
        switch (*p) {
        case '"':  g_string_append (gstr, "\\\""); break;
        case '\\': g_string_append (gstr, "\\\\"); break;
        case '\n': g_string_append (gstr, "\\n");  break;
        case '\t': g_string_append (gstr, "\\t");  break;
        case '\r': g_string_append (gstr, "\\r");  break;
        default:
            if (*p < 0x20)
                g_string_append_printf (gstr, "\\u%04x", *p);
            else
                g_string_append_c (gstr, (char) *p);
        }
    }
    g_string_append_c (gstr, '"');
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get the path to the JSON config file.
 */
char *
rconfig_get_path (void)
{
    return g_build_filename (g_get_user_config_dir (),
                              RCONFIG_DIR_NAME, RCONFIG_FILE_NAME, nullptr);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Serialize rd_data as JSON and write it to path.
 */
bool
rconfig_write (const RenData *rd_data, const char *path)
{
    auto rdel = rendata_get_rdelete  (rd_data);
    auto rins = rendata_get_rinsert  (rd_data);
    auto rovr = rendata_get_roverwr  (rd_data);
    auto rrep = rendata_get_rreplace (rd_data);
    auto rnum = rendata_get_rnumber  (rd_data);
    auto gstr = g_string_new (nullptr);
    char     *dir  = nullptr;
    GError   *gerr = nullptr;
    bool      ok;

    g_string_append (gstr, "{\n");
    g_string_append_printf (gstr, "  \"uplo\": %d,\n",
                             (int) rendata_get_uplo (rd_data));
    g_string_append_printf (gstr, "  \"spaces\": %d,\n",
                             (int) rendata_get_spaces (rd_data));
    g_string_append_printf (gstr, "  \"applyto\": %d,\n",
                             (int) rendata_get_applyto (rd_data));
    g_string_append_printf (gstr, "  \"renexit\": %d,\n",
                             (int) rendata_get_renexit (rd_data));
    g_string_append_printf (gstr, "  \"dirsel\": %d,\n",
                             (int) rendata_get_dirsel (rd_data));
    g_string_append_printf (gstr, "  \"rememberopt\": %d,\n",
                             (int) rendata_get_rememberopt (rd_data));

    g_string_append_printf (gstr, "  \"delete\": { \"cnt\": %u, \"pos\": %u },\n",
                             (unsigned) rdelete_get_cnt (rdel),
                             (unsigned) rdelete_get_pos (rdel));

    g_string_append (gstr, "  \"insert\": { \"text\": ");
    rconfig_append_json_string (gstr, rinsovr_get_text (rins));
    g_string_append_printf (gstr, ", \"pos\": %u },\n",
                             (unsigned) rinsovr_get_pos (rins));

    g_string_append (gstr, "  \"overwrite\": { \"text\": ");
    rconfig_append_json_string (gstr, rinsovr_get_text (rovr));
    g_string_append_printf (gstr, ", \"pos\": %u },\n",
                             (unsigned) rinsovr_get_pos (rovr));

    g_string_append (gstr, "  \"replace\": { \"from\": ");
    rconfig_append_json_string (gstr, rreplace_get_from (rrep));
    g_string_append (gstr, ", \"to\": ");
    rconfig_append_json_string (gstr, rreplace_get_to (rrep));
    g_string_append (gstr, " },\n");

    g_string_append_printf (gstr,
            "  \"number\": { \"opt\": %d, \"start\": %lu, \"pos\": %u }\n",
            (int) rnumber_get_opt (rnum),
            (unsigned long) rnumber_get_start (rnum),
            (unsigned) rnumber_get_pos (rnum));

    g_string_append (gstr, "}\n");

    dir = g_path_get_dirname (path);
    g_mkdir_with_parents (dir, 0700);
    g_free (dir);

    ok = g_file_set_contents (path, gstr->str, (gssize) gstr->len, &gerr);
    if (!ok) {
        g_warning ("sfrename: could not write config file %s: %s",
                   path, gerr->message);
        g_error_free (gerr);
    }

    g_string_free (gstr, TRUE);
    return ok;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Skip whitespace at the cursor.
 */
static void
skip_ws (const char **p)
{
    while (**p == ' ' || **p == '\t' || **p == '\n' || **p == '\r')
        (*p)++;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Skip whitespace, then consume the expected literal char.
 *
 * @return true if c was found and consumed, false otherwise (cursor is left
 *         unchanged on failure)
 */
static bool
expect_char (const char **p, char c)
{
    skip_ws (p);
    if (**p != c)
        return false;
    (*p)++;
    return true;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Parse a quoted, escaped JSON string into out (bounded to cap).
 *
 * Unescapes \" \\ \/ \n \t \r and \uXXXX (only codepoints below 0x80 are
 * translated; higher codepoints, which this module never writes, decode to
 * '?' rather than failing). Output longer than cap-1 bytes is truncated.
 *
 * @return true on a well-formed string, false on malformed/unterminated
 *         input (cursor position on failure is undefined)
 */
static bool
parse_json_string (const char **p, char *out, size_t cap)
{
    size_t i = 0;

    skip_ws (p);
    if (**p != '"')
        return false;
    (*p)++;

    while (**p != '"') {
        char c = **p;

        if (c == '\0')
            return false;

        if (c == '\\') {
            (*p)++;
            c = **p;
            if (c == '\0')
                return false;

            switch (c) {
            case '"':  case '\\': case '/':
                if (i < cap - 1) out[i++] = c;
                (*p)++;
                break;
            case 'n':
                if (i < cap - 1) out[i++] = '\n';
                (*p)++;
                break;
            case 't':
                if (i < cap - 1) out[i++] = '\t';
                (*p)++;
                break;
            case 'r':
                if (i < cap - 1) out[i++] = '\r';
                (*p)++;
                break;
            case 'u': {
                unsigned int cp;
                int          j;

                (*p)++;
                for (j = 0; j < 4; j++)
                    if (!g_ascii_isxdigit ((*p)[j]))
                        return false;
                {
                    char hex[5] = { (*p)[0], (*p)[1], (*p)[2], (*p)[3], '\0' };
                    cp = (unsigned int) strtoul (hex, nullptr, 16);
                }
                (*p) += 4;
                if (i < cap - 1)
                    out[i++] = (cp < 0x80) ? (char) cp : '?';
                break;
            }
            default:
                return false;
            }
        }
        else {
            if (i < cap - 1)
                out[i++] = c;
            (*p)++;
        }
    }
    (*p)++; /* closing quote */
    out[i] = '\0';
    return true;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Parse a signed decimal integer at the cursor.
 *
 * @return true on success, false if no digits were found
 */
static bool
parse_long (const char **p, long *out)
{
    char *endptr = nullptr;
    long  val;

    skip_ws (p);
    val = strtol (*p, &endptr, 10);
    if (endptr == *p)
        return false;
    *p = endptr;
    *out = val;
    return true;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Skip over one JSON value (string, object, array, or bare token)
 *         whose key was not recognized, so unknown/future keys in a
 *         hand-edited file don't break parsing.
 *
 * @return true on success, false on malformed/unterminated input
 */
static bool
skip_value (const char **p)
{
    char scratch[FN_LEN * 4];
    char c;

    skip_ws (p);
    c = **p;

    if (c == '"')
        return parse_json_string (p, scratch, sizeof (scratch));

    if (c == '{' || c == '[') {
        char     open  = c;
        char     close = (c == '{') ? '}' : ']';
        int      depth = 0;
        bool in_str = false;

        do {
            c = **p;
            if (c == '\0')
                return false;
            if (in_str) {
                if (c == '\\') {
                    (*p)++;
                    if (**p == '\0')
                        return false;
                }
                else if (c == '"') {
                    in_str = false;
                }
            }
            else {
                if (c == '"')       in_str = true;
                else if (c == open)  depth++;
                else if (c == close) depth--;
            }
            (*p)++;
        } while (depth > 0);
        return true;
    }

    if (c == '\0')
        return false;

    while (**p != '\0' && **p != ',' && **p != '}' && **p != ']' &&
           !g_ascii_isspace (**p))
        (*p)++;
    return true;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Parse a {"cnt": N, "pos": N} object (already-consumed key, cursor
 *         at the opening brace).
 */
static bool
parse_delete_obj (const char **p, long *cnt, long *pos)
{
    if (!expect_char (p, '{'))
        return false;

    skip_ws (p);
    if (**p == '}') {
        (*p)++;
        return true;
    }

    for (;;) {
        char key[32];

        if (!parse_json_string (p, key, sizeof (key)))
            return false;
        if (!expect_char (p, ':'))
            return false;

        if (strcmp (key, "cnt") == 0) {
            if (!parse_long (p, cnt)) return false;
        }
        else if (strcmp (key, "pos") == 0) {
            if (!parse_long (p, pos)) return false;
        }
        else if (!skip_value (p)) {
            return false;
        }

        skip_ws (p);
        if (**p == ',') { (*p)++; continue; }
        if (**p == '}') { (*p)++; break; }
        return false;
    }
    return true;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Parse a {"text": "...", "pos": N} object, used for both the
 *         "insert" and "overwrite" fields.
 */
static bool
parse_insovr_obj (const char **p, char *text, size_t text_cap, long *pos)
{
    if (!expect_char (p, '{'))
        return false;

    skip_ws (p);
    if (**p == '}') {
        (*p)++;
        return true;
    }

    for (;;) {
        char key[32];

        if (!parse_json_string (p, key, sizeof (key)))
            return false;
        if (!expect_char (p, ':'))
            return false;

        if (strcmp (key, "text") == 0) {
            if (!parse_json_string (p, text, text_cap)) return false;
        }
        else if (strcmp (key, "pos") == 0) {
            if (!parse_long (p, pos)) return false;
        }
        else if (!skip_value (p)) {
            return false;
        }

        skip_ws (p);
        if (**p == ',') { (*p)++; continue; }
        if (**p == '}') { (*p)++; break; }
        return false;
    }
    return true;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Parse a {"from": "...", "to": "..."} object.
 */
static bool
parse_replace_obj (const char **p,
                   char       *from, size_t from_cap,
                   char       *to,   size_t to_cap)
{
    if (!expect_char (p, '{'))
        return false;

    skip_ws (p);
    if (**p == '}') {
        (*p)++;
        return true;
    }

    for (;;) {
        char key[32];

        if (!parse_json_string (p, key, sizeof (key)))
            return false;
        if (!expect_char (p, ':'))
            return false;

        if (strcmp (key, "from") == 0) {
            if (!parse_json_string (p, from, from_cap)) return false;
        }
        else if (strcmp (key, "to") == 0) {
            if (!parse_json_string (p, to, to_cap)) return false;
        }
        else if (!skip_value (p)) {
            return false;
        }

        skip_ws (p);
        if (**p == ',') { (*p)++; continue; }
        if (**p == '}') { (*p)++; break; }
        return false;
    }
    return true;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Parse a {"opt": N, "start": N, "pos": N} object.
 */
static bool
parse_number_obj (const char **p, long *opt, long *start, long *pos)
{
    if (!expect_char (p, '{'))
        return false;

    skip_ws (p);
    if (**p == '}') {
        (*p)++;
        return true;
    }

    for (;;) {
        char key[32];

        if (!parse_json_string (p, key, sizeof (key)))
            return false;
        if (!expect_char (p, ':'))
            return false;

        if (strcmp (key, "opt") == 0) {
            if (!parse_long (p, opt)) return false;
        }
        else if (strcmp (key, "start") == 0) {
            if (!parse_long (p, start)) return false;
        }
        else if (strcmp (key, "pos") == 0) {
            if (!parse_long (p, pos)) return false;
        }
        else if (!skip_value (p)) {
            return false;
        }

        skip_ws (p);
        if (**p == ',') { (*p)++; continue; }
        if (**p == '}') { (*p)++; break; }
        return false;
    }
    return true;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Read and parse a JSON config file, applying it to rd_data.
 */
bool
rconfig_parse (RenData *rd_data, const char *path)
{
    char       *buf = nullptr;
    gsize       len = 0;
    GError     *gerr = nullptr;
    const char *p;

    long uplo        = DEF_UPLO;
    long spaces      = DEF_SPACES;
    long applyto     = DEF_APPLTO;
    long renexit     = DEF_RENEXT;
    long dirsel      = DEF_DIRSEL;
    long rememberopt = DEF_REMEMBEROPT;

    long del_cnt = 0, del_pos = 0;

    char ins_text[FN_LEN + 1] = "";
    long ins_pos = 0;
    char ovr_text[FN_LEN + 1] = "";
    long ovr_pos = 0;

    char rep_from[FN_LEN + 1] = "";
    char rep_to[FN_LEN + 1]   = "";

    long num_opt = 0, num_start = 0, num_pos = 0;

    if (!g_file_get_contents (path, &buf, &len, &gerr)) {
        g_warning ("sfrename: could not read config file %s: %s",
                   path, gerr->message);
        g_error_free (gerr);
        return false;
    }

    p = buf;

    if (!expect_char (&p, '{')) {
        g_warning ("sfrename: config file %s is malformed, ignoring", path);
        g_free (buf);
        return false;
    }

    skip_ws (&p);
    if (*p != '}') {
        for (;;) {
            char key[32];
            bool field_ok = true;

            if (!parse_json_string (&p, key, sizeof (key)))
                { field_ok = false; }
            else if (!expect_char (&p, ':'))
                { field_ok = false; }
            else if (strcmp (key, "uplo") == 0)
                field_ok = parse_long (&p, &uplo);
            else if (strcmp (key, "spaces") == 0)
                field_ok = parse_long (&p, &spaces);
            else if (strcmp (key, "applyto") == 0)
                field_ok = parse_long (&p, &applyto);
            else if (strcmp (key, "renexit") == 0)
                field_ok = parse_long (&p, &renexit);
            else if (strcmp (key, "dirsel") == 0)
                field_ok = parse_long (&p, &dirsel);
            else if (strcmp (key, "rememberopt") == 0)
                field_ok = parse_long (&p, &rememberopt);
            else if (strcmp (key, "delete") == 0)
                field_ok = parse_delete_obj (&p, &del_cnt, &del_pos);
            else if (strcmp (key, "insert") == 0)
                field_ok = parse_insovr_obj (&p, ins_text, sizeof (ins_text), &ins_pos);
            else if (strcmp (key, "overwrite") == 0)
                field_ok = parse_insovr_obj (&p, ovr_text, sizeof (ovr_text), &ovr_pos);
            else if (strcmp (key, "replace") == 0)
                field_ok = parse_replace_obj (&p, rep_from, sizeof (rep_from),
                                              rep_to, sizeof (rep_to));
            else if (strcmp (key, "number") == 0)
                field_ok = parse_number_obj (&p, &num_opt, &num_start, &num_pos);
            else
                field_ok = skip_value (&p);

            if (!field_ok) {
                g_warning ("sfrename: config file %s is malformed, ignoring", path);
                g_free (buf);
                return false;
            }

            skip_ws (&p);
            if (*p == ',') { p++; continue; }
            if (*p == '}') { p++; break; }
            g_warning ("sfrename: config file %s is malformed, ignoring", path);
            g_free (buf);
            return false;
        }
    }
    else {
        p++;
    }

    g_free (buf);

    /* Clamp to valid ranges before applying - defends against a hand-edited
     * file with out-of-range or negative values. */
    if (uplo < 0 || uplo > 2)         uplo = DEF_UPLO;
    if (spaces < 0 || spaces > 2)     spaces = DEF_SPACES;
    if (applyto < 0 || applyto > 2)   applyto = DEF_APPLTO;
    renexit     = renexit     ? 1 : 0;
    dirsel      = dirsel      ? 1 : 0;
    rememberopt = rememberopt ? 1 : 0;
    if (del_cnt < 0 || del_cnt > FN_LEN) del_cnt = 0;
    if (del_pos < 0 || del_pos > FN_LEN) del_pos = 0;
    if (ins_pos < 0 || ins_pos > FN_LEN) ins_pos = 0;
    if (ovr_pos < 0 || ovr_pos > FN_LEN) ovr_pos = 0;
    if (num_pos < 0 || num_pos > FN_LEN) num_pos = 0;
    if (num_start < 0)                   num_start = 0;

    rendata_set_uplo        (rd_data, (int8_t) uplo);
    rendata_set_spaces      (rd_data, (int8_t) spaces);
    rendata_set_applyto     (rd_data, (int8_t) applyto);
    rendata_set_renexit     (rd_data, renexit != 0);
    rendata_set_dirsel      (rd_data, (int8_t) dirsel);
    rendata_set_rememberopt (rd_data, rememberopt != 0);

    rdelete_set_cnt (rd_data->del, (uint8_t) del_cnt);
    rdelete_set_pos (rd_data->del, (uint8_t) del_pos);

    rinsovr_set_text (rd_data->ins, ins_text);
    rinsovr_set_pos  (rd_data->ins, (uint8_t) ins_pos);

    rinsovr_set_text (rd_data->ovrw, ovr_text);
    rinsovr_set_pos  (rd_data->ovrw, (uint8_t) ovr_pos);

    rreplace_set_from (rd_data->replace, rep_from);
    rreplace_set_to   (rd_data->replace, rep_to);

    rnumber_set_opt   (rd_data->number, (int8_t) num_opt);
    rnumber_set_start (rd_data->number, (uint_fast32_t) num_start);
    rnumber_set_pos   (rd_data->number, (uint8_t) num_pos);

    return true;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Save settings to the standard config path if "remember options on
 *         exit" is enabled, otherwise remove any existing config file.
 */
bool
rconfig_save (const RenData *rd_data)
{
    auto path = rconfig_get_path ();
    bool ok   = true;

    if (rendata_get_rememberopt (rd_data)) {
        ok = rconfig_write (rd_data, path);
    }
    else if (g_file_test (path, G_FILE_TEST_EXISTS)) {
        g_unlink (path);
    }

    g_free (path);
    return ok;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Load settings from the standard config path into rd_data, if it
 *         exists and parses successfully.
 */
bool
rconfig_load (RenData *rd_data)
{
    auto path = rconfig_get_path ();
    bool ok   = false;

    if (g_file_test (path, G_FILE_TEST_EXISTS))
        ok = rconfig_parse (rd_data, path);

    g_free (path);
    return ok;
}
/*----------------------------------------------------------------------------*/
