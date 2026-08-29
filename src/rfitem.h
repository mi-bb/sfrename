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
 * @var   RFitem::s_new
 * @brief New (after change) file name string
 * @var   RFitem::s_org
 * @brief Original file name string
 * @var   RFitem::s_pth
 * @brief Path to directory with file to rename
 * @var   RFitem::new_len
 * @brief Total length in bytes of s_new string
 * @var   RFitem::new_u8len
 * @brief Number of unicode chars in s_new string
 * @var   RFitem::org_len
 * @brief Total length in bytes of s_org string
 * @var   RFitem::org_u8len
 * @brief Number of unicode chars in s_org string
 * @var   RFitem::f_type
 * @brief File type in GFileType value
 * @var   RFitem::b_slink
 * @brief Is file / dir a symbolic link
 * @var   RFitem::b_hidden
 * @brief Is file / dir hidden
 * @var   RFitem::label
 * @brief Label showing file type
 * @var   RFitem::entry
 * @brief Entry for file name
 * @var   RFitem::check
 * @brief Checkbutton to set file active for name change
 * @var   RFitem::rbut
 * @brief Button to restore original file name
 * @var   RFitem::dbut
 * @brief Button to delete file item from list
 * @var   RFitem::box
 * @brief Box with all file widgets
 */
typedef struct
RFitem {
    char      *s_new;
    char      *s_org;
    char      *s_pth;
    size_t     new_len;
    size_t     new_u8len;
    size_t     org_len;
    size_t     org_u8len;
    GFileType  f_type;
    bool       b_slink;
    bool       b_hidden;
    GtkWidget *label;
    GtkWidget *entry;
    GtkWidget *check;
    GtkWidget *rbut;
    GtkWidget *dbut;
    GtkWidget *box;
} RFitem;
/*----------------------------------------------------------------------------*/
/**
 * @fn        RFitem *rfitem_new_from_gfile (GFile *g_file)
 * @brief     Create and return new RFnames object from GFile.
 * @param[in] g_file GFile item
 * @return    New RFitem object
 *
 * @fn        RFitem *rfitem_new_from_sfile (const char *s_pth)
 * @brief     Create and return new RFnames object from file path.
 * @param[in] s_pth File path
 * @return    New RFitem object
 */
/*----------------------------------------------------------------------------*/
[[nodiscard]] RFitem * rfitem_new_from_gfile (GFile      *g_file)
              __attribute__ ((returns_nonnull));

[[nodiscard]] RFitem * rfitem_new_from_sfile (const char *s_pth)
              __attribute__ ((nonnull (1), returns_nonnull));
/*----------------------------------------------------------------------------*/
/**
 * @fn            void rfitem_free (RFitem *rf_item)
 * @brief         Free allocated memory.
 * @param[in,out] rf_item Pointer to RFitem object
 * @return        none
 *
 * @fn            void rfitem_delete (RFitem *rf_item)
 * @brief         Free allocated memory and delete widgets.
 * @param[in,out] rf_item Pointer to RFitem object
 * @return        none
 */
/*----------------------------------------------------------------------------*/
void rfitem_free   (RFitem *rf_item);

void rfitem_delete (RFitem *rf_item);
/*----------------------------------------------------------------------------*/
/**
 * @fn         bool rfitem_get_checked (RFitem *rf_item)
 * @brief      Get check button checked value.
 * @param[in]  rf_item Pointer to RFitem object
 * @return     Checked value
 *
 * @fn         void rfitem_set_checked (RFitem     *rf_item,
 *                                      const bool  b_val)
 * @brief      Set check button checked value.
 * @param[out] rf_item Pointer to RFitem object
 * @param[in]  b_val   Check value
 * @return     none
 *
 * @fn            void rfitem_invert_checked (RFitem *rf_item)
 * @brief         Invert check item check value
 * @param[in,out] rf_item Pointer to RFitem object
 * @return        none
 */
/*----------------------------------------------------------------------------*/
static inline bool rfitem_get_checked     (const RFitem   *rf_item) {
    return gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (rf_item->check));
}
static inline void rfitem_set_checked     (RFitem         *rf_item,
                                           const bool      b_val) {
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (rf_item->check), b_val);
}
void     rfitem_invert_checked (RFitem         *rf_item);
/*----------------------------------------------------------------------------*/
/**
 * @fn  void rfitem_set_snew_from_sorg (RFitem *rf_item)
 *
 * @brief  Copy s_org string and length values to s_new.
 *
 * @param[in]  rf_item  RFitem object
 * @return     none
 *
 * @fn  void rfitem_set_sorg_from_snew (RFitem *rf_item)
 *
 * @brief  Copy s_new string and length values to s_org.
 *
 * @param[in]  rf_item  RFitem object
 * @return     none
 */
