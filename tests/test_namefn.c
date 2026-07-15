/**
 * @file  test_namefn.c
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
 * @brief  Unit tests for namefn.c
 *
 * These tests build RenData/RFnames/RFitem and settings structs by hand,
 * without going through rendata_new()/rfitem_new_from_gfile(). Those
 * constructors create real GTK widgets and need a display; namefn.c never
 * touches the widget fields (it only reads/writes the plain s_new buffer
 * and the settings getters), so a zero-initialized fixture is sufficient
 * and keeps these tests headless.
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#include <glib.h>
#include <string.h>
#include "namefn.h"
#include "rendata.h"
/*----------------------------------------------------------------------------*/
#define FIXTURE_MAX_ITEMS 8

typedef struct {
    RenData  rd;
    RFnames  names;
    RFitem   items[FIXTURE_MAX_ITEMS];
    RFitem  *item_ptrs[FIXTURE_MAX_ITEMS];
    char     bufs[FIXTURE_MAX_ITEMS][FN_LEN + 1];
    RDelete  del;
    RInsOvr  ins;
    RInsOvr  ovrw;
    RReplace replace;
    RNumber  number;
} Fixture;
/*----------------------------------------------------------------------------*/
static void
fixture_init (Fixture *f, const char *const *names, size_t n)
{
    memset (f, 0, sizeof (*f));

    for (size_t i = 0; i < n; ++i) {
        strcpy (f->bufs[i], names[i]);
        f->items[i].s_new     = f->bufs[i];
        f->items[i].new_len   = strlen (f->bufs[i]);
        f->items[i].new_u8len = (size_t) g_utf8_strlen (f->bufs[i], -1);
        f->item_ptrs[i] = &f->items[i];
    }
    f->names.cnt      = (uint_fast32_t) n;
    f->names.rf_items = f->item_ptrs;

    f->rd.names   = &f->names;
    f->rd.del     = &f->del;
    f->rd.ins     = &f->ins;
    f->rd.ovrw    = &f->ovrw;
    f->rd.replace = &f->replace;
    f->rd.number  = &f->number;
    f->rd.uplo    = 2; /* disabled */
    f->rd.spaces  = 2; /* disabled */
    f->rd.applyto = 2; /* name + ext */
}
/*----------------------------------------------------------------------------*/
static void
test_delete_chars (void)
{
    Fixture f;
    const char *names[] = { "hello world" };
    fixture_init (&f, names, 1);

    rdelete_set_pos (&f.del, 5);
    rdelete_set_cnt (&f.del, 6);

    name_delete_chars (&f.rd, 0);

    g_assert_cmpstr (f.items[0].s_new, ==, "hello");
}
/*----------------------------------------------------------------------------*/
static void
test_delete_chars_noop_when_count_zero (void)
{
    Fixture f;
    const char *names[] = { "hello" };
    fixture_init (&f, names, 1);
    /* del.cnt defaults to 0 */

    name_delete_chars (&f.rd, 0);

    g_assert_cmpstr (f.items[0].s_new, ==, "hello");
}
/*----------------------------------------------------------------------------*/
static void
test_insert_string (void)
{
    Fixture f;
    const char *names[] = { "helloworld" };
    fixture_init (&f, names, 1);

    rinsovr_set_text (&f.ins, " ");
    rinsovr_set_pos (&f.ins, 5);

    name_insert_string (&f.rd, 0);

    g_assert_cmpstr (f.items[0].s_new, ==, "hello world");
}
/*----------------------------------------------------------------------------*/
static void
test_insert_string_noop_when_text_empty (void)
{
    Fixture f;
    const char *names[] = { "unchanged" };
    fixture_init (&f, names, 1);
    /* ins.s_text defaults to "" */

    name_insert_string (&f.rd, 0);

    g_assert_cmpstr (f.items[0].s_new, ==, "unchanged");
}
/*----------------------------------------------------------------------------*/
static void
test_overwrite_string (void)
{
    Fixture f;
    const char *names[] = { "hello world" };
    fixture_init (&f, names, 1);

    rinsovr_set_text (&f.ovrw, "THERE");
    rinsovr_set_pos (&f.ovrw, 6);

    name_overwrite_string (&f.rd, 0);

    g_assert_cmpstr (f.items[0].s_new, ==, "hello THERE");
}
/*----------------------------------------------------------------------------*/
static void
test_number_string_sequence (void)
{
    Fixture f;
    const char *names[] = { "file", "file", "file" };
    fixture_init (&f, names, 3);

    rnumber_set_opt (&f.number, 1);
    rnumber_set_start (&f.number, 1);
    rnumber_set_pos (&f.number, 4); /* after "file" */

    name_number_string (&f.rd, 0);
    name_number_string (&f.rd, 1);
    name_number_string (&f.rd, 2);

    g_assert_cmpstr (f.items[0].s_new, ==, "file1");
    g_assert_cmpstr (f.items[1].s_new, ==, "file2");
    g_assert_cmpstr (f.items[2].s_new, ==, "file3");
}
/*----------------------------------------------------------------------------*/
static void
test_number_string_noop_when_disabled (void)
{
    Fixture f;
    const char *names[] = { "file" };
    fixture_init (&f, names, 1);
    /* number.opt defaults to 0 == disabled */

    name_number_string (&f.rd, 0);

    g_assert_cmpstr (f.items[0].s_new, ==, "file");
}
/*----------------------------------------------------------------------------*/
static void
test_to_upcase (void)
{
    Fixture f;
    const char *names[] = { "Hello" };
    fixture_init (&f, names, 1);
    f.rd.uplo = 0;

    name_to_upcase_lowercase (&f.rd, 0);

    g_assert_cmpstr (f.items[0].s_new, ==, "HELLO");
}
/*----------------------------------------------------------------------------*/
static void
test_to_lowercase (void)
{
    Fixture f;
    const char *names[] = { "Hello" };
    fixture_init (&f, names, 1);
    f.rd.uplo = 1;

    name_to_upcase_lowercase (&f.rd, 0);

    g_assert_cmpstr (f.items[0].s_new, ==, "hello");
}
/*----------------------------------------------------------------------------*/
static void
test_to_upcase_lowercase_noop_when_disabled (void)
{
    Fixture f;
    const char *names[] = { "Hello" };
    fixture_init (&f, names, 1);
    /* uplo defaults to 2 == disabled */

    name_to_upcase_lowercase (&f.rd, 0);

    g_assert_cmpstr (f.items[0].s_new, ==, "Hello");
}
/*----------------------------------------------------------------------------*/
static void
test_spaces_to_underscores (void)
{
    Fixture f;
    const char *names[] = { "my file name" };
    fixture_init (&f, names, 1);
    f.rd.spaces = 1;

    name_spaces_underscores (&f.rd, 0);

    g_assert_cmpstr (f.items[0].s_new, ==, "my_file_name");
}
/*----------------------------------------------------------------------------*/
static void
test_underscores_to_spaces (void)
{
    Fixture f;
    const char *names[] = { "my_file_name" };
    fixture_init (&f, names, 1);
    f.rd.spaces = 0;

    name_spaces_underscores (&f.rd, 0);

    g_assert_cmpstr (f.items[0].s_new, ==, "my file name");
}
/*----------------------------------------------------------------------------*/
static void
test_replace_strings (void)
{
    Fixture f;
    const char *names[] = { "foo_bar_foo" };
    fixture_init (&f, names, 1);

    rreplace_set_from (&f.replace, "foo");
    rreplace_set_to (&f.replace, "baz");

    name_replace_strings (&f.rd, 0);

    g_assert_cmpstr (f.items[0].s_new, ==, "baz_bar_baz");
}
/*----------------------------------------------------------------------------*/
static void
test_replace_strings_noop_when_from_empty (void)
{
    Fixture f;
    const char *names[] = { "unchanged" };
    fixture_init (&f, names, 1);
    /* replace.s_from defaults to "" */

    name_replace_strings (&f.rd, 0);

    g_assert_cmpstr (f.items[0].s_new, ==, "unchanged");
}
/*----------------------------------------------------------------------------*/
static void
test_applyto_ext_only (void)
{
    Fixture f;
    const char *names[] = { "Report.pdf" };
    fixture_init (&f, names, 1);
    f.rd.applyto = 0; /* ext only */
    f.rd.uplo = 0;    /* upper */

    name_to_upcase_lowercase (&f.rd, 0);

    g_assert_cmpstr (f.items[0].s_new, ==, "Report.PDF");
}
/*----------------------------------------------------------------------------*/
static void
test_applyto_name_only (void)
{
    Fixture f;
    const char *names[] = { "Report.pdf" };
    fixture_init (&f, names, 1);
    f.rd.applyto = 1; /* name only */
    f.rd.uplo = 0;    /* upper */

    name_to_upcase_lowercase (&f.rd, 0);

    g_assert_cmpstr (f.items[0].s_new, ==, "REPORT.pdf");
}
/*----------------------------------------------------------------------------*/
int
main (int argc, char *argv[])
{
    g_test_init (&argc, &argv, NULL);

    g_test_add_func ("/namefn/delete_chars/basic", test_delete_chars);
    g_test_add_func ("/namefn/delete_chars/noop_when_count_zero", test_delete_chars_noop_when_count_zero);

    g_test_add_func ("/namefn/insert_string/basic", test_insert_string);
    g_test_add_func ("/namefn/insert_string/noop_when_text_empty", test_insert_string_noop_when_text_empty);

    g_test_add_func ("/namefn/overwrite_string/basic", test_overwrite_string);

    g_test_add_func ("/namefn/number_string/sequence", test_number_string_sequence);
    g_test_add_func ("/namefn/number_string/noop_when_disabled", test_number_string_noop_when_disabled);

    g_test_add_func ("/namefn/case/upper", test_to_upcase);
    g_test_add_func ("/namefn/case/lower", test_to_lowercase);
    g_test_add_func ("/namefn/case/noop_when_disabled", test_to_upcase_lowercase_noop_when_disabled);

    g_test_add_func ("/namefn/spaces_underscores/spaces_to_underscores", test_spaces_to_underscores);
    g_test_add_func ("/namefn/spaces_underscores/underscores_to_spaces", test_underscores_to_spaces);

    g_test_add_func ("/namefn/replace_strings/basic", test_replace_strings);
    g_test_add_func ("/namefn/replace_strings/noop_when_from_empty", test_replace_strings_noop_when_from_empty);

    g_test_add_func ("/namefn/applyto/ext_only", test_applyto_ext_only);
    g_test_add_func ("/namefn/applyto/name_only", test_applyto_name_only);

    return g_test_run ();
}
