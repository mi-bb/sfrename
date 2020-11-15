/**
 * @file  dlgs.c
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
 * @brief  Application dialogs
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#include "defs.h"
#include "dlgs.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Select folder dialog.
 */
char *
add_files_folder_dialog (GtkWindow *gw_parent,
                         int       *i_opt)
{
    GtkWidget *gw_dialog;        /* Directory choose dialog */
    char      *s_folder  = NULL; /* Selected folder name */
    int        res       = 0;    /* Dialog run response */

    gw_dialog = gtk_file_chooser_dialog_new ("Select Folder",
                                          gw_parent,
                                          GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
                                          "_Cancel",
                                          GTK_RESPONSE_CANCEL,
                                          "_Open",
                                          GTK_RESPONSE_ACCEPT,
                                          NULL);

    GtkWidget *gw_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 4);
    GtkWidget *gw_chk_f = gtk_check_button_new_with_label (
            "Select files");
    GtkWidget *gw_chk_d = gtk_check_button_new_with_label (
            "Select directories");
    GtkWidget *gw_chk_s = gtk_check_button_new_with_label (
            "Select symlinks");
    GtkWidget *gw_chk_h = gtk_check_button_new_with_label (
            "Select hidden files / directories");
    GtkWidget *gw_chk_r = gtk_check_button_new_with_label (
            "Scan folders recursively");

    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gw_chk_f), 
                                  *i_opt & FOLDER_SELECT_FILES);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gw_chk_d),
                                  *i_opt & FOLDER_SELECT_FOLDERS);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gw_chk_s),
                                  *i_opt & FOLDER_SELECT_SYMLINKS);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gw_chk_h),
                                  *i_opt & FOLDER_SELECT_HIDDEN);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gw_chk_r),
                                  *i_opt & FOLDER_SCAN_RECURSIVELY);

    gtk_box_pack_start (GTK_BOX (gw_box), gw_chk_f, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (gw_box), gw_chk_d, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (gw_box), gw_chk_s, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (gw_box), gw_chk_h, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (gw_box), gw_chk_r, FALSE, FALSE, 0);
    gtk_widget_show_all (gw_box);
    gtk_file_chooser_set_extra_widget (GTK_FILE_CHOOSER (gw_dialog), gw_box);

    res = gtk_dialog_run (GTK_DIALOG (gw_dialog));

    if (res == GTK_RESPONSE_ACCEPT) {
        *i_opt = 0;
        if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (gw_chk_f))) {
            *i_opt |= FOLDER_SELECT_FILES;
        }
        if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (gw_chk_d))) {
            *i_opt |= FOLDER_SELECT_FOLDERS;
        }
        if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (gw_chk_s))) {
            *i_opt |= FOLDER_SELECT_SYMLINKS;
        }
        if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (gw_chk_h))) {
            *i_opt |= FOLDER_SELECT_HIDDEN;
        }
        if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (gw_chk_r))) {
            *i_opt |= FOLDER_SCAN_RECURSIVELY;
        }
        s_folder = gtk_file_chooser_get_filename (
                GTK_FILE_CHOOSER (gw_dialog));
    }
    gtk_widget_destroy (gw_dialog);
    return s_folder;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Select images dialog.
 */
GSList *
add_files_dialog (GtkWindow *gw_parent)
{
    GtkFileFilter *gff_filter;       /* File filter for dialog */
    GtkWidget     *gw_dialog;        /* Dialog widget */
    GSList        *gsl_files = NULL; /* Result file list */
    int            res       = 0;    /* Dialog run response */

    gw_dialog = gtk_file_chooser_dialog_new ("Select Files",
                                             gw_parent,
                                             GTK_FILE_CHOOSER_ACTION_OPEN,
                                             "_Cancel",
                                             GTK_RESPONSE_CANCEL,
                                             "_Open",
                                             GTK_RESPONSE_ACCEPT,
                                             NULL);
    gff_filter = gtk_file_filter_new ();
    gtk_file_filter_set_name (gff_filter, "All files");
    gtk_file_filter_add_pattern (gff_filter, "*");
    gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (gw_dialog), gff_filter);
    gtk_file_chooser_set_select_multiple (GTK_FILE_CHOOSER (gw_dialog), TRUE);

    res = gtk_dialog_run (GTK_DIALOG (gw_dialog));

    if (res == GTK_RESPONSE_ACCEPT) {
        gsl_files = gtk_file_chooser_get_filenames (
                GTK_FILE_CHOOSER (gw_dialog));
    }
    gtk_widget_destroy (gw_dialog);
    return gsl_files;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Dialog with information about application.
 */
void
about_app_dialog (gpointer data __attribute__ ((unused)))
{
    GtkWidget *gw_dialog;
    const char *s_authors[] = {"Michal Babik <michal.babik@protonmail.com> "
                               "http://init6.pomorze.pl", NULL};

    gw_dialog = gtk_about_dialog_new ();

    gtk_about_dialog_set_version (GTK_ABOUT_DIALOG (gw_dialog), APP_VER);

    gtk_about_dialog_set_copyright (GTK_ABOUT_DIALOG (gw_dialog), 
            "Copyright (C) 2019-2020 Michal Babik");

    gtk_about_dialog_set_comments (GTK_ABOUT_DIALOG (gw_dialog), APP_DSC);
    gtk_about_dialog_set_license_type (GTK_ABOUT_DIALOG (gw_dialog),
                                       GTK_LICENSE_GPL_3_0);

    gtk_about_dialog_set_website (GTK_ABOUT_DIALOG (gw_dialog), APP_WEB);

    gtk_about_dialog_set_authors (GTK_ABOUT_DIALOG (gw_dialog), s_authors);
    gtk_dialog_run (GTK_DIALOG (gw_dialog));
    gtk_widget_destroy (gw_dialog);
}
/*----------------------------------------------------------------------------*/