/*----------------------------------------------------------------------------*/
void     rfitem_set_snew_from_sorg (RFitem *rf_item);

void     rfitem_set_sorg_from_snew (RFitem *rf_item);
/*----------------------------------------------------------------------------*/
/**
 * @fn         const char * rfitem_get_snew (const RFitem *rf_item)
 * @brief      Get s_new value.
 * @param[in]  rf_item  RFitem object
 * @return     s_new value
 *
 * @fn         const char * rfitem_get_sorg (const RFitem *rf_item)
 * @brief      Get s_org value.
 * @param[in]  rf_item  RFitem object
 * @return     s_org value
 *
 * @fn         const char * rfitem_get_spth (const RFitem *rf_item)
 * @brief      Get s_pth value.
 * @param[in]  rf_item  RFitem object
 * @return     s_pth value
 */
/*----------------------------------------------------------------------------*/
static inline const char * rfitem_get_snew (const RFitem *rf_item) {
    return (const char *) rf_item->s_new;
}
static inline const char * rfitem_get_sorg (const RFitem *rf_item) {
    return (const char *) rf_item->s_org;
}
static inline const char * rfitem_get_spth (const RFitem *rf_item) {
    return (const char *) rf_item->s_pth;
}
/*----------------------------------------------------------------------------*/
/**
 * @fn        const char * rfitem_entry_get_text (RFitem *rf_item)
 * @brief     Get text from entry.
 * @param[in] rf_item RFitem with entry
 * @return    String with entry text
 *
 * @fn            void rfitem_entry_restore (RFitem *rf_item)
 * @brief         Restore original file name to entry
 * @param[in,out] rf_item Pointer to RFitem object
 * @return        none
 *
 * @fn            void rfitem_entry_check_and_update (RFitem     *rf_item,
                                                      const char *new_str)
 * @brief         Checks if the new entry text is equal to the previous value
 *                and changes it to a new value if it is not.
 * @param[in,out] rf_item RFitem with entry
 * @param[in]     new_str New entry value
 * @return        none
 */
/*----------------------------------------------------------------------------*/
static inline const char * rfitem_entry_get_text (const RFitem *rf_item) {
    return gtk_entry_get_text (GTK_ENTRY (rf_item->entry));
}
static inline void         rfitem_entry_restore  (RFitem       *rf_item) {
    gtk_entry_set_text (GTK_ENTRY (rf_item->entry), rf_item->s_org);
}
void rfitem_entry_check_and_update (RFitem       *rf_item,
                                    const char   *new_str);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Compare 2 RFitem objects.
 *
 * @param[in] rf_item1 First RFitem object
 * @param[in] rf_item2 Second RFitem object
 * @return    Compare value
 */
int rfitem_compare (const RFitem *rf_item1,
                    const RFitem *rf_item2);
/*----------------------------------------------------------------------------*/
/**
 * @fn  bool rfitem_is_file (const RFitem *rf_item)
 *
 * @brief  Check if RFitem type is a file.
 *
 * @param[in] rf_item Pointer to RFitem object
 * @return    true if RFitem describes a file type, false if not
 *
 * @fn  bool rfitem_is_folder (const RFitem *rf_item)
 *
 * @brief  Check if RFitem type is a directory.
 *
 * @param[in] rf_item Pointer to RFitem object
 * @return    true if RFitem describes a directory type, false if not
 *
 * @fn  bool rfitem_is_symlink (const RFitem *rf_item)
 *
 * @brief  Check if RFitem file/dir type is a symlink.
 *
 * @param[in] rf_item Pointer to RFitem object
 * @return    true if RFitem is a symlink, false if not
 *
 * @fn  bool rfitem_is_hidden  (const RFitem *rf_item)
 *
 * @brief  Check if RFitem file/dir is hidden.
 *
 * @param[in] rf_item Pointer to RFitem object
 * @return    true if RFitem is a hidden file/dir, false if not
 */
/*----------------------------------------------------------------------------*/
static inline bool rfitem_is_file        (const RFitem *rf_item) {
    return (rf_item->f_type == G_FILE_TYPE_REGULAR);
}
static inline bool rfitem_is_folder      (const RFitem *rf_item) {
    return (rf_item->f_type == G_FILE_TYPE_DIRECTORY);
}
static inline bool rfitem_is_symlink     (const RFitem *rf_item) {
    return rf_item->b_slink;
}
static inline bool rfitem_is_hidden      (const RFitem *rf_item) {
    return rf_item->b_hidden;
}
/*----------------------------------------------------------------------------*/
#endif

