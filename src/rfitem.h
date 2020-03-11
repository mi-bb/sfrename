/**
 * @file  rfitem.h
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
 * @brief  RFitem struncture and functions.
 * 
 * @author Michał Bąbik <michalb1981@o2.pl>
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
    GFileType  f_type;
    gboolean   b_slink;
    gboolean   b_hidden;
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
RFitem * rfitem_new_from_gfile      (GFile      *g_file)
         __attribute__ ((returns_nonnull));

RFitem * rfitem_new_from_sfile      (const char *s_pth)
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
 * @fn         gboolean rfitem_get_checked (RFitem *rf_item)
 * @brief      Get check button checked value.
 * @param[in]  rf_item Pointer to RFitem object
 * @return     Checked value
 *
 * @fn         void rfitem_set_checked (RFitem         *rf_item,
 *                                      const gboolean  b_val)
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
gboolean rfitem_get_checked    (const RFitem   *rf_item);

void     rfitem_set_checked    (RFitem         *rf_item,
                                const gboolean  b_val);

void     rfitem_invert_checked (RFitem         *rf_item);
/*----------------------------------------------------------------------------*/
/**
 * @fn         const char * rfitem_get_snew (const RFitem *rf_item)
 * @brief      Get s_new value.
 * @param[in]  rf_item  RFitem object
 * @return     s_new value
 *
 * @fn         void rfitem_set_snew (RFitem     *rf_item,
 *                                   const char *val)
 * @brief      Set s_new value.
 * @param[out] rf_item  RFitem object
 * @param[in]  val      New s_new value
 * @return     none
 *
 * @fn         const char * rfitem_get_sorg (const RFitem *rf_item)
 * @brief      Get s_org value.
 * @param[in]  rf_item  RFitem object
 * @return     s_org value
 *
 * @fn         void rfitem_set_sorg (RFitem       *rf_item,
 *                                   const char   *val)
 * @brief      Set s_org value.
 * @param[out] rf_item  RFitem object
 * @param[in]  val      New s_org value
 * @return     none
 *
 * @fn         const char * rfitem_get_spth (const RFitem *rf_item)
 * @brief      Get s_pth value.
 * @param[in]  rf_item  RFitem object
 * @return     s_pth value
 *
 * @fn         void rfitem_set_spth (RFitem     *rf_item,
 *                                   const char *val)
 * @brief      Set s_pth value.
 * @param[out] rf_item  RFitem object
 * @param[in]  val      New s_pth value
 * @return     none
 */
/*----------------------------------------------------------------------------*/
const char * rfitem_get_snew (const RFitem *rf_item) __attribute__ ((pure));

void         rfitem_set_snew (RFitem       *rf_item,
                              const char   *val);

const char * rfitem_get_sorg (const RFitem *rf_item) __attribute__ ((pure));

void         rfitem_set_sorg (RFitem       *rf_item,
                              const char   *val);

const char * rfitem_get_spth (const RFitem *rf_item) __attribute__ ((pure));

void         rfitem_set_spth (RFitem       *rf_item,
                              const char   *val);
/*----------------------------------------------------------------------------*/
/**
 * @fn        const char * rfitem_entry_get_text (RFitem *rf_item)
 * @brief     Get text from entry.
 * @param[in] rf_item RFitem with entry
 * @return    String with entry text
 *
 * @fn            void rfitem_entry_check_and_update (RFitem     *rf_item,
                                                      const char *new_str)
 * @brief         Checks if the new entry text is equal to the previous value
 *                and changes it to a new value if it is not.
 * @param[in,out] rf_item RFitem with entry
 * @param[in]     new_str New entry value
 * @return        none
 *
 * @fn            void rfitem_entry_restore (RFitem *rf_item)
 * @brief         Restore original file name to entry
 * @param[in,out] rf_item Pointer to RFitem object
 * @return        none
 */
/*----------------------------------------------------------------------------*/
const char * rfitem_entry_get_text         (const RFitem *rf_item);

void         rfitem_entry_check_and_update (RFitem       *rf_item,
                                            const char   *new_str);

void         rfitem_entry_restore          (RFitem       *rf_item);
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
 * @fn        gboolean rfitem_is_file (const RFitem *rf_item)
 * @brief     Check if RFitem type is a file.
 * @param[in] rf_item Pointer to RFitem object
 * @return    1 if RFitem describes a file type, 0 if not
 *
 * @fn        gboolean rfitem_is_folder (const RFitem *rf_item)
 * @brief     Check if RFitem type is a directory.
 * @param[in] rf_item Pointer to RFitem object
 * @return    1 if RFitem describes a directory type, 0 if not
 *
 * @fn        gboolean rfitem_is_symlink (const RFitem *rf_item)
 * @brief     Check if RFitem file/dir type is a symlink.
 * @param[in] rf_item Pointer to RFitem object
 * @return    1 if RFitem is a symlink, 0 if not
 *
 * @fn        gboolean rfitem_is_hidden  (const RFitem *rf_item)
 * @brief     Check if RFitem file/dir is hidden.
 * @param[in] rf_item Pointer to RFitem object
 * @return    1 if RFitem is a hidden file/dir, 0 if not
 */
/*----------------------------------------------------------------------------*/
gboolean rfitem_is_file    (const RFitem *rf_item) __attribute__ ((pure));

gboolean rfitem_is_folder  (const RFitem *rf_item) __attribute__ ((pure));

gboolean rfitem_is_symlink (const RFitem *rf_item) __attribute__ ((pure));

gboolean rfitem_is_hidden  (const RFitem *rf_item) __attribute__ ((pure));
/*----------------------------------------------------------------------------*/
#endif

