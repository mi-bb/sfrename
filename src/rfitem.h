/**
 * @file  rfitem.h
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
 * @brief  RFitem struncture and functions.
 * 
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#ifndef RFITEM_H
#define RFITEM_H

#include <gtk/gtk.h>
/*----------------------------------------------------------------------------*/
/**
 * @struct RFitem
 *
 * @brief  Single file name item with name strings and widgets
 *
 * @var   RFitem::new_name
 * @brief Pending file name, holds up to FN_U8_LEN bytes. This is the name a
 *        rename uses; the entry only displays and edits it
 * @var   RFitem::org_name
 * @brief Original file name string
 * @var   RFitem::path
 * @brief Path to directory with file to rename
 * @var   RFitem::new_len
 * @brief Total length in bytes of new_name string
 * @var   RFitem::new_u8len
 * @brief Number of unicode chars in new_name string
 * @var   RFitem::org_len
 * @brief Total length in bytes of org_name string
 * @var   RFitem::org_u8len
 * @brief Number of unicode chars in org_name string
 * @var   RFitem::file_type
 * @brief File type in GFileType value
 * @var   RFitem::is_symlink
 * @brief Is file / dir a symbolic link
 * @var   RFitem::is_hidden
 * @brief Is file / dir hidden
 * @var   RFitem::checked
 * @brief Is item active for name change; the check button mirrors this value
 * @var   RFitem::object
 * @brief Object representing this item in the list model, owned by the item
 * @var   RFitem::label
 * @brief Label showing file type, null while the item has no visible row
 * @var   RFitem::entry
 * @brief Entry for file name, null while the item has no visible row
 * @var   RFitem::check_button
 * @brief Checkbutton to set file active for name change, null while the item
 *        has no visible row
 * @var   RFitem::restore_button
 * @brief Button to restore original file name, null while the item has no
 *        visible row
 * @var   RFitem::delete_button
 * @brief Button to delete file item from list, null while the item has no
 *        visible row
 * @var   RFitem::box
 * @brief Row with all file widgets, null while the item has no visible row
 */
typedef struct
RFitem {
    char      *new_name;
    char      *org_name;
    char      *path;
    size_t     new_len;
    size_t     new_u8len;
    size_t     org_len;
    size_t     org_u8len;
    GFileType  file_type;
    bool       is_symlink;
    bool       is_hidden;
    bool       checked;
    GObject   *object;
    GtkWidget *label;
    GtkWidget *entry;
    GtkWidget *check_button;
    GtkWidget *restore_button;
    GtkWidget *delete_button;
    GtkWidget *box;
} RFitem;
/*----------------------------------------------------------------------------*/
/**
 * @fn        RFitem *rfitem_new_from_gfile (GFile *g_file)
 * @brief     Create and return new RFnames object from GFile.
 * @param[in] g_file GFile item
 * @return    New RFitem object
 *
 * @fn        RFitem *rfitem_new_from_file_path (const char *path)
 * @brief     Create and return new RFnames object from file path.
 * @param[in] path File path
 * @return    New RFitem object
 */
/*----------------------------------------------------------------------------*/
[[nodiscard]] RFitem * rfitem_new_from_gfile     (GFile      *g_file)
              __attribute__ ((returns_nonnull));

[[nodiscard]] RFitem * rfitem_new_from_file_path (const char *path)
              __attribute__ ((nonnull (1), returns_nonnull));
/*----------------------------------------------------------------------------*/
/**
 * @fn            void rfitem_free (RFitem *item)
 * @brief         Free allocated memory. The item must not be bound to a row.
 * @param[in,out] item Pointer to RFitem object
 * @return        none
 */
/*----------------------------------------------------------------------------*/
void rfitem_free (RFitem *item);
/*----------------------------------------------------------------------------*/
/**
 * @fn         GtkWidget * rfitem_row_new (void)
 * @brief      Create widgets of a list row, not showing any item yet.
 * @return     Row widget
 *
 * @fn            void rfitem_row_bind (RFitem *item, GtkWidget *row)
 * @brief         Show item in a row and connect the row's widgets to it.
 * @param[in,out] item Pointer to RFitem object
 * @param[in]     row  Row made by rfitem_row_new()
 * @return        none
 *
 * @fn            void rfitem_row_unbind (RFitem *item, GtkWidget *row)
 * @brief         Disconnect a row's widgets from item.
 *
 * The widget pointers of the item are cleared only if it is still bound to
 * this row: a list can bind an item to a new row before releasing the old one.
 *
 * @param[in,out] item Pointer to RFitem object
 * @param[in]     row  Row to disconnect
 * @return        none
 */
/*----------------------------------------------------------------------------*/
[[nodiscard]] GtkWidget * rfitem_row_new (void)
              __attribute__ ((returns_nonnull));

void rfitem_row_bind   (RFitem    *item,
                        GtkWidget *row)
                       __attribute__ ((nonnull (1, 2)));

void rfitem_row_unbind (RFitem    *item,
                        GtkWidget *row)
                       __attribute__ ((nonnull (1, 2)));
/*----------------------------------------------------------------------------*/
/**
 * @fn         bool rfitem_get_checked (RFitem *item)
 * @brief      Get item checked value.
 * @param[in]  item Pointer to RFitem object
 * @return     Checked value
 *
 * @fn         void rfitem_set_checked (RFitem     *item,
 *                                      const bool  val)
 * @brief      Set item checked value and update check button, if there is one.
 * @param[out] item  Pointer to RFitem object
 * @param[in]  val   Check value
 * @return     none
 *
 * @fn            void rfitem_invert_checked (RFitem *item)
 * @brief         Invert check item check value
 * @param[in,out] item Pointer to RFitem object
 * @return        none
 */
