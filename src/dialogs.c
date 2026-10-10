/**
 * @file  dialogs.c
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
#include "defs.h"
#include "dialogs.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Select folder dialog.
 */
char *
add_files_folder_dialog (GtkWindow *parent,
                         int       *options)
{
    GtkWidget *dialog;            /* Directory choose dialog */
    char      *folder  = nullptr; /* Selected folder name */
    int        result  = 0;       /* Dialog run response */

    dialog = gtk_file_chooser_dialog_new ("Select Folder",
                                          parent,
                                          GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
                                          "_Cancel",
                                          GTK_RESPONSE_CANCEL,
                                          "_Open",
                                          GTK_RESPONSE_ACCEPT,
                                          nullptr);

    GtkWidget *box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 4);
    GtkWidget *chk_files = gtk_check_button_new_with_label (
            "Select files");
    GtkWidget *chk_dirs = gtk_check_button_new_with_label (
            "Select directories");
    GtkWidget *chk_links = gtk_check_button_new_with_label (
            "Select symlinks");
    GtkWidget *chk_hidden = gtk_check_button_new_with_label (
            "Select hidden files / directories");
    GtkWidget *chk_rec = gtk_check_button_new_with_label (
            "Scan folders recursively");

    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (chk_files), 
                                  *options & FOLDER_SELECT_FILES);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (chk_dirs),
                                  *options & FOLDER_SELECT_FOLDERS);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (chk_links),
                                  *options & FOLDER_SELECT_SYMLINKS);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (chk_hidden),
                                  *options & FOLDER_SELECT_HIDDEN);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (chk_rec),
                                  *options & FOLDER_SCAN_RECURSIVELY);

    gtk_box_pack_start (GTK_BOX (box), chk_files,  FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (box), chk_dirs,   FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (box), chk_links,  FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (box), chk_hidden, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (box), chk_rec,    FALSE, FALSE, 0);
    gtk_widget_show_all (box);
    gtk_file_chooser_set_extra_widget (GTK_FILE_CHOOSER (dialog), box);

    result = gtk_dialog_run (GTK_DIALOG (dialog));

    if (result == GTK_RESPONSE_ACCEPT) {
        *options = 0;
        if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (chk_files))) {
            *options |= FOLDER_SELECT_FILES;
        }
        if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (chk_dirs))) {
            *options |= FOLDER_SELECT_FOLDERS;
        }
        if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (chk_links))) {
            *options |= FOLDER_SELECT_SYMLINKS;
        }
        if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (chk_hidden))) {
            *options |= FOLDER_SELECT_HIDDEN;
        }
        if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (chk_rec))) {
            *options |= FOLDER_SCAN_RECURSIVELY;
        }
        folder = gtk_file_chooser_get_filename (
                GTK_FILE_CHOOSER (dialog));
    }
    gtk_widget_destroy (dialog);
    return folder;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Select images dialog.
 */
GSList *
add_files_dialog (GtkWindow *parent)
{
    GtkFileFilter *filter;              /* File filter for dialog */
    GtkWidget     *dialog;              /* Dialog widget */
    GSList        *gsl_files = nullptr; /* Result file list */
    int            result    = 0;       /* Dialog run response */

    dialog = gtk_file_chooser_dialog_new ("Select Files",
                                             parent,
                                             GTK_FILE_CHOOSER_ACTION_OPEN,
                                             "_Cancel",
                                             GTK_RESPONSE_CANCEL,
                                             "_Open",
                                             GTK_RESPONSE_ACCEPT,
                                             nullptr);
    filter = gtk_file_filter_new ();
    gtk_file_filter_set_name (filter, "All files");
    gtk_file_filter_add_pattern (filter, "*");
    gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (dialog), filter);
    gtk_file_chooser_set_select_multiple (GTK_FILE_CHOOSER (dialog), TRUE);

    result = gtk_dialog_run (GTK_DIALOG (dialog));

    if (result == GTK_RESPONSE_ACCEPT) {
        gsl_files = gtk_file_chooser_get_filenames (
                GTK_FILE_CHOOSER (dialog));
    }
    gtk_widget_destroy (dialog);
    return gsl_files;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Dialog with information about application.
 */
void
about_app_dialog ([[maybe_unused]] gpointer data)
{
    GtkWidget *dialog;
    const char *authors[] = {"Michal Babik <michal.babik@protonmail.com> "
                               "http://init6.pomorze.pl", nullptr};

    dialog = gtk_about_dialog_new ();

    gtk_about_dialog_set_version (GTK_ABOUT_DIALOG (dialog), APP_VER);

    gtk_about_dialog_set_copyright (GTK_ABOUT_DIALOG (dialog), 
            "Copyright (C) 2019-2026 Michal Babik");

    gtk_about_dialog_set_comments (GTK_ABOUT_DIALOG (dialog), APP_DSC);
    gtk_about_dialog_set_license_type (GTK_ABOUT_DIALOG (dialog),
                                       GTK_LICENSE_GPL_3_0);

    gtk_about_dialog_set_website (GTK_ABOUT_DIALOG (dialog), APP_WEB);

    gtk_about_dialog_set_authors (GTK_ABOUT_DIALOG (dialog), authors);
    gtk_dialog_run (GTK_DIALOG (dialog));
    gtk_widget_destroy (dialog);
}
/*----------------------------------------------------------------------------*/

