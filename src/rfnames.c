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
 * @brief  RFnames structure and functions.
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <stdint.h>
#include <err.h>
#include "defs.h"
#include "rfnames.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Object holding one RFitem in the list model.
 *
 * The pointer is cleared before the item is freed, because the view can
 * release its row after that.
 */
#define RF_TYPE_ROW_ITEM (rf_row_item_get_type ())
G_DECLARE_FINAL_TYPE (RFRowItem, rf_row_item, RF, ROW_ITEM, GObject)

struct _RFRowItem {
    GObject  parent_instance;
    RFitem  *item;
};

G_DEFINE_FINAL_TYPE (RFRowItem, rf_row_item, G_TYPE_OBJECT)

static void
rf_row_item_class_init ([[maybe_unused]] RFRowItemClass *klass)
{
}

static void
rf_row_item_init ([[maybe_unused]] RFRowItem *self)
{
}
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
 * @param[in]  item  RFitem object to add
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
 * @param[in]  widget The object which received the signal
 * @param[out] names  Pointer to RFnames object
 * @return     none
 */
static void event_click_del (GtkWidget *widget,
                             RFnames   *names);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Select items on RFnames list if they match the fun return value.
 *
 * @param[in,out] names     Pointer to RFnames object
 * @param[in]     fun       Function to examine RFitem object
 * @param[in]     selected  Boolean value to select or not select item
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
 * @param[in]     fun   Function to examine RFitem object.
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
    names->store = g_list_store_new (RF_TYPE_ROW_ITEM);

    return names;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Disconnect item from its row and from the list model object.
 *
 * @param[in,out] item RFitem object about to be freed
 * @return        none
 */
