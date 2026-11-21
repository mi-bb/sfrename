/**
 * @file  test_rrename.c
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
 * @brief  Unit tests for rrename.c and the widget-free state of RFitem
 *
 * RFitem objects are built by hand with all widget pointers null, so the
 * tests need no display. Files are created in a temporary directory.
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <glib.h>
#include <glib/gstdio.h>
#include <string.h>
#include "rrename.h"
/*----------------------------------------------------------------------------*/
typedef struct {
    RFitem  item;
    char    new_name[FN_U8_LEN + 1];
    char    org_name[FN_LEN + 1];
    char    path[1024];
    char   *dir;
} Fixture;
/*----------------------------------------------------------------------------*/
static void
touch (const Fixture *f, const char *name, const char *content)
{
    g_autofree char *file = g_build_filename (f->dir, name, nullptr);
    g_assert_true (g_file_set_contents (file, content, -1, nullptr));
}
/*----------------------------------------------------------------------------*/
static bool
exists (const Fixture *f, const char *name)
{
    g_autofree char *file = g_build_filename (f->dir, name, nullptr);
    return g_file_test (file, G_FILE_TEST_EXISTS);
}
/*----------------------------------------------------------------------------*/
static char *
content_of (const Fixture *f, const char *name)
{
    g_autofree char *file = g_build_filename (f->dir, name, nullptr);
    char *text = nullptr;
    g_assert_true (g_file_get_contents (file, &text, nullptr, nullptr));
    return text;
}
/*----------------------------------------------------------------------------*/
static void
fixture_setup (Fixture *f, gconstpointer user_data)
{
    const char *name = user_data != nullptr ? user_data : "old.txt";

    *f = (Fixture){};
    f->dir = g_dir_make_tmp ("sfrename-test-XXXXXX", nullptr);
    g_assert_nonnull (f->dir);
    g_snprintf (f->path, sizeof (f->path), "%s/", f->dir);

    f->item.new_name = f->new_name;
    f->item.org_name = f->org_name;
    f->item.path     = f->path;
    f->item.checked  = true;
    rfitem_set_org_name (&f->item, name);
    rfitem_set_new_name_from_org_name (&f->item);

    touch (f, name, "original");
}
/*----------------------------------------------------------------------------*/
static void
fixture_teardown (Fixture *f, [[maybe_unused]] gconstpointer user_data)
{
    GDir       *dir = g_dir_open (f->dir, 0, nullptr);
    const char *name;

    g_assert_nonnull (dir);
    while ((name = g_dir_read_name (dir)) != nullptr) {
        g_autofree char *file = g_build_filename (f->dir, name, nullptr);
        g_assert_cmpint (g_remove (file), ==, 0);
    }
    g_dir_close (dir);
    g_assert_cmpint (g_rmdir (f->dir), ==, 0);
    g_free (f->dir);
}
/*----------------------------------------------------------------------------*/
static void
test_item_renames_checked (Fixture *f, [[maybe_unused]] gconstpointer data)
{
    rfitem_set_new_name (&f->item, "new.txt");

    g_assert_cmpint (rrename_item (&f->item), ==, REN_OK);

    g_assert_false (exists (f, "old.txt"));
    g_assert_true (exists (f, "new.txt"));
    g_assert_cmpstr (rfitem_get_org_name (&f->item), ==, "new.txt");
    g_assert_cmpuint (f->item.org_len, ==, strlen ("new.txt"));
}
/*----------------------------------------------------------------------------*/
static void
test_item_skips_unchecked (Fixture *f, [[maybe_unused]] gconstpointer data)
{
    rfitem_set_new_name (&f->item, "new.txt");
    rfitem_set_checked (&f->item, false);

    g_assert_cmpint (rrename_item (&f->item), ==, REN_SKIP);

    g_assert_true (exists (f, "old.txt"));
    g_assert_false (exists (f, "new.txt"));
    g_assert_cmpstr (rfitem_get_org_name (&f->item), ==, "old.txt");
    g_assert_cmpstr (rfitem_get_new_name (&f->item), ==, "new.txt");
}
/*----------------------------------------------------------------------------*/
static void
test_item_no_change (Fixture *f, [[maybe_unused]] gconstpointer data)
{
    g_assert_cmpint (rrename_item (&f->item), ==, REN_NC);

    g_assert_true (exists (f, "old.txt"));
    g_assert_cmpstr (rfitem_get_org_name (&f->item), ==, "old.txt");
}
/*----------------------------------------------------------------------------*/
static void
test_item_target_exists (Fixture *f, [[maybe_unused]] gconstpointer data)
{
    g_autofree char *kept = nullptr;

    touch (f, "new.txt", "other file");
    rfitem_set_new_name (&f->item, "new.txt");

    g_assert_cmpint (rrename_item (&f->item), ==, REN_EXISTS);

    kept = content_of (f, "new.txt");
    g_assert_cmpstr (kept, ==, "other file");
    g_assert_true (exists (f, "old.txt"));
    g_assert_cmpstr (rfitem_get_new_name (&f->item), ==, "old.txt");
    g_assert_cmpstr (rfitem_get_org_name (&f->item), ==, "old.txt");
}
/*----------------------------------------------------------------------------*/
static void
test_item_missing_source (Fixture *f, [[maybe_unused]] gconstpointer data)
{
    g_autofree char *file = g_build_filename (f->dir, "old.txt", nullptr);

    g_assert_cmpint (g_remove (file), ==, 0);
    rfitem_set_new_name (&f->item, "new.txt");

    g_assert_cmpint (rrename_item (&f->item), ==, REN_NOT_REN);

    g_assert_false (exists (f, "new.txt"));
    g_assert_cmpstr (rfitem_get_new_name (&f->item), ==, "old.txt");
}
/*----------------------------------------------------------------------------*/
static void
test_item_too_long_is_not_shortened (Fixture *f,
                                     [[maybe_unused]] gconstpointer data)
{
    GString *name = g_string_new (nullptr);

    /* 255 characters, 1020 bytes: fits the entry, not the file system */
    for (int i = 0; i < FN_LEN; ++i)
        g_string_append (name, "\xF0\x9D\x92\xB3");

    rfitem_set_new_name (&f->item, name->str);
    g_string_free (name, TRUE);

    g_assert_cmpint (rrename_item (&f->item), ==, REN_NOT_REN);

    g_assert_true (exists (f, "old.txt"));
    g_assert_cmpstr (rfitem_get_org_name (&f->item), ==, "old.txt");
    g_assert_cmpstr (rfitem_get_new_name (&f->item), ==, "old.txt");
}
/*----------------------------------------------------------------------------*/
static void
test_new_name_lengths (Fixture *f, [[maybe_unused]] gconstpointer data)
{
    rfitem_set_new_name (&f->item, "za\xC5\xBC\xC3\xB3\xC5\x82\xC4\x87");

    g_assert_cmpuint (f->item.new_len, ==, 10);
    g_assert_cmpuint (f->item.new_u8len, ==, 6);
}
/*----------------------------------------------------------------------------*/
static void
test_new_name_keeps_full_utf8 (Fixture *f, [[maybe_unused]] gconstpointer data)
{
    GString *name = g_string_new (nullptr);

    for (int i = 0; i < FN_LEN; ++i)
        g_string_append (name, "\xF0\x9D\x92\xB3");

    rfitem_set_new_name (&f->item, name->str);

    g_assert_cmpuint (f->item.new_len, ==, FN_U8_LEN);
    g_assert_cmpuint (f->item.new_u8len, ==, FN_LEN);
    g_assert_cmpstr (f->item.new_name, ==, name->str);
    g_string_free (name, TRUE);
}
/*----------------------------------------------------------------------------*/
static void
test_new_name_truncates_on_boundary (Fixture *f,
                                     [[maybe_unused]] gconstpointer data)
{
    GString *name = g_string_new ("a");

    /* The FN_U8_LEN limit falls inside a 4-byte character */
    for (int i = 0; i < FN_LEN; ++i)
        g_string_append (name, "\xF0\x9D\x92\xB3");

    rfitem_set_new_name (&f->item, name->str);
    g_string_free (name, TRUE);

    g_assert_cmpuint (f->item.new_len, ==, FN_U8_LEN - 3);
    g_assert_true (g_utf8_validate (f->item.new_name, -1, nullptr));
}
/*----------------------------------------------------------------------------*/
static void
test_org_from_new_stays_in_buffer (Fixture *f,
                                   [[maybe_unused]] gconstpointer data)
{
    g_autofree char *name = g_strnfill (FN_U8_LEN, 'a');

    rfitem_set_new_name (&f->item, name);
    rfitem_set_org_name_from_new_name (&f->item);

    g_assert_cmpuint (f->item.org_len, ==, FN_LEN);
    g_assert_cmpuint (strlen (f->item.org_name), ==, FN_LEN);
}
/*----------------------------------------------------------------------------*/
static void
test_checked_without_widgets (Fixture *f, [[maybe_unused]] gconstpointer data)
{
    g_assert_true (rfitem_get_checked (&f->item));

    rfitem_set_checked (&f->item, false);
    g_assert_false (rfitem_get_checked (&f->item));

    rfitem_invert_checked (&f->item);
    g_assert_true (rfitem_get_checked (&f->item));
}
/*----------------------------------------------------------------------------*/
static void
test_restore_without_widgets (Fixture *f, [[maybe_unused]] gconstpointer data)
{
    rfitem_set_new_name (&f->item, "edited.txt");

    rfitem_entry_restore (&f->item);

    g_assert_cmpstr (rfitem_get_new_name (&f->item), ==, "old.txt");
    g_assert_cmpuint (f->item.new_len, ==, strlen ("old.txt"));
}
/*----------------------------------------------------------------------------*/
#define ADD_TEST(path, func) \
    g_test_add (path, Fixture, nullptr, fixture_setup, func, fixture_teardown)

