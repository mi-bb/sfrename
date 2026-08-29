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
 * @param[out] rf_names Pointer to RFnames object
 * @return     none
 */
static void rfnames_init (RFnames *rf_names);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Append RFitem object to RFnames.
 *
 * @param[out] rf_names Pointer to RFnames object
 * @param[in]  rf_item RFitem object to add
 * @return     none
 */
static void rfnames_append (RFnames *rf_names,
                            RFitem  *rf_item);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Delete RFitem object from RFnames list at given position.
 *
 * @param[out] rf_names Pointer to RFnames object
 * @param[in]  ui_pos   Position of RFitem to delete
 * @return     none
 */
static void rfnames_delete_at_pos (RFnames             *rf_names,
                                   const uint_fast32_t  ui_pos);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Delete item from file list clicked.
 *
 * @param[in]  widget   The object which received the signal
 * @param[out] rf_names Pointer to RFnames object
 * @return     none
 */
static void event_click_del (GtkWidget *widget,
                             RFnames   *rf_names);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Select items on RFnames list if they match the fun return value.
 *
 * @param[in,out] rf_names Pointer to RFnames object
 * @param[in]     fun      Function to examine RFitem object
 * @param[in]     b_sel    Boolean value to select or not select item
 * @return     none
 */
static void rfnames_select_unselect (RFnames         *rf_names,
                                     int         (*fun) (const RFitem *rf_item),
                                     const gboolean   b_sel);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Select all check items on list.
 *
 * @param[out] rf_names Pointer to RFnames object
 * @return     none
 */
static void rfnames_select_all (RFnames *rf_names);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Unselect all check items on list.
 *
 * @param[out] rf_names Pointer to RFnames object
 * @return     none
 */
static void rfnames_unselect_all (RFnames *rf_names);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Remove item from RFnames list if it matches the fun return value.
 *
 * @param[in,out] rf_names Pointer to RFnames object
 * @param[in]     fun      Function to examine RFitem object.
 * @return     none
 */
static void rfnames_remove (RFnames   *rf_names,
                            int      (*fun) (const RFitem *rf_item));
/*----------------------------------------------------------------------------*/
/**
 * @brief  RFnames initialization.
 */
