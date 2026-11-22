/**
 * @file  test_rfnames.c
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
 * @brief  Unit tests for the RFnames list and its model
 *
 * Items have no widgets until a list view binds them, so these tests need no
 * display. Files are created in a temporary directory.
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <glib.h>
#include <glib/gstdio.h>
#include <string.h>
#include "defs.h"
#include "rfnames.h"
/*----------------------------------------------------------------------------*/
static const char *const file_names[] = {"b.txt", ".hid", "a.txt"};
/*----------------------------------------------------------------------------*/
typedef struct {
    RFnames *names;
    char    *dir;
} Fixture;
/*----------------------------------------------------------------------------*/
static void
add_path (Fixture *f, const char *name)
{
    g_autofree char *path = g_build_filename (f->dir, name, nullptr);
    GFile           *file = g_file_new_for_path (path);

    rfnames_add_gfile (f->names, file);
    g_object_unref (file);
}
/*----------------------------------------------------------------------------*/
static void
fixture_setup (Fixture *f, [[maybe_unused]] gconstpointer data)
{
    g_autofree char *sub = nullptr;

    f->dir = g_dir_make_tmp ("sfrename-test-XXXXXX", nullptr);
    g_assert_nonnull (f->dir);
    f->names = rfnames_new ();

    for (size_t i = 0; i < G_N_ELEMENTS (file_names); ++i) {
        g_autofree char *path = g_build_filename (f->dir, file_names[i],
                                                  nullptr);
        g_assert_true (g_file_set_contents (path, "x", -1, nullptr));
    }
    sub = g_build_filename (f->dir, "dir", nullptr);
    g_assert_cmpint (g_mkdir (sub, 0755), ==, 0);

    for (size_t i = 0; i < G_N_ELEMENTS (file_names); ++i)
        add_path (f, file_names[i]);

    add_path (f, "dir");
}
/*----------------------------------------------------------------------------*/
static void
fixture_teardown (Fixture *f, [[maybe_unused]] gconstpointer data)
{
    g_autofree char *sub = g_build_filename (f->dir, "dir", nullptr);

    rfnames_free (f->names);

    for (size_t i = 0; i < G_N_ELEMENTS (file_names); ++i) {
        g_autofree char *path = g_build_filename (f->dir, file_names[i],
                                                  nullptr);
        g_assert_cmpint (g_remove (path), ==, 0);
    }
    g_assert_cmpint (g_rmdir (sub), ==, 0);
    g_assert_cmpint (g_rmdir (f->dir), ==, 0);
    g_free (f->dir);
}
/*----------------------------------------------------------------------------*/
static void
assert_model_matches (const RFnames *names)
{
    GListModel *model = G_LIST_MODEL (names->store);

    g_assert_cmpuint (g_list_model_get_n_items (model), ==, names->cnt);

    for (uint_fast32_t i = 0; i < names->cnt; ++i) {
        GObject *object = g_list_model_get_item (model, (guint) i);
        g_assert_true (object == names->items[i]->object);
        g_object_unref (object);
    }
}
/*----------------------------------------------------------------------------*/
static void
test_add_keeps_order (Fixture *f, [[maybe_unused]] gconstpointer data)
{
    g_assert_cmpuint (rfnames_get_cnt (f->names), ==, 4);
    g_assert_cmpstr (f->names->items[0]->org_name, ==, "b.txt");
    g_assert_cmpstr (f->names->items[3]->org_name, ==, "dir");
    g_assert_true (rfitem_get_checked (f->names->items[0]));
    g_assert_null (f->names->items[0]->entry);
    assert_model_matches (f->names);
}
/*----------------------------------------------------------------------------*/
static void
test_remove_middle (Fixture *f, [[maybe_unused]] gconstpointer data)
{
    rfnames_unselect_files (f->names);
    rfnames_unselect_folders (f->names);
    rfitem_set_checked (f->names->items[1], true);

    rfnames_remove_selected (f->names);

    g_assert_cmpuint (rfnames_get_cnt (f->names), ==, 3);
    g_assert_cmpstr (f->names->items[0]->org_name, ==, "b.txt");
    g_assert_cmpstr (f->names->items[1]->org_name, ==, "a.txt");
    g_assert_cmpstr (f->names->items[2]->org_name, ==, "dir");
    assert_model_matches (f->names);
}
/*----------------------------------------------------------------------------*/
static void
test_remove_by_type (Fixture *f, [[maybe_unused]] gconstpointer data)
{
    rfnames_remove_all_hidden (f->names);
    g_assert_cmpuint (rfnames_get_cnt (f->names), ==, 3);
    assert_model_matches (f->names);

    rfnames_remove_all_folders (f->names);
    g_assert_cmpuint (rfnames_get_cnt (f->names), ==, 2);
    assert_model_matches (f->names);

    rfnames_remove_all (f->names);
    g_assert_cmpuint (rfnames_get_cnt (f->names), ==, 0);
    assert_model_matches (f->names);
}
/*----------------------------------------------------------------------------*/
static void
test_sort_updates_model (Fixture *f, [[maybe_unused]] gconstpointer data)
{
    rfnames_sort (f->names);

    g_assert_cmpstr (f->names->items[0]->org_name, ==, "dir");
    g_assert_cmpstr (f->names->items[1]->org_name, ==, ".hid");
    g_assert_cmpstr (f->names->items[2]->org_name, ==, "a.txt");
    g_assert_cmpstr (f->names->items[3]->org_name, ==, "b.txt");
    assert_model_matches (f->names);
}
/*----------------------------------------------------------------------------*/
static void
test_restore_without_rows (Fixture *f, [[maybe_unused]] gconstpointer data)
{
    rfitem_set_new_name (f->names->items[0], "edited.txt");

    rfnames_restore_all (f->names);

    g_assert_cmpstr (rfitem_get_new_name (f->names->items[0]), ==, "b.txt");
}
/*----------------------------------------------------------------------------*/
static void
test_long_directory_path (void)
{
    g_autofree char *dir  = g_strnfill (FN_LEN * 3, 'd');
    g_autofree char *path = g_strconcat ("/", dir, "/name.txt", nullptr);
    RFitem          *item = rfitem_new_from_file_path (path);

    g_assert_cmpuint (strlen (rfitem_get_path (item)), ==, FN_LEN * 3 + 2);
    g_assert_cmpstr (rfitem_get_org_name (item), ==, "name.txt");

    rfitem_free (item);
}
/*----------------------------------------------------------------------------*/
#define ADD_TEST(path, func) \
    g_test_add (path, Fixture, nullptr, fixture_setup, func, fixture_teardown)

int
main (int argc, char **argv)
{
    g_test_init (&argc, &argv, nullptr);

    ADD_TEST ("/rfnames/add/keeps_order", test_add_keeps_order);
    ADD_TEST ("/rfnames/remove/middle", test_remove_middle);
    ADD_TEST ("/rfnames/remove/by_type", test_remove_by_type);
    ADD_TEST ("/rfnames/sort/updates_model", test_sort_updates_model);
    ADD_TEST ("/rfnames/restore/without_rows", test_restore_without_rows);
    g_test_add_func ("/rfitem/new/long_directory_path",
                     test_long_directory_path);

    return g_test_run ();
}
/*----------------------------------------------------------------------------*/

