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
#include "images.h"
#include "dialogs.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  State of a running file dialog.
 */
typedef struct {
    GCancellable     *cancellable; /**< Cancelled when parent is destroyed */
    DialogFilesFunc   files_func;  /**< Callback of select files dialog */
    DialogFolderFunc  folder_func; /**< Callback of select folder dialog */
    gpointer          user_data;   /**< Data for callback */
} DialogRun;
/*----------------------------------------------------------------------------*/
/**
 * @brief  Parent window destroyed, cancel its running dialog.
 *
 * @param[in]     widget      The window which received the signal
 * @param[in,out] cancellable Cancellable of the dialog
 * @return        none
 */
static void
event_parent_destroy ([[maybe_unused]] GtkWidget *widget,
                      GCancellable *cancellable)
{
    g_cancellable_cancel (cancellable);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create state of a dialog tied to the lifetime of its parent.
 *
 * @param[in] parent Dialog parent window
 * @return    New DialogRun, freed with dialog_run_free()
 */
static DialogRun *
dialog_run_new (GtkWindow *parent)
{
    DialogRun *run = g_new0 (DialogRun, 1);

    run->cancellable = g_cancellable_new ();
    g_signal_connect_object (parent, "destroy",
                             G_CALLBACK (event_parent_destroy),
                             run->cancellable, 0);
    return run;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free state of a dialog.
 *
 * @param[in,out] run DialogRun to free
 * @return        none
 */
static void
dialog_run_free (DialogRun *run)
{
    g_object_unref (run->cancellable);
    g_free (run);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Print a dialog error, unless the dialog was just closed.
 *
 * @param[in,out] error Error to report and free
 * @return        none
 */
static void
dialog_report_error (GError *error)
{
    if (!g_error_matches (error, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_DISMISSED) &&
        !g_error_matches (error, GTK_DIALOG_ERROR, GTK_DIALOG_ERROR_CANCELLED)) {
        g_printerr ("File dialog failed: %s\n", error->message);
    }
    g_error_free (error);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Select folder dialog finished.
 *
 * @param[in]     source    GtkFileDialog object
 * @param[in]     result    Result of the dialog
 * @param[in,out] user_data DialogRun object
 * @return        none
 */
static void
folder_dialog_finished (GObject      *source,
                        GAsyncResult *result,
                        gpointer      user_data)
{
    DialogRun *run   = user_data;
    GError    *error = nullptr;
    GFile     *folder;

    folder = gtk_file_dialog_select_folder_finish (GTK_FILE_DIALOG (source),
                                                   result, &error);
    if (folder != nullptr) {
        if (!g_cancellable_is_cancelled (run->cancellable))
            run->folder_func (folder, run->user_data);
        g_object_unref (folder);
    }
    else {
        dialog_report_error (error);
    }
    dialog_run_free (run);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Select files dialog finished.
 *
 * @param[in]     source    GtkFileDialog object
 * @param[in]     result    Result of the dialog
 * @param[in,out] user_data DialogRun object
 * @return        none
 */
static void
files_dialog_finished (GObject      *source,
                       GAsyncResult *result,
                       gpointer      user_data)
{
    DialogRun  *run   = user_data;
    GError     *error = nullptr;
    GListModel *files;

    files = gtk_file_dialog_open_multiple_finish (GTK_FILE_DIALOG (source),
                                                  result, &error);
    if (files != nullptr) {
        if (!g_cancellable_is_cancelled (run->cancellable))
            run->files_func (files, run->user_data);
        g_object_unref (files);
    }
    else {
        dialog_report_error (error);
    }
    dialog_run_free (run);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Show select folder dialog.
 */
void
add_files_folder_dialog (GtkWindow        *parent,
                         DialogFolderFunc  callback,
                         gpointer          user_data)
{
    GtkFileDialog *dialog = gtk_file_dialog_new ();
    DialogRun     *run    = dialog_run_new (parent);

    run->folder_func = callback;
    run->user_data   = user_data;

    gtk_file_dialog_set_title (dialog, "Select Folder");
    gtk_file_dialog_select_folder (dialog, parent, run->cancellable,
                                   folder_dialog_finished, run);
    g_object_unref (dialog);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Show select files dialog.
 */
void
add_files_dialog (GtkWindow       *parent,
                  DialogFilesFunc  callback,
                  gpointer         user_data)
{
    GtkFileDialog *dialog = gtk_file_dialog_new ();
    DialogRun     *run    = dialog_run_new (parent);

    run->files_func = callback;
    run->user_data  = user_data;

    gtk_file_dialog_set_title (dialog, "Select Files");
    gtk_file_dialog_open_multiple (dialog, parent, run->cancellable,
                                   files_dialog_finished, run);
    g_object_unref (dialog);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Dialog with information about application.
 */
void
about_app_dialog (GtkWindow *parent)
{
    GtkWidget  *dialog;
    GdkTexture *logo;
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

    if ((logo = get_image (W_ICON_ABOUT)) != nullptr) {
        gtk_about_dialog_set_logo (GTK_ABOUT_DIALOG (dialog),
                                   GDK_PAINTABLE (logo));
        g_object_unref (logo);
    }
    gtk_window_set_transient_for (GTK_WINDOW (dialog), parent);
    gtk_window_set_modal (GTK_WINDOW (dialog), TRUE);
    /* A new dialog is made each time, closing must destroy this one */
    gtk_window_set_hide_on_close (GTK_WINDOW (dialog), FALSE);
    gtk_window_present (GTK_WINDOW (dialog));
}
/*----------------------------------------------------------------------------*/

