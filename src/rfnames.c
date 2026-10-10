/**
 * @file  rfnames.c
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
 * @brief  RFnames struncture and functions.
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <stdint.h>
#include <err.h>
#include "defs.h"
#include "rfnames.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  RFnames initialization.
 *
 * @param[out] names Pointer to RFnames object
 * @return     none
 */
static void rfnames_init (RFnames *names);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Append RFitem object to RFnames.
 *
 * @param[out] names Pointer to RFnames object
 * @param[in]  item RFitem object to add
 * @return     none
 */
static void rfnames_append (RFnames *names,
                            RFitem  *item);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Delete RFitem object from RFnames list at given position.
 *
 * @param[out] names Pointer to RFnames object
 * @param[in]  pos   Position of RFitem to delete
 * @return     none
 */
static void rfnames_delete_at_pos (RFnames             *names,
                                   const uint_fast32_t  pos);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Delete item from file list clicked.
 *
 * @param[in]  widget   The object which received the signal
 * @param[out] names Pointer to RFnames object
 * @return     none
 */
static void event_click_del (GtkWidget *widget,
                             RFnames   *names);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Select items on RFnames list if they match the fun return value.
 *
 * @param[in,out] names Pointer to RFnames object
 * @param[in]     fun      Function to examine RFitem object
 * @param[in]     selected    Boolean value to select or not select item
 * @return     none
 */
static void rfnames_select_unselect (RFnames     *names,
                                     bool     (*fun) (const RFitem *item),
                                     const bool   selected);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Select all check items on list.
 *
 * @param[out] names Pointer to RFnames object
 * @return     none
 */
static void rfnames_select_all (RFnames *names);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Unselect all check items on list.
 *
 * @param[out] names Pointer to RFnames object
 * @return     none
 */
static void rfnames_unselect_all (RFnames *names);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Remove item from RFnames list if it matches the fun return value.
 *
 * @param[in,out] names Pointer to RFnames object
 * @param[in]     fun      Function to examine RFitem object.
 * @return     none
 */
static void rfnames_remove (RFnames *names,
                            bool  (*fun) (const RFitem *item));
/*----------------------------------------------------------------------------*/
/**
 * @brief  RFnames initialization.
 */
