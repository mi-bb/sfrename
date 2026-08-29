/**
 * @file  test_strfn.c
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
 * @brief  Unit tests for strfn.c
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <glib.h>
#include <string.h>
#include "strfn.h"
/*----------------------------------------------------------------------------*/
static ProcessData
make_pd (char *s_srcdst, size_t *len, size_t *u8len)
{
    ProcessData pd = { s_srcdst, len, u8len, NULL, 0, 0, NULL, 0, 0, 0, 0, 0, 0, 0 };
    return pd;
}
/*----------------------------------------------------------------------------*/
static void
test_get_valid_length_ascii (void)
{
    g_assert_cmpuint (string_get_valid_length ("hello", 255), ==, 5);
    g_assert_cmpuint (string_get_valid_length ("hello", 3), ==, 3);
    g_assert_cmpuint (string_get_valid_length ("", 255), ==, 0);
}
/*----------------------------------------------------------------------------*/
static void
test_get_valid_length_utf8_boundary (void)
{
    /* "café": "caf" (3 ascii bytes) + U+00E9 (2-byte UTF-8 char) = 5 bytes. */
    const char *s = "caf\xc3\xa9";

    /* Truncating to 4 bytes would cut the 2-byte char in half; must back
     * off to the last valid boundary (3 bytes, "caf"). */
    g_assert_cmpuint (string_get_valid_length (s, 4), ==, 3);
    /* 5 bytes is the full, well-formed string. */
    g_assert_cmpuint (string_get_valid_length (s, 5), ==, 5);
    /* A max longer than the string just clamps to the actual length. */
    g_assert_cmpuint (string_get_valid_length (s, 100), ==, 5);
}
/*----------------------------------------------------------------------------*/
static void
test_extract_combine_name_ext_basic (void)
{
    char name[FN_LEN + 1] = "";
    char ext[FN_LEN + 1]  = "";
    char combined[FN_LEN + 1] = "";

    string_extract_name_ext ("file.txt", name, ext);
    g_assert_cmpstr (name, ==, "file");
    g_assert_cmpstr (ext, ==, ".txt");

    string_combine_name_ext (combined, name, ext);
    g_assert_cmpstr (combined, ==, "file.txt");
}
/*----------------------------------------------------------------------------*/
static void
test_extract_name_ext_multiple_dots (void)
{
    char name[FN_LEN + 1] = "";
    char ext[FN_LEN + 1]  = "";

    /* Only the last dot counts as the extension separator. */
    string_extract_name_ext ("archive.tar.gz", name, ext);
    g_assert_cmpstr (name, ==, "archive.tar");
    g_assert_cmpstr (ext, ==, ".gz");
}
/*----------------------------------------------------------------------------*/
static void
test_extract_name_ext_no_ext (void)
{
    char name[FN_LEN + 1] = "";
    char ext[FN_LEN + 1]  = "";

    string_extract_name_ext ("noext", name, ext);
    g_assert_cmpstr (name, ==, "noext");
    g_assert_cmpstr (ext, ==, "");
}
/*----------------------------------------------------------------------------*/
static void
test_extract_name_ext_hidden_dotfile (void)
{
    char name[FN_LEN + 1] = "";
    char ext[FN_LEN + 1]  = "";

    /* A leading dot is not treated as an extension separator. */
    string_extract_name_ext (".bashrc", name, ext);
    g_assert_cmpstr (name, ==, ".bashrc");
    g_assert_cmpstr (ext, ==, "");
}
/*----------------------------------------------------------------------------*/
static void
test_delete_chars_middle (void)
{
    char   s[FN_LEN + 1];
    size_t len, u8len;

    strcpy (s, "hello world");
    len = strlen (s);
    u8len = (size_t) g_utf8_strlen (s, -1);

    ProcessData pd = make_pd (s, &len, &u8len);
    pd.ui_pos = 5; /* delete starting at the space */
    pd.ui_cnt = 6; /* " world" */

    string_delete_chars (&pd);

    g_assert_cmpstr (s, ==, "hello");
    g_assert_cmpuint (len, ==, 5);
    g_assert_cmpuint (u8len, ==, 5);
}
/*----------------------------------------------------------------------------*/
static void
test_delete_chars_beyond_end_clamps (void)
{
    char   s[FN_LEN + 1];
    size_t len, u8len;

    strcpy (s, "abc");
    len = strlen (s);
    u8len = (size_t) g_utf8_strlen (s, -1);

    ProcessData pd = make_pd (s, &len, &u8len);
    pd.ui_pos = 100; /* far beyond string length */
    pd.ui_cnt = 2;

    string_delete_chars (&pd);

    /* Deleting the last 2 chars from a clamped position. */
    g_assert_cmpstr (s, ==, "a");
    g_assert_cmpuint (len, ==, 1);
}
/*----------------------------------------------------------------------------*/
static void
test_insert_string_middle (void)
{
    char   s[FN_LEN + 1];
    size_t len, u8len;

    strcpy (s, "helloworld");
    len = strlen (s);
    u8len = (size_t) g_utf8_strlen (s, -1);

    ProcessData pd = make_pd (s, &len, &u8len);
    pd.s_str1 = " ";
    pd.str1_len = 1;
    pd.str1_u8len = 1;
    pd.ui_pos = 5;

    string_insert_string (&pd);

    g_assert_cmpstr (s, ==, "hello world");
    g_assert_cmpuint (len, ==, 11);
    g_assert_cmpuint (u8len, ==, 11);
}
/*----------------------------------------------------------------------------*/
static void
test_insert_string_truncates_at_max (void)
{
    char   s[FN_LEN + 1];
    size_t len, u8len;

    memset (s, 'a', FN_LEN);
    s[FN_LEN] = '\0';
    len = strlen (s);
    u8len = (size_t) g_utf8_strlen (s, -1);

    ProcessData pd = make_pd (s, &len, &u8len);
    pd.s_str1 = "XYZ";
    pd.str1_len = 3;
    pd.str1_u8len = 3;
    pd.ui_pos = 0;

    string_insert_string (&pd);

    g_assert_cmpuint (strlen (s), <=, FN_LEN);
    g_assert_cmpuint (len, <=, FN_LEN);
    g_assert (g_str_has_prefix (s, "XYZ"));
}
/*----------------------------------------------------------------------------*/
static void
test_overwrite_string_middle (void)
{
    char   s[FN_LEN + 1];
    size_t len, u8len;

    strcpy (s, "hello world");
    len = strlen (s);
    u8len = (size_t) g_utf8_strlen (s, -1);

    ProcessData pd = make_pd (s, &len, &u8len);
    pd.s_str1 = "THERE";
    pd.str1_u8len = 5;
    pd.ui_pos = 6; /* overwrite "world" */

    string_overwrite_string (&pd);

    g_assert_cmpstr (s, ==, "hello THERE");
}
/*----------------------------------------------------------------------------*/
static void
test_replace_in_basic (void)
{
    char   s[FN_LEN + 1];
    size_t len, u8len;

    strcpy (s, "foo bar foo");
    len = strlen (s);
    u8len = (size_t) g_utf8_strlen (s, -1);

    ProcessData pd = make_pd (s, &len, &u8len);
    pd.s_str1 = "foo";
    pd.str1_len = 3;
    pd.s_str2 = "baz";

    string_replace_in (&pd);

    g_assert_cmpstr (s, ==, "baz bar baz");
}
/*----------------------------------------------------------------------------*/
static void
test_replace_in_no_match_is_noop (void)
{
    char   s[FN_LEN + 1];
    size_t len, u8len;

    strcpy (s, "hello");
    len = strlen (s);
    u8len = (size_t) g_utf8_strlen (s, -1);

    ProcessData pd = make_pd (s, &len, &u8len);
    pd.s_str1 = "xyz";
    pd.str1_len = 3;
    pd.s_str2 = "abc";

    string_replace_in (&pd);

    g_assert_cmpstr (s, ==, "hello");
    g_assert_cmpuint (len, ==, 5);
}
/*----------------------------------------------------------------------------*/
static void
test_add_number_basic (void)
{
    char   s[FN_LEN + 1];
    size_t len, u8len;

    strcpy (s, "file");
    len = strlen (s);
    u8len = (size_t) g_utf8_strlen (s, -1);

    ProcessData pd = make_pd (s, &len, &u8len);
    pd.ui_pos = 4;   /* append at end */
    pd.ui_no = 2;    /* this is the 3rd file (0-indexed) */
    pd.ui_start = 1;
    pd.ui_max = 8;   /* 9 files total (0..8); with start offset 1, highest
                      * numbered file is 9 -> single digit, no zero-padding */

    string_add_number (&pd);

    g_assert_cmpstr (s, ==, "file3");
}
/*----------------------------------------------------------------------------*/
static void
test_add_number_zero_pads (void)
{
    char   s[FN_LEN + 1];
    size_t len, u8len;

    strcpy (s, "file");
    len = strlen (s);
    u8len = (size_t) g_utf8_strlen (s, -1);

    ProcessData pd = make_pd (s, &len, &u8len);
    pd.ui_pos = 4;
    pd.ui_no = 0;
    pd.ui_start = 0;
    pd.ui_max = 10; /* two-digit range (00..10) forces zero padding */

    string_add_number (&pd);

    g_assert_cmpstr (s, ==, "file00");
}
/*----------------------------------------------------------------------------*/
static void
test_to_upper_lower (void)
{
    char   s[FN_LEN + 1];
    size_t len, u8len;

    strcpy (s, "Hello Wörld");
    len = strlen (s);
    u8len = (size_t) g_utf8_strlen (s, -1);
    ProcessData pd = make_pd (s, &len, &u8len);

    string_to_upper (&pd);
    g_assert_cmpstr (s, ==, "HELLO WÖRLD");

    len = strlen (s);
    u8len = (size_t) g_utf8_strlen (s, -1);
    pd = make_pd (s, &len, &u8len);
    string_to_lower (&pd);
    g_assert_cmpstr (s, ==, "hello wörld");
}
/*----------------------------------------------------------------------------*/
static void
test_process_filename_applies_to_name_only (void)
{
    char   s[FN_LEN + 1];
    size_t len, u8len;

    strcpy (s, "report.PDF");
    len = strlen (s);
    u8len = (size_t) g_utf8_strlen (s, -1);
    ProcessData pd = make_pd (s, &len, &u8len);

    /* ne == 1: apply to name only, extension untouched */
    string_process_filename (string_to_upper, &pd, 1);

    g_assert_cmpstr (s, ==, "REPORT.PDF");
}
/*----------------------------------------------------------------------------*/
static void
test_process_filename_applies_to_ext_only (void)
{
    char   s[FN_LEN + 1];
    size_t len, u8len;

    strcpy (s, "Report.pdf");
    len = strlen (s);
    u8len = (size_t) g_utf8_strlen (s, -1);
    ProcessData pd = make_pd (s, &len, &u8len);

    /* ne == 0: apply to extension only, name untouched */
    string_process_filename (string_to_upper, &pd, 0);

    g_assert_cmpstr (s, ==, "Report.PDF");
}
/*----------------------------------------------------------------------------*/
static void
test_process_filename_applies_to_both (void)
{
    char   s[FN_LEN + 1];
    size_t len, u8len;

    strcpy (s, "Report.pdf");
    len = strlen (s);
    u8len = (size_t) g_utf8_strlen (s, -1);
    ProcessData pd = make_pd (s, &len, &u8len);

    /* ne == 2: apply to the whole string at once */
    string_process_filename (string_to_upper, &pd, 2);

    g_assert_cmpstr (s, ==, "REPORT.PDF");
}
/*----------------------------------------------------------------------------*/
int
main (int argc, char *argv[])
{
    g_test_init (&argc, &argv, NULL);

    g_test_add_func ("/strfn/get_valid_length/ascii", test_get_valid_length_ascii);
    g_test_add_func ("/strfn/get_valid_length/utf8_boundary", test_get_valid_length_utf8_boundary);

    g_test_add_func ("/strfn/name_ext/basic", test_extract_combine_name_ext_basic);
    g_test_add_func ("/strfn/name_ext/multiple_dots", test_extract_name_ext_multiple_dots);
    g_test_add_func ("/strfn/name_ext/no_ext", test_extract_name_ext_no_ext);
    g_test_add_func ("/strfn/name_ext/hidden_dotfile", test_extract_name_ext_hidden_dotfile);

    g_test_add_func ("/strfn/delete_chars/middle", test_delete_chars_middle);
    g_test_add_func ("/strfn/delete_chars/beyond_end_clamps", test_delete_chars_beyond_end_clamps);

    g_test_add_func ("/strfn/insert_string/middle", test_insert_string_middle);
    g_test_add_func ("/strfn/insert_string/truncates_at_max", test_insert_string_truncates_at_max);

    g_test_add_func ("/strfn/overwrite_string/middle", test_overwrite_string_middle);

    g_test_add_func ("/strfn/replace_in/basic", test_replace_in_basic);
    g_test_add_func ("/strfn/replace_in/no_match_is_noop", test_replace_in_no_match_is_noop);

    g_test_add_func ("/strfn/add_number/basic", test_add_number_basic);
    g_test_add_func ("/strfn/add_number/zero_pads", test_add_number_zero_pads);

    g_test_add_func ("/strfn/case/upper_lower", test_to_upper_lower);

    g_test_add_func ("/strfn/process_filename/name_only", test_process_filename_applies_to_name_only);
    g_test_add_func ("/strfn/process_filename/ext_only", test_process_filename_applies_to_ext_only);
    g_test_add_func ("/strfn/process_filename/both", test_process_filename_applies_to_both);

    return g_test_run ();
}