static void
rfnames_detach_item (RFitem *item)
{
    if (item->box != nullptr)
        rfitem_row_unbind (item, item->box);

    RF_ROW_ITEM (item->object)->item = nullptr;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free allocated memory.
 */
void
rfnames_free (RFnames *names)
{
    while (names->cnt--) {
        rfnames_detach_item (names->items[names->cnt]);
        rfitem_free (names->items[names->cnt]);
    }
    free (names->items);
    g_clear_object (&names->store);
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

    RFRowItem *row_item = g_object_new (RF_TYPE_ROW_ITEM, nullptr);
    row_item->item = item;
    item->object = G_OBJECT (row_item);
    g_list_store_append (names->store, row_item);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Delete RFitem object from RFnames list at given position.
 */
static void
rfnames_delete_at_pos (RFnames             *names,
                       const uint_fast32_t  pos)
{
    RFitem **tmp = nullptr;

    if (pos >= names->cnt || names->cnt == 0)
        return;

    rfnames_detach_item (names->items[pos]);
    g_list_store_remove (names->store, (guint) pos);
    rfitem_free (names->items[pos]);

    names->cnt -= 1;

    /* Close the gap on the list, the model already did */
    for (uint_fast32_t i = pos; i < names->cnt; ++i) {
        names->items[i] = names->items[i+1];
    }

    if (names->cnt == 0) {
        free (names->items);
        names->items = nullptr;
        return;
    }

    tmp = realloc (names->items, (names->cnt) * sizeof (RFitem*));
    if (tmp == nullptr) {
        /* The item at index names->cnt is stale: either already freed
         * by rfitem_free() above, or a duplicate of index cnt-1 left
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
 * @brief  Give focus to an entry from the main loop.
 *
 * Rows are bound during layout, when focus can not be changed.
 *
 * @param[in] data Entry widget
 * @return    G_SOURCE_REMOVE
 */
static gboolean
focus_entry_idle (gpointer data)
{
    gtk_widget_grab_focus (GTK_WIDGET (data));
    return G_SOURCE_REMOVE;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  New row widget needed by the list view.
 *
 * @param[in] factory Unused list item factory
 * @param[in] object  GtkListItem to fill
 * @param[in] names   RFnames object passed to the row's delete button
 * @return    none
 */
static void
event_row_setup ([[maybe_unused]] GtkSignalListItemFactory *factory,
                 GObject *object,
                 RFnames *names)
{
    GtkListItem *list_item = GTK_LIST_ITEM (object);
    GtkWidget   *row       = rfitem_row_new ();

    /* Rows are not selected or activated, only the widgets in them are used */
    gtk_list_item_set_selectable (list_item, FALSE);
    gtk_list_item_set_activatable (list_item, FALSE);
    gtk_list_item_set_focusable (list_item, FALSE);
    gtk_list_item_set_child (list_item, row);

    g_signal_connect (gtk_widget_get_last_child (row), "clicked",
                      G_CALLBACK (event_click_del), names);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Row starts showing an item.
 *
 * @param[in]     factory Unused list item factory
 * @param[in]     object  GtkListItem with the row
 * @param[in,out] names   RFnames object
 * @return        none
 */
static void
event_row_bind ([[maybe_unused]] GtkSignalListItemFactory *factory,
                GObject *object,
                RFnames *names)
{
    GtkListItem *list_item = GTK_LIST_ITEM (object);
    RFRowItem   *row_item  = gtk_list_item_get_item (list_item);

    if (row_item->item == nullptr)
        return;

    rfitem_row_bind (row_item->item, gtk_list_item_get_child (list_item));

    if (names->focus_first && names->cnt > 0 &&
        names->items[0] == row_item->item) {
        names->focus_first = false;
        g_idle_add_full (G_PRIORITY_DEFAULT_IDLE, focus_entry_idle,
                         g_object_ref (row_item->item->entry), g_object_unref);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Row stops showing an item.
 *
 * @param[in] factory Unused list item factory
 * @param[in] object  GtkListItem with the row
 * @param[in] names   Unused RFnames object
 * @return    none
 */
static void
event_row_unbind ([[maybe_unused]] GtkSignalListItemFactory *factory,
                  GObject *object,
                  [[maybe_unused]] RFnames *names)
{
    GtkListItem *list_item = GTK_LIST_ITEM (object);
    RFRowItem   *row_item  = gtk_list_item_get_item (list_item);

    if (row_item != nullptr && row_item->item != nullptr) {
        rfitem_row_unbind (row_item->item,
                           gtk_list_item_get_child (list_item));
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create the list view showing the items.
 */
GtkWidget *
rfnames_create_view (RFnames *names)
{
    GtkListItemFactory *factory = gtk_signal_list_item_factory_new ();
    GtkCssProvider     *css     = gtk_css_provider_new ();
    GtkNoSelection     *model;

    g_signal_connect (factory, "setup", G_CALLBACK (event_row_setup), names);
    g_signal_connect (factory, "bind", G_CALLBACK (event_row_bind), names);
    g_signal_connect (factory, "unbind", G_CALLBACK (event_row_unbind), names);

    model = gtk_no_selection_new (G_LIST_MODEL (g_object_ref (names->store)));
    /* The view takes ownership of the model and factory */
    names->view = gtk_list_view_new (GTK_SELECTION_MODEL (model), factory);
    /* Rows are plain widgets on the window background, not a data view */
    gtk_widget_add_css_class (names->view, "file-list");
    gtk_css_provider_load_from_string (css,
            "listview.file-list, listview.file-list > row "
            "{ background: none; }");
    gtk_style_context_add_provider_for_display (
            gtk_widget_get_display (names->view),
            GTK_STYLE_PROVIDER (css),
            GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref (css);

    return names->view;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Give focus to the entry of the first item.
 */
void
rfnames_focus_first (RFnames *names)
{
    if (names->cnt == 0)
        return;

    if (names->items[0]->entry != nullptr) {
        gtk_widget_grab_focus (names->items[0]->entry);
    }
    else {
        names->focus_first = true;
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create new RFitem object using data from gfile item and add it
 *         to RFnames item.
 */
void
rfnames_add_gfile (RFnames *names,
                   GFile   *g_file)
{
    /* GTK4 file dialogs can return remote files, RFitem needs a local path */
    if (g_file_peek_path (g_file) == nullptr) {
        g_autofree char *name = g_file_get_parse_name (g_file);
        g_printerr ("Skipping %s: not a local file\n", name);
        return;
    }
    rfnames_append (names, rfitem_new_from_gfile (g_file));
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
    GObject **objects;
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
    dirs         = g_list_sort (dirs,         rfitem_compare_glist);
    hidden_dirs  = g_list_sort (hidden_dirs,  rfitem_compare_glist);
    files        = g_list_sort (files,        rfitem_compare_glist);
    hidden_files = g_list_sort (hidden_files, rfitem_compare_glist);

    sorted = g_list_concat (sorted, hidden_dirs);
    sorted = g_list_concat (sorted, dirs);
    sorted = g_list_concat (sorted, hidden_files);
    sorted = g_list_concat (sorted, files);

    node = sorted;
    objects = g_new (GObject *, names->cnt);

    while (node != nullptr) {
        RFitem *item = node->data;
        names->items[j] = item;
        objects[j] = item->object;
        node = node->next;
        ++j;
    }
    g_list_free (sorted);

    /* Put the same objects back in the new order */
    g_list_store_splice (names->store, 0, (guint) names->cnt,
                         (gpointer *) objects, (guint) names->cnt);
    g_free (objects);
}
/*----------------------------------------------------------------------------*/