static void
rfnames_init (RFnames *names)
{
    *names = (RFnames){};
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create new RFnames object.
 */
RFnames *
rfnames_new (void)
{
    RFnames *names = nullptr;

    if ((names = malloc (sizeof (RFnames))) == nullptr)
        err (EXIT_FAILURE, nullptr);

    rfnames_init (names);

    return names;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free allocated memory.
 */
void
rfnames_free (RFnames *names)
{
    while (names->cnt--) {
        rfitem_free (names->items[names->cnt]);
    }
    free (names->items);
    free (names);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Append RFitem object to RFnames.
 */
static void
rfnames_append (RFnames *names,
                RFitem  *item)
{
    RFitem **tmp = nullptr;
    /* Malloc if null, realloc if not null */
    if (names->items == nullptr) {
        names->items = malloc (sizeof (RFitem*));
    }
    else {
        tmp = realloc (names->items,
                          (names->cnt + 1) * sizeof (RFitem*));
        if (tmp == nullptr) {
            for (uint_fast32_t i = 0; i < names->cnt; ++i) {
                rfitem_free (names->items[i]);
            }
            free (names->items);
            err (EXIT_FAILURE, nullptr);
        }
        else {
            names->items = tmp;
        }
    }
    names->items[names->cnt++] = item;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Delete RFitem object from RFnames list at given position.
 */
static void
rfnames_delete_at_pos (RFnames             *names,
                       const uint_fast32_t  pos)
{
    RFitem    **tmp = nullptr;
    GtkWidget *item_box;
    GtkBox    *parent_box;

    if (pos >= names->cnt || names->cnt == 0)
        return;

    /* Get item box and parent list box */
    item_box = names->items[pos]->box;
    parent_box = GTK_BOX (gtk_widget_get_parent (item_box));

    rfitem_delete (names->items[pos]);

    names->cnt -= 1;

    /* Reorder items on the list and on the box widget */
    for (uint_fast32_t i = pos; i < names->cnt; ++i) {
        names->items[i] = names->items[i+1];
        item_box = names->items[i]->box;
        gtk_box_reorder_child (parent_box, item_box, (gint) i);
    }

    if (names->cnt == 0) {
        free (names->items);
        names->items = nullptr;
        return;
    }

    tmp = realloc (names->items, (names->cnt) * sizeof (RFitem*));
    if (tmp == nullptr) {
        /* The item at index names->cnt is stale: either already freed
         * by rfitem_delete() above, or a duplicate of index cnt-1 left
         * behind by the reorder loop. Only the cnt live entries need
         * freeing here. */
        for (uint_fast32_t i = 0; i < names->cnt; ++i) {
            rfitem_free (names->items[i]);
        }
        free (names->items);
        err (EXIT_FAILURE, nullptr);
    }
    else {
        names->items = tmp;
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Delete item from file list clicked.
 */
static void
event_click_del (GtkWidget *widget,
                 RFnames   *names)
{
    for (uint_fast32_t i = 0; i < names->cnt; ++i) {
        if (names->items[i]->delete_button == widget) {
            rfnames_delete_at_pos (names, i);
            break;
        }
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Append file to RFnames.
 */
GtkWidget *
rfnames_append_gfile (RFnames *names,
                      GFile   *g_file)
{
    RFitem *item;
   
    item = rfitem_new_from_gfile (g_file);
    rfnames_append (names, item);

    g_signal_connect (G_OBJECT (item->delete_button), "clicked",
                      G_CALLBACK (event_click_del), names);

    return item->box;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create new RFitem object using data from gfile item and add it
 *         to RFnames item.
 */
void
rfnames_add_gfile_to_file_box (RFnames *names,
                               GFile   *g_file)
{
    /* Create and append file widgets (entry, check button, buttons) to file
     * list, get box widget with them */
    auto widget = rfnames_append_gfile (names, g_file);
    gtk_widget_show_all (widget);
    /* Add box to the container */
    gtk_box_pack_start (
            GTK_BOX (names->file_box),
            widget,
            FALSE, FALSE, 0);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create new RFitem object using data from file_path string and add it
 *         to RFnames item.
 */
void
rfnames_add_file_path_to_file_box (RFnames    *names,
                                   const char *file_path)
{
    auto g_file = g_file_new_for_path (file_path);
    rfnames_add_gfile_to_file_box (names, g_file);
    g_object_unref (g_file);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Select all check items on list.
 */
static void
rfnames_select_all (RFnames *names)
{
    for (uint_fast32_t i = 0; i < names->cnt; ++i) {
        rfitem_set_checked (names->items[i], true);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Unselect all check items on list.
 */
static void
rfnames_unselect_all (RFnames *names)
{
    for (uint_fast32_t i = 0; i < names->cnt; ++i) {
        rfitem_set_checked (names->items[i], false);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Select or unselect all check items on list.
 */
void
rfnames_select_unselect_all (RFnames *names)
{
    bool          selected = true;
    uint_fast32_t i        = names->cnt;

    while (i-- && (selected = rfitem_get_checked (names->items[i])));

    if (selected) {
        rfnames_unselect_all (names);
    }
    else {
        rfnames_select_all (names);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Select items on RFnames list if they match the fun return value.
 */
static void
rfnames_select_unselect (RFnames     *names,
                         bool     (*fun) (const RFitem *item),
                         const bool   selected)
{
    for (uint_fast32_t i = 0; i < names->cnt; ++i) {
        if (fun (names->items[i])) {
            rfitem_set_checked (names->items[i], selected);
        }
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Select all items with file type on list.
 */
void
rfnames_select_files (RFnames *names)
{
    rfnames_select_unselect (names, rfitem_is_file, true);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Invert selection of items on list.
 */
void
rfnames_select_invert (RFnames *names)
{
    for (uint_fast32_t i = 0; i < names->cnt; ++i) {
        rfitem_invert_checked (names->items[i]);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Unselect all items with file type on list.
 */
void
rfnames_unselect_files (RFnames *names)
{
    rfnames_select_unselect (names, rfitem_is_file, false);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Select all items with folder type on list.
 */
void
rfnames_select_folders (RFnames *names)
{
    rfnames_select_unselect (names, rfitem_is_folder, true);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Unselect all items with folder type on list.
 */
void
rfnames_unselect_folders (RFnames *names)
{
    rfnames_select_unselect (names, rfitem_is_folder, false);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Select all items with symlink type on list.
 */
void
rfnames_select_symlinks (RFnames *names)
{
    rfnames_select_unselect (names, rfitem_is_symlink, true);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Unselect all items with symlink type on list.
 */
void
rfnames_unselect_symlinks (RFnames *names)
{
    rfnames_select_unselect (names, rfitem_is_symlink, false);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Select all items with hidden type on list.
 */
void
rfnames_select_hidden (RFnames *names)
{
    rfnames_select_unselect (names, rfitem_is_hidden, true);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Unselect all items with hidden type on list.
 */
void
rfnames_unselect_hidden (RFnames *names)
{
    rfnames_select_unselect (names, rfitem_is_hidden, false);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Remove all items from list.
 */
void
rfnames_remove_all (RFnames *names)
{
    uint_fast32_t i = names->cnt;
    while (i--) {
        rfnames_delete_at_pos (names, i);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Remove item from RFnames list if it matches the fun return value.
 */
static void
rfnames_remove (RFnames *names,
                bool  (*fun) (const RFitem *item))
{
    uint_fast32_t i = names->cnt;
    while (i--) {
        if (fun (names->items[i])) {
            rfnames_delete_at_pos (names, i);
        }
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Remove selected items from list.
 */
void
rfnames_remove_selected (RFnames *names)
{
    rfnames_remove (names, rfitem_get_checked);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Remove all files from list.
 */
void
rfnames_remove_all_files (RFnames *names)
{
    rfnames_remove (names, rfitem_is_file);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Remove all folders from list.
 */
void
rfnames_remove_all_folders (RFnames *names)
{
    rfnames_remove (names, rfitem_is_folder);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Remove all symlinks from list.
 */
void
rfnames_remove_all_symlinks (RFnames *names)
{
    rfnames_remove (names, rfitem_is_symlink);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Remove all hidden files/folders from list.
 */
void
rfnames_remove_all_hidden (RFnames *names)
{
    rfnames_remove (names, rfitem_is_hidden);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Restore original file name of selected RFitem on list if it matches
 *         the fun return value.
 */
static void
rfnames_restore (RFnames *names,
                 bool  (*fun) (const RFitem *item))
{
    uint_fast32_t i = names->cnt;
    while (i--) {
        if (fun (names->items[i])) {
            rfitem_entry_restore (names->items[i]);
        }
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Restore original file name in all RFitem objects on list.
 */
void
rfnames_restore_all (RFnames *names)
{
    for (uint_fast32_t i = 0; i < names->cnt; ++i) {
        rfitem_entry_restore (names->items[i]);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Restore original file name in selected RFitem objects on list.
 */
void
rfnames_restore_selected (RFnames *names)
{
    rfnames_restore (names, rfitem_get_checked);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Restore original file name for all files from list.
 */
void
rfnames_restore_all_files (RFnames *names)
{
    rfnames_restore (names, rfitem_is_file);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Restore original file name for all folders from list.
 */
void
rfnames_restore_all_folders (RFnames *names)
{
    rfnames_restore (names, rfitem_is_folder);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Restore original file name for all symlinks from list.
 */
void
rfnames_restore_all_symlinks (RFnames *names)
{
    rfnames_restore (names, rfitem_is_symlink);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Restore original file name for all hidden files/folders from list.
 */
void
rfnames_restore_all_hidden (RFnames *names)
{
    rfnames_restore (names, rfitem_is_hidden);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  GCompareFunc adapter for rfitem_compare().
 *
 * g_list_sort() takes a gint (*) (gconstpointer, gconstpointer), which is not
 * a compatible type with rfitem_compare()'s RFitem-typed signature. Calling
 * a function through an incompatible pointer type is undefined behaviour, so
 * the conversion happens here, in a function that genuinely has the signature
 * GLib calls it with.
 *
 * @param[in] a First RFitem object
 * @param[in] b Second RFitem object
 * @return    Compare result
 */
static gint
rfitem_compare_glist (gconstpointer a,
                      gconstpointer b)
{
    return rfitem_compare ((const RFitem *) a, (const RFitem *) b);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Sort RFitem objects in RFnames list by the original path string.
 */
void
rfnames_sort (RFnames *names)
{
    GtkBox *parent_box;
    GList  *sorted       = nullptr;
    GList  *node         = nullptr;
    GList  *dirs         = nullptr;
    GList  *files        = nullptr;
    GList  *hidden_dirs  = nullptr;
    GList  *hidden_files = nullptr;
    int     j            = 0;

    if (names->cnt == 0)
        return;

    for (uint_fast32_t i = 0; i < names->cnt; ++i) {
        if (names->items[i]->file_type == G_FILE_TYPE_DIRECTORY) {
            if (names->items[i]->is_hidden) {
                hidden_dirs = g_list_append (hidden_dirs, names->items[i]);
            }
            else {
                dirs = g_list_append (dirs, names->items[i]);
            }
        }
        else {
            if (names->items[i]->is_hidden) {
                hidden_files = g_list_append (hidden_files, names->items[i]);
            }
            else {
                files = g_list_append (files, names->items[i]);
            }
        }
    }
    dirs    = g_list_sort (dirs,    rfitem_compare_glist);
    hidden_dirs  = g_list_sort (hidden_dirs,  rfitem_compare_glist);
    files   = g_list_sort (files,   rfitem_compare_glist);
    hidden_files = g_list_sort (hidden_files, rfitem_compare_glist);

    sorted = g_list_concat (sorted, hidden_dirs);
    sorted = g_list_concat (sorted, dirs);
    sorted = g_list_concat (sorted, hidden_files);
    sorted = g_list_concat (sorted, files);

    parent_box = GTK_BOX (gtk_widget_get_parent (names->items[0]->box));

    node = sorted;

    while (node != nullptr) {
        RFitem *item = node->data;
        gtk_box_reorder_child (parent_box, item->box, j);
        names->items[j] = item;
        node = node->next;
        ++j;
    }
    g_list_free (sorted);
}
/*----------------------------------------------------------------------------*/