int
main (int argc, char **argv)
{
    g_test_init (&argc, &argv, nullptr);

    ADD_TEST ("/rrename/item/renames_checked", test_item_renames_checked);
    ADD_TEST ("/rrename/item/skips_unchecked", test_item_skips_unchecked);
    ADD_TEST ("/rrename/item/no_change", test_item_no_change);
    ADD_TEST ("/rrename/item/target_exists", test_item_target_exists);
    ADD_TEST ("/rrename/item/missing_source", test_item_missing_source);
    ADD_TEST ("/rrename/item/too_long_is_not_shortened",
              test_item_too_long_is_not_shortened);
    ADD_TEST ("/rfitem/new_name/lengths", test_new_name_lengths);
    ADD_TEST ("/rfitem/new_name/keeps_full_utf8",
              test_new_name_keeps_full_utf8);
    ADD_TEST ("/rfitem/new_name/truncates_on_boundary",
              test_new_name_truncates_on_boundary);
    ADD_TEST ("/rfitem/org_name/from_new_stays_in_buffer",
              test_org_from_new_stays_in_buffer);
    ADD_TEST ("/rfitem/checked/without_widgets", test_checked_without_widgets);
    ADD_TEST ("/rfitem/restore/without_widgets", test_restore_without_widgets);

    return g_test_run ();
}
/*----------------------------------------------------------------------------*/

