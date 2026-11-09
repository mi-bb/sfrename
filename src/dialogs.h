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
#include "defs.h"   /* FOLDER_SELECT_* flags */
/*----------------------------------------------------------------------------*/
/**
 * @fn  char * add_files_folder_dialog (GtkWindow *parent, int *options)
 * @brief         Select folder dialog.
 * @param[in]     parent  Dialog parent widget
 * @param[in,out] options Pointer to int value with options for selecting
 *                        files/dirs
 * @return        Foler path or nullptr.
 *
 * @fn  GSList * add_files_dialog (GtkWindow *parent)
 * @brief     Select files dialog.
 * @param[in] parent  Dialog parent widget
 * @return    List with files.
 *
 * @fn  void about_app_dialog (void);
 * @brief     Dialog with information about application.
 * @param[in] data  Unused pointer for event call
 * @return    none
 */
/*----------------------------------------------------------------------------*/
char   * add_files_folder_dialog (GtkWindow  *parent,
                                  int        *options);

GSList * add_files_dialog        (GtkWindow  *parent);

void     about_app_dialog        (gpointer    data);
/*----------------------------------------------------------------------------*/
#endif

