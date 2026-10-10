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
 * @brief New (after change) file name string
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
 * @var   RFitem::label
 * @brief Label showing file type
 * @var   RFitem::entry
 * @brief Entry for file name
 * @var   RFitem::check_button
 * @brief Checkbutton to set file active for name change
 * @var   RFitem::restore_button
 * @brief Button to restore original file name
 * @var   RFitem::delete_button
 * @brief Button to delete file item from list
 * @var   RFitem::box
 * @brief Box with all file widgets
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
 * @brief         Free allocated memory.
 * @param[in,out] item Pointer to RFitem object
 * @return        none
 *
 * @fn            void rfitem_delete (RFitem *item)
 * @brief         Free allocated memory and delete widgets.
 * @param[in,out] item Pointer to RFitem object
 * @return        none
 */
/*----------------------------------------------------------------------------*/
void rfitem_free   (RFitem *item);

void rfitem_delete (RFitem *item);
/*----------------------------------------------------------------------------*/
/**
 * @fn         bool rfitem_get_checked (RFitem *item)
 * @brief      Get check button checked value.
 * @param[in]  item Pointer to RFitem object
 * @return     Checked value
 *
 * @fn         void rfitem_set_checked (RFitem     *item,
 *                                      const bool  val)
 * @brief      Set check button checked value.
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
    return gtk_toggle_button_get_active (
                GTK_TOGGLE_BUTTON (item->check_button));
}
static inline void rfitem_set_checked     (RFitem         *item,
                                           const bool      val) {
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (item->check_button), val);
}
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
 * @param[in]  item  RFitem object
 * @return     none
 */
/*----------------------------------------------------------------------------*/
void     rfitem_set_new_name_from_org_name (RFitem *item);

void     rfitem_set_org_name_from_new_name (RFitem *item);
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
 * @fn        const char * rfitem_entry_get_text (RFitem *item)
 * @brief     Get text from entry.
 * @param[in] item RFitem with entry
 * @return    String with entry text
 *
 * @fn            void rfitem_entry_restore (RFitem *item)
 * @brief         Restore original file name to entry
 * @param[in,out] item Pointer to RFitem object
 * @return        none
 *
 * @fn            void rfitem_entry_check_and_update (RFitem     *item,
                                                      const char *new_str)
 * @brief         Checks if the new entry text is equal to the previous value
 *                and changes it to a new value if it is not.
 * @param[in,out] item RFitem with entry
 * @param[in]     new_str New entry value
 * @return        none
 */
/*----------------------------------------------------------------------------*/
static inline const char * rfitem_entry_get_text (const RFitem *item) {
    return gtk_entry_get_text (GTK_ENTRY (item->entry));
}
static inline void         rfitem_entry_restore  (RFitem       *item) {
    gtk_entry_set_text (GTK_ENTRY (item->entry), item->org_name);
}
void rfitem_entry_check_and_update (RFitem       *item,
                                    const char   *new_str);
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