/*----------------------------------------------------------------------------*/
static inline bool rfitem_get_checked     (const RFitem   *item) {
    return item->checked;
}
void     rfitem_set_checked    (RFitem         *item,
                                const bool      val);

void     rfitem_invert_checked (RFitem         *item);
/*----------------------------------------------------------------------------*/
/**
 * @fn  void rfitem_set_new_name_from_org_name (RFitem *item)
 *
 * @brief  Copy org_name string and length values to new_name.
 *
 * @param[in]  item  RFitem object
 * @return     none
 *
 * @fn  void rfitem_set_org_name_from_new_name (RFitem *item)
 *
 * @brief  Copy new_name string and length values to org_name.
 *
 * The value is truncated to FN_LEN bytes.
 *
 * @param[in]  item  RFitem object
 * @return     none
 *
 * @fn  void rfitem_set_new_name (RFitem *item, const char *val)
 *
 * @brief  Set new_name string and length values.
 *
 * The value is truncated to FN_U8_LEN bytes, on a UTF-8 character boundary.
 *
 * @param[out] item  RFitem object
 * @param[in]  val   New new_name value
 * @return     none
 *
 * @fn  void rfitem_set_org_name (RFitem *item, const char *val)
 *
 * @brief  Set org_name string and length values.
 *
 * Used after a successful rename to store the name the file now has on
 * disk. The value is truncated to FN_LEN bytes.
 *
 * @param[out] item  RFitem object
 * @param[in]  val   New org_name value
 * @return     none
 */
/*----------------------------------------------------------------------------*/
void     rfitem_set_new_name_from_org_name (RFitem *item);

void     rfitem_set_org_name_from_new_name (RFitem *item);

void     rfitem_set_new_name               (RFitem     *item,
                                            const char *val)
                                           __attribute__ ((nonnull (1, 2)));

void     rfitem_set_org_name             (RFitem     *item,
                                            const char *val)
                                           __attribute__ ((nonnull (1, 2)));
/*----------------------------------------------------------------------------*/
/**
 * @fn         const char * rfitem_get_new_name (const RFitem *item)
 * @brief      Get new_name value.
 * @param[in]  item  RFitem object
 * @return     new_name value
 *
 * @fn         const char * rfitem_get_org_name (const RFitem *item)
 * @brief      Get org_name value.
 * @param[in]  item  RFitem object
 * @return     org_name value
 *
 * @fn         const char * rfitem_get_path (const RFitem *item)
 * @brief      Get path value.
 * @param[in]  item  RFitem object
 * @return     path value
 */
/*----------------------------------------------------------------------------*/
static inline const char * rfitem_get_new_name (const RFitem *item) {
    return (const char *) item->new_name;
}
static inline const char * rfitem_get_org_name (const RFitem *item) {
    return (const char *) item->org_name;
}
static inline const char * rfitem_get_path (const RFitem *item) {
    return (const char *) item->path;
}
/*----------------------------------------------------------------------------*/
/**
 * @fn            void rfitem_entry_restore (RFitem *item)
 * @brief         Set new_name back to the original name and show it in entry.
 * @param[in,out] item Pointer to RFitem object
 * @return        none
 *
 * @fn            void rfitem_entry_sync (RFitem *item)
 * @brief         Show new_name in the entry, if there is one and its text
 *                differs.
 * @param[in,out] item RFitem with entry
 * @return        none
 */
/*----------------------------------------------------------------------------*/
void rfitem_entry_restore (RFitem *item);

void rfitem_entry_sync    (RFitem *item);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Compare 2 RFitem objects.
 *
 * @param[in] item1 First RFitem object
 * @param[in] item2 Second RFitem object
 * @return    Compare value
 */
int rfitem_compare (const RFitem *item1,
                    const RFitem *item2);
/*----------------------------------------------------------------------------*/
/**
 * @fn  bool rfitem_is_file (const RFitem *item)
 *
 * @brief  Check if RFitem type is a file.
 *
 * @param[in] item Pointer to RFitem object
 * @return    true if RFitem describes a file type, false if not
 *
 * @fn  bool rfitem_is_folder (const RFitem *item)
 *
 * @brief  Check if RFitem type is a directory.
 *
 * @param[in] item Pointer to RFitem object
 * @return    true if RFitem describes a directory type, false if not
 *
 * @fn  bool rfitem_is_symlink (const RFitem *item)
 *
 * @brief  Check if RFitem file/dir type is a symlink.
 *
 * @param[in] item Pointer to RFitem object
 * @return    true if RFitem is a symlink, false if not
 *
 * @fn  bool rfitem_is_hidden  (const RFitem *item)
 *
 * @brief  Check if RFitem file/dir is hidden.
 *
 * @param[in] item Pointer to RFitem object
 * @return    true if RFitem is a hidden file/dir, false if not
 */
/*----------------------------------------------------------------------------*/
static inline bool rfitem_is_file        (const RFitem *item) {
    return (item->file_type == G_FILE_TYPE_REGULAR);
}
static inline bool rfitem_is_folder      (const RFitem *item) {
    return (item->file_type == G_FILE_TYPE_DIRECTORY);
}
static inline bool rfitem_is_symlink     (const RFitem *item) {
    return item->is_symlink;
}
static inline bool rfitem_is_hidden      (const RFitem *item) {
    return item->is_hidden;
}
/*----------------------------------------------------------------------------*/
#endif

