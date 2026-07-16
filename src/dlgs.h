/**
 * @file  dlgs.h
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
 * @brief  Application dialogs
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#ifndef DLGS_H
#define DLGS_H
/*----------------------------------------------------------------------------*/
/**
 * @def   FOLDER_SELECT_FILES
 * @brief Option for select files in folder
 * @def   FOLDER_SELECT_FOLDERS
 * @brief Option for select folders in folder
 * @def   FOLDER_SELECT_SYMLINKS
 * @brief Option for select file/dir symlinks
 * @def   FOLDER_SELECT_HIDDEN
 * @brief Option for select hidden files/dirs
 * @def   FOLDER_SCAN_RECURSIVELY
 * @brief Option for scanning directory recursively
 */
#define FOLDER_SELECT_FILES      0x01
#define FOLDER_SELECT_FOLDERS    0x02
#define FOLDER_SELECT_SYMLINKS   0x04
#define FOLDER_SELECT_HIDDEN     0x08
#define FOLDER_SCAN_RECURSIVELY  0x10

#include <gtk/gtk.h>
/*----------------------------------------------------------------------------*/
/**
 * @fn  char * add_files_folder_dialog (GtkWindow *gw_parent, int *i_opt)
 * @brief         Select folder dialog.
 * @param[in]     gw_parent Dialog parent widget
 * @param[in,out] i_opt     Pointer to int value with options for selecting
 *                          files/dirs
 * @return        Foler path or NULL.
 *
 * @fn  GSList * add_files_dialog (GtkWindow *gw_parent)
 * @brief     Select files dialog.
 * @param[in] gw_parent  Dialog parent widget
 * @return    List with files.
 *
 * @fn  void about_app_dialog (void);
 * @brief     Dialog with information about application.
 * @param[in] data  Unused pointer for event call
 * @return    none
 */
/*----------------------------------------------------------------------------*/
char   * add_files_folder_dialog (GtkWindow  *gw_parent,
                                  int        *i_opt);

GSList * add_files_dialog        (GtkWindow  *gw_parent);

void     about_app_dialog        (gpointer data);
/*----------------------------------------------------------------------------*/
#endif