static void
rfnames_init (RFnames *rf_names)
{
    rf_names->cnt      = 0;    /* names count */
    rf_names->rf_items = NULL; /* item list */
    rf_names->file_box = NULL; /* box with widgets */
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create new RFnames object.
 */
RFnames *
rfnames_new (void)
{
    RFnames *rf_names = NULL;

    if ((rf_names = malloc (sizeof (RFnames))) == NULL)
        err (EXIT_FAILURE, NULL);

    rfnames_init (rf_names);

    return rf_names;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free allocated memory.
 */
void
rfnames_free (RFnames *rf_names)
{
    while (rf_names->cnt--) {
        rfitem_free (rf_names->rf_items[rf_names->cnt]);
    }
    free (rf_names);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Append RFitem object to RFnames.
 */
static void
rfnames_append (RFnames *rf_names,
                RFitem  *rf_item)
{
    RFitem **rf_tmp = NULL;
    /* Malloc if null, realloc if not null */
    if (rf_names->rf_items == NULL) {
        rf_names->rf_items = malloc (sizeof (RFitem*));
    }
    else {
        rf_tmp = realloc (rf_names->rf_items,
                          (rf_names->cnt + 1) * sizeof (RFitem*));
        if (rf_tmp == NULL) {
            for (uint_fast32_t i = 0; i < rf_names->cnt; ++i) {
                rfitem_free (rf_names->rf_items[i]);
            }
            free (rf_names->rf_items);
            err (EXIT_FAILURE, NULL);
        }
        else {
            rf_names->rf_items = rf_tmp;
        }
    }
    rf_names->rf_items[rf_names->cnt++] = rf_item;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Delete RFitem object from RFnames list at given position.
 */
static void
rfnames_delete_at_pos (RFnames             *rf_names,
                       const uint_fast32_t  ui_pos)
{
    RFitem    **rf_tmp = NULL;
    GtkWidget  *gw_ibox;
    GtkBox     *gb_pbox;

    if (ui_pos >= rf_names->cnt || rf_names->cnt == 0)
        return;

    /* Get item box and parent list box */
    gw_ibox = rf_names->rf_items[ui_pos]->box;
    gb_pbox = GTK_BOX (gtk_widget_get_parent (gw_ibox));

    rfitem_delete (rf_names->rf_items[ui_pos]);

    rf_names->cnt -= 1;

    /* Reorder items on the list and on the box widget */
    for (uint_fast32_t i = ui_pos; i < rf_names->cnt; ++i) {
        rf_names->rf_items[i] = rf_names->rf_items[i+1];
        gw_ibox = rf_names->rf_items[i]->box;
        gtk_box_reorder_child (gb_pbox, gw_ibox, (gint) i);
    }

    if (rf_names->cnt == 0) {
        free (rf_names->rf_items);
        rf_names->rf_items = NULL;
        return;
    }

    rf_tmp = realloc (rf_names->rf_items, (rf_names->cnt) * sizeof (RFitem*));
    if (rf_tmp == NULL) {
        /* The item at index rf_names->cnt is stale: either already freed
         * by rfitem_delete() above, or a duplicate of index cnt-1 left
         * behind by the reorder loop. Only the cnt live entries need
         * freeing here. */
        for (uint_fast32_t i = 0; i < rf_names->cnt; ++i) {
            rfitem_free (rf_names->rf_items[i]);
        }
        free (rf_names->rf_items);
        err (EXIT_FAILURE, NULL);
    }
    else {
        rf_names->rf_items = rf_tmp;
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Delete item from file list clicked.
 */
static void
event_click_del (GtkWidget *widget,
                 RFnames   *rf_names)
{
    for (uint_fast32_t i = 0; i < rf_names->cnt; ++i) {
        if (rf_names->rf_items[i]->dbut == widget) {
            rfnames_delete_at_pos (rf_names, i);
            break;
        }
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Append file to RFnames.
 */
GtkWidget *
rfnames_append_gfile (RFnames *rf_names,
                      GFile   *g_file)
{
    RFitem *rf_item;
   
    rf_item = rfitem_new_from_gfile (g_file);
    rfnames_append (rf_names, rf_item);

    g_signal_connect (G_OBJECT (rf_item->dbut), "clicked",
                      G_CALLBACK (event_click_del), rf_names);

    return rf_item->box;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create new RFitem object using data from gfile item and add it
 *         to RFnames item.
 */
void
rfnames_add_gfile_to_file_box (RFnames *rf_names,
                               GFile   *g_file)
{
    /* Create and append file widgets (entry, check, buttons) to file
     * list, get box widget with them */
    GtkWidget *gw_widget = rfnames_append_gfile (rf_names, g_file);
    gtk_widget_show_all (gw_widget);
    /* Add box to the container */
    gtk_box_pack_start (
            GTK_BOX (rf_names->file_box),
            gw_widget,
            FALSE, FALSE, 0);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create new RFitem object using data from s_fn string and add it
 *         to RFnames item.
 */
void
rfnames_add_sfile_to_file_box (RFnames    *rf_names,
                               const char *s_fn)
{
    GFile *g_file = g_file_new_for_path (s_fn);
    rfnames_add_gfile_to_file_box (rf_names, g_file);
    g_object_unref (g_file);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Select all check items on list.
 */
static void
rfnames_select_all (RFnames *rf_names)
{
    for (uint_fast32_t i = 0; i < rf_names->cnt; ++i) {
        rfitem_set_checked (rf_names->rf_items[i], TRUE);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Unselect all check items on list.
 */
static void
rfnames_unselect_all (RFnames *rf_names)
{
    for (uint_fast32_t i = 0; i < rf_names->cnt; ++i) {
        rfitem_set_checked (rf_names->rf_items[i], FALSE);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Select or unselect all check items on list.
 */
void
rfnames_select_unselect_all (RFnames *rf_names)
{
    gboolean      b_sel = TRUE;
    uint_fast32_t i     = rf_names->cnt;

    while (i-- && (b_sel = rfitem_get_checked (rf_names->rf_items[i])));

    if (b_sel) {
        rfnames_unselect_all (rf_names);
    }
    else {
        rfnames_select_all (rf_names);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Select items on RFnames list if they match the fun return value.
 */
static void
rfnames_select_unselect (RFnames         *rf_names,
                         gboolean       (*fun) (const RFitem *rf_item),
                         const gboolean   b_sel)
{
    for (uint_fast32_t i = 0; i < rf_names->cnt; ++i) {
        if (fun (rf_names->rf_items[i])) {
            rfitem_set_checked (rf_names->rf_items[i], b_sel);
        }
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Select all items with file type on list.
 */
void
rfnames_select_files (RFnames *rf_names)
{
    rfnames_select_unselect (rf_names, rfitem_is_file, TRUE);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Invert selection of items on list.
 */
void
rfnames_select_invert (RFnames *rf_names)
{
    for (uint_fast32_t i = 0; i < rf_names->cnt; ++i) {
        rfitem_invert_checked (rf_names->rf_items[i]);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Unselect all items with file type on list.
 */
void
rfnames_unselect_files (RFnames *rf_names)
{
    rfnames_select_unselect (rf_names, rfitem_is_file, FALSE);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Select all items with folder type on list.
 */
void
rfnames_select_folders (RFnames *rf_names)
{
    rfnames_select_unselect (rf_names, rfitem_is_folder, TRUE);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Unselect all items with folder type on list.
 */
void
rfnames_unselect_folders (RFnames *rf_names)
{
    rfnames_select_unselect (rf_names, rfitem_is_folder, FALSE);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Select all items with symlink type on list.
 */
void
rfnames_select_symlinks (RFnames *rf_names)
{
    rfnames_select_unselect (rf_names, rfitem_is_symlink, TRUE);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Unselect all items with symlink type on list.
 */
void
rfnames_unselect_symlinks (RFnames *rf_names)
{
    rfnames_select_unselect (rf_names, rfitem_is_symlink, FALSE);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Select all items with hidden type on list.
 */
void
rfnames_select_hidden (RFnames *rf_names)
{
    rfnames_select_unselect (rf_names, rfitem_is_hidden, TRUE);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Unselect all items with hidden type on list.
 */
void
rfnames_unselect_hidden (RFnames *rf_names)
{
    rfnames_select_unselect (rf_names, rfitem_is_hidden, FALSE);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Remove all items from list.
 */
void
rfnames_remove_all (RFnames *rf_names)
{
    uint_fast32_t i = rf_names->cnt;
    while (i--) {
        rfnames_delete_at_pos (rf_names, i);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Remove item from RFnames list if it matches the fun return value.
 */
static void
rfnames_remove (RFnames   *rf_names,
                gboolean (*fun) (const RFitem *rf_item))
{
    uint_fast32_t i = rf_names->cnt;
    while (i--) {
        if (fun (rf_names->rf_items[i])) {
            rfnames_delete_at_pos (rf_names, i);
        }
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Remove selected items from list.
 */
void
rfnames_remove_selected (RFnames *rf_names)
{
    rfnames_remove (rf_names, rfitem_get_checked);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Remove all files from list.
 */
void
rfnames_remove_all_files (RFnames *rf_names)
{
    rfnames_remove (rf_names, rfitem_is_file);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Remove all folders from list.
 */
void
rfnames_remove_all_folders (RFnames *rf_names)
{
    rfnames_remove (rf_names, rfitem_is_folder);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Remove all symlinks from list.
 */
void
rfnames_remove_all_symlinks (RFnames *rf_names)
{
    rfnames_remove (rf_names, rfitem_is_symlink);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Remove all hidden files/folders from list.
 */
void
rfnames_remove_all_hidden (RFnames *rf_names)
{
    rfnames_remove (rf_names, rfitem_is_hidden);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Restore original file name of selected RFitem on list if it matches
 *         the fun return value.
 */
static void
rfnames_restore (RFnames   *rf_names,
                 gboolean (*fun) (const RFitem *rf_item))
{
    uint_fast32_t i = rf_names->cnt;
    while (i--) {
        if (fun (rf_names->rf_items[i])) {
            rfitem_entry_restore (rf_names->rf_items[i]);
        }
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Restore original file name in all RFitem objects on list.
 */
void
rfnames_restore_all (RFnames *rf_names)
{
    for (uint_fast32_t i = 0; i < rf_names->cnt; ++i) {
        rfitem_entry_restore (rf_names->rf_items[i]);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Restore original file name in selected RFitem objects on list.
 */
void
rfnames_restore_selected (RFnames *rf_names)
{
    rfnames_restore (rf_names, rfitem_get_checked);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Restore original file name for all files from list.
 */
void
rfnames_restore_all_files (RFnames *rf_names)
{
    rfnames_restore (rf_names, rfitem_is_file);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Restore original file name for all folders from list.
 */
void
rfnames_restore_all_folders (RFnames *rf_names)
{
    rfnames_restore (rf_names, rfitem_is_folder);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Restore original file name for all symlinks from list.
 */
void
rfnames_restore_all_symlinks (RFnames *rf_names)
{
    rfnames_restore (rf_names, rfitem_is_symlink);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Restore original file name for all hidden files/folders from list.
 */
void
rfnames_restore_all_hidden (RFnames *rf_names)
{
    rfnames_restore (rf_names, rfitem_is_hidden);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Sort RFitem objects in RFnames list by the original path string.
 */
void
rfnames_sort (RFnames *rf_names)
{
    GtkBox *gb_pbox;
    GList  *gl_items   = NULL;
    GList  *gl_items1  = NULL;
    GList  *gl_dirs    = NULL;
    GList  *gl_files   = NULL;
    GList  *gl_dirs_h  = NULL;
    GList  *gl_files_h = NULL;
    int              j = 0;

    if (rf_names->cnt == 0)
        return;

    for (uint_fast32_t i = 0; i < rf_names->cnt; ++i) {
        if (rf_names->rf_items[i]->f_type == G_FILE_TYPE_DIRECTORY) {
            if (rf_names->rf_items[i]->b_hidden) {
                gl_dirs_h = g_list_append (gl_dirs_h, rf_names->rf_items[i]);
            }
            else {
                gl_dirs = g_list_append (gl_dirs, rf_names->rf_items[i]);
            }
        }
        else {
            if (rf_names->rf_items[i]->b_hidden) {
                gl_files_h = g_list_append (gl_files_h, rf_names->rf_items[i]);
            }
            else {
                gl_files = g_list_append (gl_files, rf_names->rf_items[i]);
            }
        }
    }
    gl_dirs    = g_list_sort (gl_dirs,    (GCompareFunc) rfitem_compare);
    gl_dirs_h  = g_list_sort (gl_dirs_h,  (GCompareFunc) rfitem_compare);
    gl_files   = g_list_sort (gl_files,   (GCompareFunc) rfitem_compare);
    gl_files_h = g_list_sort (gl_files_h, (GCompareFunc) rfitem_compare);

    gl_items = g_list_concat (gl_items, gl_dirs_h);
    gl_items = g_list_concat (gl_items, gl_dirs);
    gl_items = g_list_concat (gl_items, gl_files_h);
    gl_items = g_list_concat (gl_items, gl_files);

    gb_pbox = GTK_BOX (gtk_widget_get_parent (rf_names->rf_items[0]->box));

    gl_items1 = gl_items;

    while (gl_items1 != NULL) {
        RFitem *rf_item = gl_items1->data;
        gtk_box_reorder_child (gb_pbox, rf_item->box, j);
        rf_names->rf_items[j] = rf_item;
        gl_items1 = gl_items1->next;
        ++j;
    }
    g_list_free (gl_items);
    g_list_free (gl_items1);
}
/*----------------------------------------------------------------------------*/

