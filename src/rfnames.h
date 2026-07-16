/**
 * @file  rfnames.h
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
 * @author Michal Babik <michal.babik@pm.me>
 */
#ifndef RFNAMES_H
#define RFNAMES_H

#include <gtk/gtk.h>
#include "rfitem.h"
/*----------------------------------------------------------------------------*/
/**
 * @struct RFnames
 *
 * @brief  File names buffer and entries
 *
 * @var   RFnames::cnt
 * @brief Count of file names
 *
 * @var   RFnames::file_box
 * @brief Box with file items
 *
 * @var   RFnames::rf_items
 * @brief Items with file name, entry, buttons
 */
typedef struct
RFnames {
    uint_fast32_t   cnt;
    GtkWidget      *file_box;
    RFitem        **rf_items;
} RFnames;
/*----------------------------------------------------------------------------*/
/**
 * @fn  RFnames * rfnames_new (void)
 *
 * @brief  Create new RFnames object.
 *
 * @return  New RFnames object
 *
 * @fn  void rfnames_free (RFnames *rf_names)
 *
 * @brief  Free allocated memory.
 *
 * @param[in,out] rf_names Pointer to RFnames object
 * @return        none
 */
/*----------------------------------------------------------------------------*/
RFnames * rfnames_new  (void) __attribute__ ((returns_nonnull));

void      rfnames_free (RFnames *rf_names);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Append file to RFnames.
 *
 * @param[out] rf_names Pointer to RFnames object
 * @param[in]  g_file   GFile object for file to add
 * @return     Box widget with item widgets
 */
GtkWidget * rfnames_append_gfile (RFnames *rf_names,
                                  GFile   *g_file);
/*----------------------------------------------------------------------------*/
/**
 * @fn  void rfnames_add_gfile_to_file_box (RFnames *rf_names,
 *                                          GFile   *g_file)
 *
 * @brief  Create new RFitem object using data from gfile item and add it
 *         to RFnames item.
 *
 * @param[out] rf_names Pointer to RFnames object
 * @param[in]  g_file   GFile object
 * @return     none
 *
 * @fn  void rfnames_add_sfile_to_file_box (RFnames    *rf_names,
 *                                          const char *s_fn)
 *
 * @brief  Create new RFitem object using data from s_fn string and add it
 *         to RFnames item.
 *
 * @param[out] rf_names Pointer to RFnames object
 * @param[in]  s_fn     String with file path
 * @return     none
 */
/*----------------------------------------------------------------------------*/
void rfnames_add_gfile_to_file_box (RFnames    *rf_names,
                                    GFile      *g_file);
/*----------------------------------------------------------------------------*/
void rfnames_add_sfile_to_file_box (RFnames    *rf_names,
                                    const char *s_fn);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get length of rf_items list
 *
 * @param[in] rf_names Pointer to RFnames object
 * @return    Length of list
 */
static inline uint_fast32_t rfnames_get_cnt (const RFnames *rf_names) {
    return rf_names->cnt;
}
/*----------------------------------------------------------------------------*/
/**
 * @fn  void rfnames_select_unselect_all (RFnames *rf_names)
 *
 * @brief  Select or unselect all check items on list.
 *
 * @param[out] rf_names RFnames list
 * @return     none
 *
 * @fn  void rfnames_select_invert (RFnames *rf_names)
 *
 * @brief  Invert selection of items on list.
 *
 * @param[out] rf_names RFnames list
 * @return     none
 */
/*----------------------------------------------------------------------------*/
void rfnames_select_unselect_all (RFnames *rf_names);

void rfnames_select_invert       (RFnames *rf_names);
/*----------------------------------------------------------------------------*/
/**
 * @fn  void rfnames_select_files (RFnames *rf_names)
 *
 * @brief  Select all items with file type on list.
 *
 * @param[out] rf_names RFnames list
 * @return     none
 *
 * @fn  void rfnames_unselect_files (RFnames *rf_names)
 *
 * @brief  Unselect all items with file type on list.
 *
 * @param[out] rf_names RFnames list
 * @return     none
 */
/*----------------------------------------------------------------------------*/
void rfnames_select_files        (RFnames *rf_names);

void rfnames_unselect_files      (RFnames *rf_names);
/*----------------------------------------------------------------------------*/
/**
 * @fn  void rfnames_select_folders (RFnames *rf_names)
 *
 * @brief  Select all items with folder type on list.
 *
 * @param[out] rf_names RFnames list
 * @return     none
 *
 * @fn  void rfnames_unselect_folders (RFnames *rf_names)
 *
 * @brief  Unselect all items with folder type on list.
 *
 * @param[out] rf_names RFnames list
 * @return     none
 */
/*----------------------------------------------------------------------------*/
void rfnames_select_folders      (RFnames *rf_names);

