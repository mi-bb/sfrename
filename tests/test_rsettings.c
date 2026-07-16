/**
 * @file  test_rsettings.c
 * @copyright Copyright (C) 2019-2020 Michal Babik
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
 * @brief  Unit tests for rdelete.c, rinsovr.c, rreplace.c and rnumber.c
 *
 * Unlike RenData/RFnames/RFitem, these settings structs hold no GTK widgets,
 * so they can be exercised through their real rXXX_new()/rXXX_free()
 * constructors instead of a hand-built fixture.
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#include <glib.h>
#include <string.h>
#include "rdelete.h"
#include "rinsovr.h"
#include "rreplace.h"
#include "rnumber.h"
/*----------------------------------------------------------------------------*/
static void
test_rdelete_defaults (void)
{
    RDelete *d = rdelete_new ();

    g_assert_cmpuint (rdelete_get_cnt (d), ==, 0);
    g_assert_cmpuint (rdelete_get_pos (d), ==, 0);

    rdelete_free (d);
}
/*----------------------------------------------------------------------------*/
static void
test_rdelete_set_get (void)
{
    RDelete *d = rdelete_new ();

    rdelete_set_cnt (d, 6);
    rdelete_set_pos (d, 5);

    g_assert_cmpuint (rdelete_get_cnt (d), ==, 6);
    g_assert_cmpuint (rdelete_get_pos (d), ==, 5);

    rdelete_free (d);
}
/*----------------------------------------------------------------------------*/
static void
test_rinsovr_defaults (void)
{
    RInsOvr *r = rinsovr_new ();

    g_assert_cmpstr (rinsovr_get_text (r), ==, "");
    g_assert_cmpuint (rinsovr_get_pos (r), ==, 0);
    g_assert_cmpuint (rinsovr_get_len (r), ==, 0);
    g_assert_cmpuint (rinsovr_get_u8len (r), ==, 0);
    g_assert_true (rinsovr_empty (r));

    rinsovr_free (r);
}
/*----------------------------------------------------------------------------*/
static void
test_rinsovr_set_text (void)
{
    RInsOvr *r = rinsovr_new ();

    rinsovr_set_text (r, "hello");

    g_assert_cmpstr (rinsovr_get_text (r), ==, "hello");
    g_assert_cmpuint (rinsovr_get_len (r), ==, 5);
    g_assert_cmpuint (rinsovr_get_u8len (r), ==, 5);
    g_assert_false (rinsovr_empty (r));

    rinsovr_free (r);
}
/*----------------------------------------------------------------------------*/
static void
test_rinsovr_set_text_utf8 (void)
{
    RInsOvr *r = rinsovr_new ();

    /* "wörld": 6 bytes (ö is 2-byte UTF-8), 5 unicode chars. */
    rinsovr_set_text (r, "w\xc3\xb6rld");

    g_assert_cmpuint (rinsovr_get_len (r), ==, 6);
    g_assert_cmpuint (rinsovr_get_u8len (r), ==, 5);

    rinsovr_free (r);
}
/*----------------------------------------------------------------------------*/
static void
test_rinsovr_set_text_truncates_at_max (void)
{
    RInsOvr *r = rinsovr_new ();
    char     s[FN_LEN + 100];

    memset (s, 'a', sizeof (s) - 1);
    s[sizeof (s) - 1] = '\0';

    rinsovr_set_text (r, s);

    g_assert_cmpuint (rinsovr_get_len (r), <=, FN_LEN);
    g_assert_cmpuint (strlen (rinsovr_get_text (r)), <=, FN_LEN);

    rinsovr_free (r);
}
/*----------------------------------------------------------------------------*/
static void
test_rinsovr_set_pos (void)
{
    RInsOvr *r = rinsovr_new ();

    rinsovr_set_pos (r, 42);

    g_assert_cmpuint (rinsovr_get_pos (r), ==, 42);

    rinsovr_free (r);
}
/*----------------------------------------------------------------------------*/
static void
test_rreplace_defaults (void)
{
    RReplace *rr = rreplace_new ();

    g_assert_cmpstr (rreplace_get_from (rr), ==, "");
    g_assert_cmpstr (rreplace_get_to (rr), ==, "");
    g_assert_cmpuint (rreplace_get_from_len (rr), ==, 0);
    g_assert_cmpuint (rreplace_get_from_u8len (rr), ==, 0);
    g_assert_cmpuint (rreplace_get_to_len (rr), ==, 0);
    g_assert_cmpuint (rreplace_get_to_u8len (rr), ==, 0);
    g_assert_true (rreplace_empty_from (rr));
    g_assert_true (rreplace_empty_to (rr));

    rreplace_free (rr);
}
/*----------------------------------------------------------------------------*/
static void
test_rreplace_set_from_to (void)
{
    RReplace *rr = rreplace_new ();

    rreplace_set_from (rr, "foo");
    rreplace_set_to (rr, "baz");

    g_assert_cmpstr (rreplace_get_from (rr), ==, "foo");
    g_assert_cmpstr (rreplace_get_to (rr), ==, "baz");
    g_assert_cmpuint (rreplace_get_from_len (rr), ==, 3);
    g_assert_cmpuint (rreplace_get_from_u8len (rr), ==, 3);
    g_assert_cmpuint (rreplace_get_to_len (rr), ==, 3);
    g_assert_cmpuint (rreplace_get_to_u8len (rr), ==, 3);
    g_assert_false (rreplace_empty_from (rr));
    g_assert_false (rreplace_empty_to (rr));

    rreplace_free (rr);
}
/*----------------------------------------------------------------------------*/
static void
test_rreplace_set_from_utf8 (void)
{
    RReplace *rr = rreplace_new ();

    /* "café": 5 bytes (é is 2-byte UTF-8), 4 unicode chars. */
    rreplace_set_from (rr, "caf\xc3\xa9");

    g_assert_cmpuint (rreplace_get_from_len (rr), ==, 5);
    g_assert_cmpuint (rreplace_get_from_u8len (rr), ==, 4);

    rreplace_free (rr);
}
/*----------------------------------------------------------------------------*/
static void
test_rreplace_set_truncates_at_max (void)
{
    RReplace *rr = rreplace_new ();
    char      s[FN_LEN + 100];

    memset (s, 'b', sizeof (s) - 1);
    s[sizeof (s) - 1] = '\0';

    rreplace_set_from (rr, s);
    rreplace_set_to (rr, s);

    g_assert_cmpuint (rreplace_get_from_len (rr), <=, FN_LEN);
    g_assert_cmpuint (rreplace_get_to_len (rr), <=, FN_LEN);

    rreplace_free (rr);
}
/*----------------------------------------------------------------------------*/
static void
test_rnumber_defaults (void)
{
    RNumber *n = rnumber_new ();

    g_assert_cmpint (rnumber_get_opt (n), ==, 0);
    g_assert_cmpuint (rnumber_get_start (n), ==, 0);
    g_assert_cmpuint (rnumber_get_pos (n), ==, 0);

    rnumber_free (n);
}
/*----------------------------------------------------------------------------*/
static void
test_rnumber_set_get (void)
{
    RNumber *n = rnumber_new ();

    rnumber_set_opt (n, 1);
    rnumber_set_start (n, 10);
    rnumber_set_pos (n, 4);

    g_assert_cmpint (rnumber_get_opt (n), ==, 1);
    g_assert_cmpuint (rnumber_get_start (n), ==, 10);
    g_assert_cmpuint (rnumber_get_pos (n), ==, 4);

    rnumber_free (n);
}
/*----------------------------------------------------------------------------*/
int
main (int argc, char *argv[])
{
    g_test_init (&argc, &argv, NULL);

    g_test_add_func ("/rdelete/defaults", test_rdelete_defaults);
    g_test_add_func ("/rdelete/set_get", test_rdelete_set_get);

    g_test_add_func ("/rinsovr/defaults", test_rinsovr_defaults);
    g_test_add_func ("/rinsovr/set_text/basic", test_rinsovr_set_text);
    g_test_add_func ("/rinsovr/set_text/utf8", test_rinsovr_set_text_utf8);
    g_test_add_func ("/rinsovr/set_text/truncates_at_max", test_rinsovr_set_text_truncates_at_max);
    g_test_add_func ("/rinsovr/set_pos", test_rinsovr_set_pos);

    g_test_add_func ("/rreplace/defaults", test_rreplace_defaults);
    g_test_add_func ("/rreplace/set_from_to", test_rreplace_set_from_to);
    g_test_add_func ("/rreplace/set_from/utf8", test_rreplace_set_from_utf8);
    g_test_add_func ("/rreplace/set/truncates_at_max", test_rreplace_set_truncates_at_max);

    g_test_add_func ("/rnumber/defaults", test_rnumber_defaults);
    g_test_add_func ("/rnumber/set_get", test_rnumber_set_get);

    return g_test_run ();
}
