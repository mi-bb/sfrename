/**
 * @file  dialogs.h
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
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#ifndef DIALOGS_H
#define DIALOGS_H
/*----------------------------------------------------------------------------*/
#include <gtk/gtk.h>
/*----------------------------------------------------------------------------*/
/**
 * @brief  Called with the files chosen in the select files dialog.
 *
 * Not called if the dialog was dismissed or its parent window was destroyed.
 *
 * @param[in]     files     List model of GFile objects, owned by the caller
 * @param[in,out] user_data User data passed to add_files_dialog()
 */
typedef void (*DialogFilesFunc)  (GListModel *files,
                                  gpointer    user_data);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Called with the folder chosen in the select folder dialog.
 *
 * Not called if the dialog was dismissed or its parent window was destroyed.
 *
 * @param[in]     folder    Chosen folder, owned by the caller
 * @param[in,out] user_data User data passed to add_files_folder_dialog()
 */
typedef void (*DialogFolderFunc) (GFile    *folder,
                                  gpointer  user_data);
/*----------------------------------------------------------------------------*/
/**
 * @fn  void add_files_folder_dialog (GtkWindow        *parent,
 *                                    DialogFolderFunc  callback,
 *                                    gpointer          user_data)
 * @brief         Show select folder dialog, returns before it is closed.
 * @param[in]     parent    Dialog parent window
 * @param[in]     callback  Function to call with the chosen folder
 * @param[in,out] user_data Data for callback
 * @return        none
 *
 * @fn  void add_files_dialog (GtkWindow       *parent,
 *                             DialogFilesFunc  callback,
 *                             gpointer         user_data)
 * @brief         Show select files dialog, returns before it is closed.
 * @param[in]     parent    Dialog parent window
 * @param[in]     callback  Function to call with the chosen files
 * @param[in,out] user_data Data for callback
 * @return        none
 *
 * @fn  void about_app_dialog (GtkWindow *parent)
 * @brief     Show dialog with information about application.
 * @param[in] parent  Dialog parent window
 * @return    none
 */
/*----------------------------------------------------------------------------*/
void add_files_folder_dialog (GtkWindow        *parent,
                              DialogFolderFunc  callback,
                              gpointer          user_data);

void add_files_dialog        (GtkWindow        *parent,
                              DialogFilesFunc   callback,
                              gpointer          user_data);

void about_app_dialog        (GtkWindow        *parent);
/*----------------------------------------------------------------------------*/
#endif