void rfnames_unselect_folders    (RFnames *rf_names);
/*----------------------------------------------------------------------------*/
/**
 * @fn  void rfnames_select_symlinks (RFnames *rf_names)
 *
 * @brief  Select all items with symlink type on list.
 *
 * @param[out] rf_names RFnames list
 * @return     none
 *
 * @fn  void rfnames_unselect_symlinks (RFnames *rf_names)
 *
 * @brief  Unselect all items with symlink type on list.
 *
 * @param[out] rf_names RFnames list
 * @return     none
 */
/*----------------------------------------------------------------------------*/
void rfnames_select_symlinks     (RFnames *rf_names);

void rfnames_unselect_symlinks   (RFnames *rf_names);
/*----------------------------------------------------------------------------*/
/**
 * @fn  void rfnames_select_hidden (RFnames *rf_names)
 *
 * @brief  Select all items with hidden type on list.
 *
 * @param[out] rf_names RFnames list
 * @return     none
 *
 * @fn  void rfnames_unselect_hidden (RFnames *rf_names)
 *
 * @brief  Unselect all items with hidden type on list.
 *
 * @param[out] rf_names RFnames list
 * @return     none
 */
/*----------------------------------------------------------------------------*/
void rfnames_select_hidden       (RFnames *rf_names);

void rfnames_unselect_hidden     (RFnames *rf_names);
/*----------------------------------------------------------------------------*/
/**
 * @fn            void rfnames_remove_selected (RFnames *rf_names)
 * @brief         Remove selected items from list.
 * @param[in,out] rf_names RFnames list
 * @return        none
 *
 * @fn            void rfnames_remove_all (RFnames *rf_names)
 * @brief         Remove all items from list.
 * @param[in,out] rf_names RFnames list
 * @return        none
 *
 * @fn            void rfnames_remove_all_files (RFnames *rf_names)
 * @brief         Remove all files from list.
 * @param[in,out] rf_names RFnames list
 * @return        none
 *
 * @fn            void rfnames_remove_all_folders (RFnames *rf_names)
 * @brief         Remove all folders from list.
 * @param[in,out] rf_names RFnames list
 * @return        none
 *
 * @fn            void rfnames_remove_all_symlinks (RFnames *rf_names)
 * @brief         Remove all symlinks from list.
 * @param[in,out] rf_names RFnames list
 * @return        none
 *
 * @fn            void rfnames_remove_all_hidden (RFnames *rf_names)
 * @brief         Remove all hidden files/folders from list.
 * @param[in,out] rf_names RFnames list
 * @return        none
 */
/*----------------------------------------------------------------------------*/
void rfnames_remove_selected     (RFnames *rf_names);

void rfnames_remove_all          (RFnames *rf_names);

void rfnames_remove_all_files    (RFnames *rf_names);

void rfnames_remove_all_folders  (RFnames *rf_names);

void rfnames_remove_all_symlinks (RFnames *rf_names);

void rfnames_remove_all_hidden   (RFnames *rf_names);
/*----------------------------------------------------------------------------*/
/**
 * @fn  void rfnames_restore_selected (RFnames *rf_names)
 *
 * @brief  Restore original file name in selected RFitem objects on list.
 *
 * @param[in,out] rf_names RFnames list to restore original names
 * @return        none
 *
 * @fn  void rfnames_restore_all (RFnames *rf_names)
 *
 * @brief  Restore original file name in all RFitem objects on list.
 *
 * @param[in,out] rf_names RFnames list to restore original names
 * @return        none
 *
 * @fn  void rfnames_restore_all_files (RFnames *rf_names)
 *
 * @brief  Restore original file name for all files from list.
 *
 * @param[in,out] rf_names RFnames list to restore original names
 * @return        none
 *
 * @fn  void rfnames_restore_all_folders (RFnames *rf_names)
 *
 * @brief  Restore original file name for all folders from list.
 *
 * @param[in,out] rf_names RFnames list to restore original names
 * @return        none
 *
 * @fn  void rfnames_restore_all_symlinks (RFnames *rf_names)
 *
 * @brief  Restore original file name for all symlinks from list.
 *
 * @param[in,out] rf_names RFnames list to restore original names
 * @return        none
 *
 * @fn  void rfnames_restore_all_hidden (RFnames *rf_names)
 *
 * @brief  Restore original file name for all hidden files/folders from list.
 *
 * @param[in,out] rf_names RFnames list to restore original names
 * @return        none
 */
/*----------------------------------------------------------------------------*/
void rfnames_restore_selected     (RFnames *rf_names);

void rfnames_restore_all          (RFnames *rf_names);

void rfnames_restore_all_files    (RFnames *rf_names);

void rfnames_restore_all_folders  (RFnames *rf_names);

void rfnames_restore_all_symlinks (RFnames *rf_names);

void rfnames_restore_all_hidden   (RFnames *rf_names);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Sort RFitem objects in RFnames list by the original path string.
 *
 * @param[in,out] rf_names RFnames object to sort RFitems
 * @return        none
 */
void rfnames_sort (RFnames *rf_names);
/*----------------------------------------------------------------------------*/
#endif

