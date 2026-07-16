/**
 * @file  test_rconfig.c
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
 * @brief  Unit tests for rconfig.c
 *
 * Like test_rsettings.c, RenData is built by hand on the stack (with
 * rd.names left NULL, since rconfig.c never touches the file list) rather
 * than via rendata_new()/rendata_free(), avoiding an unnecessary dependency
 * on rfnames.c/rfitem.c/imgs.c for this GTK-independent logic.
 *
 * rconfig_write()/rconfig_parse() (the low-level, path-explicit API) are
 * exercised against a private tmp directory. rconfig_save()/rconfig_load()
 * (the high-level API hardcoded to the XDG config path) are exercised too,
 * with $XDG_CONFIG_HOME redirected to that same tmp directory so the real
 * user config file is never touched.
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#include <glib.h>
#include <glib/gstdio.h>
#include <string.h>
#include "rconfig.h"
#include "rendata.h"
/*----------------------------------------------------------------------------*/
static char *test_tmp_dir = NULL;
/*----------------------------------------------------------------------------*/
static char *
test_path (const char *name)
{
    return g_build_filename (test_tmp_dir, name, NULL);
}
/*----------------------------------------------------------------------------*/
static void
fixture_init (RenData *rd)
{
    rd->names       = NULL;
    rd->del         = rdelete_new ();
    rd->ins         = rinsovr_new ();
    rd->ovrw        = rinsovr_new ();
    rd->replace     = rreplace_new ();
    rd->number      = rnumber_new ();
    rd->uplo        = DEF_UPLO;
    rd->spaces      = DEF_SPACES;
    rd->applyto     = DEF_APPLTO;
    rd->renexit     = DEF_RENEXT;
    rd->dirsel      = DEF_DIRSEL;
    rd->rememberopt = DEF_REMEMBEROPT;
}
/*----------------------------------------------------------------------------*/
static void
fixture_clear (RenData *rd)
{
    rdelete_free  (rd->del);
    rinsovr_free  (rd->ins);
    rinsovr_free  (rd->ovrw);
    rreplace_free (rd->replace);
    rnumber_free  (rd->number);
}
/*----------------------------------------------------------------------------*/
static void
assert_rendata_equal (const RenData *a, const RenData *b)
{
    g_assert_cmpint (rendata_get_uplo (a), ==, rendata_get_uplo (b));
    g_assert_cmpint (rendata_get_spaces (a), ==, rendata_get_spaces (b));
    g_assert_cmpint (rendata_get_applyto (a), ==, rendata_get_applyto (b));
    g_assert_cmpint (rendata_get_renexit (a), ==, rendata_get_renexit (b));
    g_assert_cmpint (rendata_get_dirsel (a), ==, rendata_get_dirsel (b));
    g_assert_cmpint (rendata_get_rememberopt (a), ==, rendata_get_rememberopt (b));

    g_assert_cmpuint (rdelete_get_cnt (rendata_get_rdelete (a)), ==,
                       rdelete_get_cnt (rendata_get_rdelete (b)));
    g_assert_cmpuint (rdelete_get_pos (rendata_get_rdelete (a)), ==,
                       rdelete_get_pos (rendata_get_rdelete (b)));

    g_assert_cmpstr (rinsovr_get_text (rendata_get_rinsert (a)), ==,
                      rinsovr_get_text (rendata_get_rinsert (b)));
    g_assert_cmpuint (rinsovr_get_pos (rendata_get_rinsert (a)), ==,
                       rinsovr_get_pos (rendata_get_rinsert (b)));

    g_assert_cmpstr (rinsovr_get_text (rendata_get_roverwr (a)), ==,
                      rinsovr_get_text (rendata_get_roverwr (b)));
    g_assert_cmpuint (rinsovr_get_pos (rendata_get_roverwr (a)), ==,
                       rinsovr_get_pos (rendata_get_roverwr (b)));

    g_assert_cmpstr (rreplace_get_from (rendata_get_rreplace (a)), ==,
                      rreplace_get_from (rendata_get_rreplace (b)));
    g_assert_cmpstr (rreplace_get_to (rendata_get_rreplace (a)), ==,
                      rreplace_get_to (rendata_get_rreplace (b)));

    g_assert_cmpint (rnumber_get_opt (rendata_get_rnumber (a)), ==,
                      rnumber_get_opt (rendata_get_rnumber (b)));
    g_assert_cmpuint (rnumber_get_start (rendata_get_rnumber (a)), ==,
                       rnumber_get_start (rendata_get_rnumber (b)));
    g_assert_cmpuint (rnumber_get_pos (rendata_get_rnumber (a)), ==,
                       rnumber_get_pos (rendata_get_rnumber (b)));
}
/*----------------------------------------------------------------------------*/
static void
test_rconfig_roundtrip_defaults (void)
{
    RenData  a, b;
    char    *path = test_path ("roundtrip_defaults.json");

    fixture_init (&a);
    fixture_init (&b);

    g_assert_true (rconfig_write (&a, path));
    g_assert_true (rconfig_parse (&b, path));

    assert_rendata_equal (&a, &b);

    g_free (path);
    fixture_clear (&a);
    fixture_clear (&b);
}
/*----------------------------------------------------------------------------*/
static void
test_rconfig_roundtrip_nondefaults (void)
{
    RenData  a, b;
    char    *path = test_path ("roundtrip_nondefaults.json");

    fixture_init (&a);
    fixture_init (&b);

    rendata_set_uplo        (&a, 0);
    rendata_set_spaces      (&a, 1);
    rendata_set_applyto     (&a, 0);
    rendata_set_renexit     (&a, 0);
    rendata_set_dirsel      (&a, 0);
    rendata_set_rememberopt (&a, 1);

    rdelete_set_cnt (a.del, 0);
    rdelete_set_pos (a.del, FN_LEN);

    rinsovr_set_text (a.ins, "prefix_");
    rinsovr_set_pos  (a.ins, 0);

    rinsovr_set_text (a.ovrw, "");
    rinsovr_set_pos  (a.ovrw, FN_LEN);

    rreplace_set_from (a.replace, "foo");
    rreplace_set_to   (a.replace, "bar");

    rnumber_set_opt   (a.number, 1);
    rnumber_set_start (a.number, 1000000);
    rnumber_set_pos   (a.number, 3);

    g_assert_true (rconfig_write (&a, path));
    g_assert_true (rconfig_parse (&b, path));

    assert_rendata_equal (&a, &b);

    g_free (path);
    fixture_clear (&a);
    fixture_clear (&b);
}
/*----------------------------------------------------------------------------*/
static void
test_rconfig_roundtrip_utf8_and_escaping (void)
{
    RenData  a, b;
    char    *path = test_path ("roundtrip_utf8.json");

    fixture_init (&a);
    fixture_init (&b);

    /* "café_" with a quote, a backslash, and a newline mixed in. */
    rinsovr_set_text  (a.ins, "caf\xc3\xa9_\"quoted\"\\back\nslash");
    rreplace_set_from (a.replace, "caf\xc3\xa9_from");
    rreplace_set_to   (a.replace, "\"to\"\\value");

    g_assert_true (rconfig_write (&a, path));
    g_assert_true (rconfig_parse (&b, path));

    assert_rendata_equal (&a, &b);

    g_free (path);
    fixture_clear (&a);
    fixture_clear (&b);
}
/*----------------------------------------------------------------------------*/
static void
test_rconfig_load_missing_file (void)
{
    RenData  rd;
    char    *path = test_path ("does_not_exist.json");

    fixture_init (&rd);

    g_test_expect_message (NULL, G_LOG_LEVEL_WARNING, "*could not read config file*");
    g_assert_false (rconfig_parse (&rd, path));
    g_test_assert_expected_messages ();

    g_free (path);
    fixture_clear (&rd);
}
/*----------------------------------------------------------------------------*/
static void
test_rconfig_load_corrupt_file (void)
{
    RenData  rd;
    char    *path = test_path ("corrupt.json");

    fixture_init (&rd);
    rendata_set_uplo (&rd, 1);
    rdelete_set_cnt (rd.del, 7);
    rinsovr_set_text (rd.ins, "sentinel");

    g_assert_true (g_file_set_contents (path,
            "{\"uplo\": 2, \"delete\": {\"cnt\": ", -1, NULL));

    g_test_expect_message (NULL, G_LOG_LEVEL_WARNING, "*malformed*");
    g_assert_false (rconfig_parse (&rd, path));
    g_test_assert_expected_messages ();

    /* Two-phase parse-then-apply: a malformed file must leave rd untouched. */
    g_assert_cmpint (rendata_get_uplo (&rd), ==, 1);
    g_assert_cmpuint (rdelete_get_cnt (rd.del), ==, 7);
    g_assert_cmpstr (rinsovr_get_text (rd.ins), ==, "sentinel");

    g_free (path);
    fixture_clear (&rd);
}
/*----------------------------------------------------------------------------*/
static void
test_rconfig_save_toggles_file_existence (void)
{
    RenData   rd;
    char     *path = rconfig_get_path ();

    fixture_init (&rd);

    rendata_set_rememberopt (&rd, 1);
    rinsovr_set_text (rd.ins, "remembered");
    g_assert_true (rconfig_save (&rd));
    g_assert_true (g_file_test (path, G_FILE_TEST_EXISTS));

    fixture_clear (&rd);
    fixture_init (&rd);
    g_assert_true (rconfig_load (&rd));
    g_assert_cmpstr (rinsovr_get_text (rd.ins), ==, "remembered");

    rendata_set_rememberopt (&rd, 0);
    g_assert_true (rconfig_save (&rd));
    g_assert_false (g_file_test (path, G_FILE_TEST_EXISTS));

    g_free (path);
    fixture_clear (&rd);
}
/*----------------------------------------------------------------------------*/
int
main (int argc, char *argv[])
{
    int status;

    g_test_init (&argc, &argv, NULL);

    test_tmp_dir = g_dir_make_tmp ("sfrename_test_rconfig_XXXXXX", NULL);
    g_assert_nonnull (test_tmp_dir);
    g_setenv ("XDG_CONFIG_HOME", test_tmp_dir, TRUE);

    g_test_add_func ("/rconfig/roundtrip/defaults", test_rconfig_roundtrip_defaults);
    g_test_add_func ("/rconfig/roundtrip/nondefaults", test_rconfig_roundtrip_nondefaults);
    g_test_add_func ("/rconfig/roundtrip/utf8_and_escaping", test_rconfig_roundtrip_utf8_and_escaping);
    g_test_add_func ("/rconfig/load/missing_file", test_rconfig_load_missing_file);
    g_test_add_func ("/rconfig/load/corrupt_file", test_rconfig_load_corrupt_file);
    g_test_add_func ("/rconfig/save/toggles_file_existence", test_rconfig_save_toggles_file_existence);

    status = g_test_run ();

    return status;
}
