/** 
 *  _____           _ _    _____ _ _        _____                           
 * |   __|_____ ___| | |  |   __|_| |___   | __  |___ ___ ___ _____ ___ ___ 
 * |__   |     | .'| | |  |   __| | | -_|  |    -| -_|   | .'|     | -_|  _|
 * |_____|_|_|_|__,|_|_|  |__|  |_|_|___|  |__|__|___|_|_|__,|_|_|_|___|_|  
 *                                                                          
 * @file  sfrename.c
 * @copyright Copyright (C) 2019-2026 Michal Babik
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @brief Small File Renamer program source
 *
 * Program renames files.
 *
 * @date November 14, 2026
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <ctype.h>
#include <inttypes.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include <gio/gio.h>
#include <gdk/gdkkeysyms.h>
#include "dialogs.h"
#include "string_functions.h"
#include "rendata.h"
#include "rconfig.h"
#include "rrename.h"
#include "namefn.h"
#include "images.h"
#include "defs.h"
/*----------------------------------------------------------------------------*/
/** @brief Object data key with the option value of a radio button */
constexpr char RADIO_VALUE_KEY[] = "sfrename-radio-value";
/** @brief Object data key with the FOLDER_* flag of a check button */
constexpr char DIRSEL_FLAG_KEY[] = "sfrename-dirsel-flag";
/*----------------------------------------------------------------------------*/
/**
 * @brief  Apply changes to one file name in buffer and update its entry.
 *
 * Items that are not checked are left untouched.
 *
 * @param[in,out] data RenData object with file list and settings
 * @param[in]     i    Index of the item in the file list
 * @return        none
 */
static void
file_name_update_changes (RenData             *data,
                          const uint_fast32_t  i)
{
    RFitem *item = data->names->items[i]; /* RFitem object to process */

    if (!rfitem_get_checked (item))
        return;

    /* copy original name to new to process */
    rfitem_set_new_name_from_org_name (item);

    /* Execute rename functions */
    name_to_upcase_lowercase (data, i);
    name_spaces_underscores (data, i);
    name_delete_chars (data, i);
    name_replace_strings (data, i);
    name_insert_string (data, i);
    name_overwrite_string (data, i);
    name_number_string (data, i);
    /* Update file name entry if name has changed */
    rfitem_entry_sync (item);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Apply changes to all file names in buffer and update entries.
 *
 * @param[in,out] data RenData object with file list and settings
 * @return        none
 */
static void
file_names_update_changes (RenData *data)
{
    uint_fast32_t i = rfnames_get_cnt (rendata_get_rfnames (data));

    while (i--)
        file_name_update_changes (data, i);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Scan folder folder content, check for adding and add files, folders.
 *
 * @param[out] names   RFnames item with file list
 * @param[in]  dir     Directory to scan
 * @param[in]  options Options with info which items to add (files, folders)
 * @return     none
 */
static void
enumerate_folder (RFnames   *names,
                  GFile     *dir,
                  const int  options)
{
    GFileEnumerator *f_enum;
    GError          *g_err = nullptr;
    GFile           *file;
    GFileInfo       *f_info;
    GFileType        f_type;

    #ifdef DEBUG
        printf ("Enumerating folder %s\n", g_file_peek_path (dir));
    #endif

    f_enum = g_file_enumerate_children (dir,
                                        "standard::*",
                                        G_FILE_QUERY_INFO_NONE,
                                        nullptr,
                                        &g_err);
    if (f_enum == nullptr) {
        g_printerr ("Could not read folder %s: %s\n",
                    g_file_peek_path (dir), g_err->message);
        g_error_free (g_err);
        return;
    }
    while (g_file_enumerator_iterate (f_enum, &f_info, &file, nullptr, &g_err)
           && f_info != nullptr) {
        f_type = g_file_info_get_file_type (f_info);
        /* If hidden file/directory was not set skip adding it */
        if (g_file_info_get_is_hidden (f_info) &&
            !(options & FOLDER_SELECT_HIDDEN)) {
            continue;
        }
        /* If symlink file/directory was not set skip adding it */
        if (g_file_info_get_is_symlink (f_info) &&
            !(options & FOLDER_SELECT_SYMLINKS)) {
            continue;
        }
        if (f_type == G_FILE_TYPE_DIRECTORY) {
            if (options & FOLDER_SELECT_FOLDERS) {
                rfnames_add_gfile (names, file);
            }
            /* Do not enter symlinks, one can lead back to a parent folder */
            if ((options & FOLDER_SCAN_RECURSIVELY) &&
                !g_file_info_get_is_symlink (f_info)) {
                enumerate_folder (names, file, options);
            }
        }
        if (f_type == G_FILE_TYPE_REGULAR &&
            (options & FOLDER_SELECT_FILES)) {
            rfnames_add_gfile (names, file);
        }
    }
    if (g_err != nullptr) {
        g_printerr ("Could not read all of folder %s: %s\n",
                    g_file_peek_path (dir), g_err->message);
        g_error_free (g_err);
    }
    g_object_unref (f_enum);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Checks if file is a folder and calls enumerate_folder if it is.
 *
 * @param[out] names    RFnames item with file list
 * @param[in]  dir      Directory
 * @param[in]  options  Options with info which items to add (files, folders)
 * @return     none
 */
static void
get_folder_content (RFnames   *names,
                    GFile     *dir,
                    const int  options)
{
    GError    *g_err = nullptr;
    GFileInfo *f_info;
    GFileType  f_type;

    /* GTK4 file dialogs can return remote folders */
    if (g_file_peek_path (dir) == nullptr) {
        g_autofree char *name = g_file_get_parse_name (dir);
        g_printerr ("Skipping %s: not a local folder\n", name);
        return;
    }
    f_info = g_file_query_info (dir,
                                "standard::*",
                                G_FILE_QUERY_INFO_NONE,
                                nullptr,
                                &g_err);
    if (f_info != nullptr) {
        f_type = g_file_info_get_file_type (f_info);
        g_object_unref (f_info);

        if (f_type == G_FILE_TYPE_DIRECTORY) {
            enumerate_folder (names, dir, options);
        }
    }
    else {
        g_printerr ("Could not read folder %s: %s\n",
                    g_file_peek_path (dir), g_err->message);
        g_error_free (g_err);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get the option value stored on a radio button.
 *
 * Values are explicit because they are saved in the config file: the top
 * button of a group is 2 (no change / name and ext), then 1, then 0.
 *
 * @param[in] radio  Radio button
 * @return    Option value
 */
static int8_t
radio_get_value (GtkCheckButton *radio)
{
    return (int8_t) GPOINTER_TO_INT (g_object_get_data (G_OBJECT (radio),
                                                        RADIO_VALUE_KEY));
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Destroy program window event.
 *
 * @param[in] widget The object which received the signal
 * @param[in] data   User data set when the signal handler was connected
 * @return    none
 */
static void
event_close (GtkWidget *widget,
             [[maybe_unused]] gpointer data)
{
    GtkRoot *root = gtk_widget_get_root (widget);

    if (root != nullptr)
        gtk_window_destroy (GTK_WINDOW (root));
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Rename button clicked.
 *
 * Tries to rename the file of each checked item to its pending name.
 * Unchecked items are left alone.
 * If File can not be renamed it prints the reason and its entry is reset
 * to the original name, so the list shows the name the file still has.
 * Renamed and unchanged items get the current settings applied again.
 * If exit after rename is selected, application is closed after rename.
 *
 * @param[in]     widget  Button which received the signal
 * @param[in,out] data    RenData object with file list and settings
 * @return     none
 */
static void
event_click_rename (GtkWidget *widget,
                    RenData   *data)
{
    char          *old_name      = nullptr; /* Old file name */
    char          *new_name      = nullptr; /* New file name */
    RenResult      result        = REN_OK;  /* Renaming result */
    uint_fast32_t  renamed_count = 0;       /* Number of renamed files */
    uint_fast32_t  checked_count = 0;       /* Number of checked files */
    uint_fast32_t  names_count   = 0;       /* Number of files on list */
    RFitem        *item;                    /* Processed RFitem object */
    const bool     exit_after    = rendata_get_renexit (data);

    names_count = rfnames_get_cnt (rendata_get_rfnames (data));

    for (uint_fast32_t i = 0; i < names_count; ++i) {
        item = data->names->items[i];

        /* Copies for messages, rrename_item() updates the item's names */
        old_name = g_strdup (rfitem_get_org_name (item));
        new_name = g_strdup (rfitem_get_new_name (item));

        result = rrename_item (item);

        if (result != REN_SKIP)
            ++checked_count;

        switch (result) {

            case REN_OK:
                printf ("File: %s renamed to: %s\n", old_name, new_name);
                ++renamed_count;
                break;

            case REN_NC:
                printf ("No change in file: %s\n", old_name);
                break;

            case REN_NOT_REN:
                printf ("File: %s could not be renamed\n", old_name);
                break;

            case REN_EXISTS:
                printf ("File: %s already exists\n", new_name);
                break;

            case REN_SKIP:
                break;
        }
        if (result == REN_OK || result == REN_NC) {
            /* Apply current settings to the (possibly new) name */
            if (!exit_after)
                file_name_update_changes (data, i);
        }
        else if (result != REN_SKIP) {
            /* Rename failed, new_name is back to the original one. Show it
             * in the entry, so it is visible the file still has its old
             * name. */
            rfitem_entry_sync (item);
        }
        g_free (old_name);
        g_free (new_name);
    }
    printf ("Renamed %" PRIdFAST32 " files of %" PRIdFAST32 "\n",
            renamed_count, checked_count);

    /* exit application if "Exit after rename" checkbox was selected */
    if (exit_after)
        event_close (widget, nullptr);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Insert string text position in SpinButton changed.
 *
 * Copies SpinButton value to RFiles and calls to update file name changes if
 * insert string text is not empty.
 *
 * @param[in]  sp_button SpinButton which received the signal
 * @param[out] data      RenData object with file list and settings
 * @return     none
 */
static void
event_insert_pos_changed (GtkSpinButton *sp_button,
                          RenData       *data)
{
    rinsovr_set_pos (data->ins,
                     (uint8_t) gtk_spin_button_get_value_as_int (sp_button));

    if (!rinsovr_empty (rendata_get_rinsert (data)))
        file_names_update_changes (data);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Insert string text in Entry changed.
 *
 * Copies string to insert to RFiles and calls to update file
 * name changes.
 *
 * @param[in]  widget  Entry widget which received the signal
 * @param[out] data    RenData object with file list and settings
 * @return     none
 */
static void
event_insert_string_entry_changed (GtkWidget *widget,
                                   RenData   *data)
{
    const char *text = gtk_editable_get_text (GTK_EDITABLE (widget));

    rinsovr_set_text (data->ins, text);

    file_names_update_changes (data);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Overwrite string text position in SpinButton changed.
 *
 * Copies SpinButton value to RFiles and calls to update file name changes if
 * overwrite string text is not empty.
 *
 * @param[in]  sp_button SpinButton which received the signal
 * @param[out] data      RenData object with file list and settings
 * @return     none
 */
static void
event_overwrite_pos_changed (GtkSpinButton *sp_button,
                             RenData       *data)
{
    rinsovr_set_pos (data->ovrw,
                     (uint8_t) gtk_spin_button_get_value_as_int (sp_button));

    if (!rinsovr_empty (rendata_get_roverwr (data)))
        file_names_update_changes (data);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Overwrite string text in Entry changed.
 *
 * Copies string to overwrite to RFiles and calls to update file
 * name changes.
 *
 * @param[in]  widget  Entry widget which received the signal
 * @param[out] data    RenData object with file list and settings
 * @return     none
 */
static void
event_overwrite_string_entry_changed (GtkWidget *widget,
                                      RenData   *data)
{
    const char *text = gtk_editable_get_text (GTK_EDITABLE (widget));

    rinsovr_set_text (data->ovrw, text);

    file_names_update_changes (data);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Delete char number of chars to delete in SpinButton changed.
 *
 * Reads SpinButon value of chars to delete and calls to update file name
 * changes.
 *
 * @param[in]  sp_button SpinButton which received the signal
 * @param[out] data      RenData object with file list and settings
 * @return     none
 */
static void
event_delete_cnt_changed (GtkSpinButton *sp_button,
                          RenData       *data)
{
    rdelete_set_cnt (data->del,
                     (uint8_t) gtk_spin_button_get_value_as_int (sp_button));

    file_names_update_changes (data);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Delete char position velue in SpinButton changed.
 *
 * Reads SpinButon value and if number of chars to delete is greater then 0
 * it calls to update file name changes.
 *
 * @param[in]  sp_button SpinButton which received the signal
 * @param[out] data      RenData object with file list and settings
 * @return     none
 */
static void
event_delete_pos_changed (GtkSpinButton *sp_button,
                          RenData       *data)
{
    rdelete_set_pos (data->del,
                     (uint8_t) gtk_spin_button_get_value_as_int (sp_button));

    if (rdelete_get_cnt (rendata_get_rdelete (data)) > 0)
        file_names_update_changes (data);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Upper/lower case radio button changed.
 *
 * Both the button turned off and the one turned on emit the signal, only the
 * second one is handled.
 *
 * @param[in]  radio   Radio button which received the signal
 * @param[out] data    RenData object with file list and settings
 * @return     none
 */
static void
event_case_radio_active (GtkCheckButton *radio,
                         RenData        *data)
{
    if (gtk_check_button_get_active (radio)) {
        rendata_set_uplo (data, radio_get_value (radio));
        file_names_update_changes (data);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Space/underscore replace radio button changed.
 *
 * Both the button turned off and the one turned on emit the signal, only the
 * second one is handled.
 *
 * @param[in]  radio   Radio button which received the signal
 * @param[out] data    RenData object with file list and settings
 * @return     none
 */
static void
event_spaces_radio_active (GtkCheckButton *radio,
                           RenData        *data)
{
    if (gtk_check_button_get_active (radio)) {
        rendata_set_spaces (data, radio_get_value (radio));
        file_names_update_changes (data);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Changed "replace from" value in replace strings section.
 *
 * Copies "replace from" value to RFiles and runs file name update changes
 * function.
 *
 * @param[in]  widget  Entry widget which received the signal
 * @param[out] data    RenData object with file list and settings
 * @return     none
 */
static void
event_replace_from_entry_changed (GtkWidget *widget,
                                  RenData   *data)
{
    const char *text = gtk_editable_get_text (GTK_EDITABLE (widget));

    rreplace_set_from (data->replace, text);

    file_names_update_changes (data);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Changed "replace to" value in replace strings section.
 *
 * Copies "replace to" value to RFiles, checks if "replace from" string is not
 * empty and calls update changes function.
 *
 * @param[in]  widget  Entry widget which received the signal
 * @param[out] data    RenData object with file list and settings
 * @return     none
 */
static void
event_replace_to_entry_changed (GtkWidget *widget,
                                RenData   *data)
{
    const char *text = gtk_editable_get_text (GTK_EDITABLE (widget));

    rreplace_set_to (data->replace, text);

    if (!rreplace_empty_from (rendata_get_rreplace (data)))
        file_names_update_changes (data);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Changed state of radio button responsible for applying changes to
 *         file name, file extension or both.
 *
 * Both the button turned off and the one turned on emit the signal, only the
 * second one is handled.
 *
 * @param[in]  radio   Radio button which received the signal
 * @param[out] data    RenData object with file list and settings
 * @return     none
 */
static void
event_apply_radio_active (GtkCheckButton *radio,
                          RenData        *data)
{
    if (gtk_check_button_get_active (radio)) {
        rendata_set_applyto (data, radio_get_value (radio));
        file_names_update_changes (data);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Number names CheckBox state changed.
 *
 * @param[in]  check   CheckButton which received the signal
 * @param[out] data    RenData object with file list and settings
 * @return     none
 */
static void
event_toggle_number_names (GtkCheckButton *check,
                           RenData        *data)
{
    rnumber_set_opt (data->number,
            (int8_t) gtk_check_button_get_active (check));

    file_names_update_changes (data);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Numbering strings start value in SpinButton changed.
 *
 * @param[in]  sp_button SpinButton which received the signal
 * @param[out] data      RenData object with file list and settings
 * @return     none
 */
static void
event_number_start_changed (GtkSpinButton *sp_button,
                            RenData        *data)
{
    rnumber_set_start (data->number,
        (uint32_t) gtk_spin_button_get_value_as_int (sp_button));

    if (rnumber_get_opt (rendata_get_rnumber (data)))
        file_names_update_changes (data);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Numbering strings position in string SpinButton changed.
 *
 * @param[in]  sp_button SpinButton which received the signal
 * @param[out] data      RenData object with file list and settings
 * @return     none
 */
static void
event_number_pos_changed (GtkSpinButton *sp_button,
                          RenData       *data)
{
    rnumber_set_pos (data->number,
        (uint8_t) gtk_spin_button_get_value_as_int (sp_button));

    if (rnumber_get_opt (rendata_get_rnumber (data)))
        file_names_update_changes (data);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Exit after rename CheckBox state changed.
 *
 * Sets renexit value in r_files structure based on the value of ChackBox.
 *
 * @param[in]  check   CheckButton which received the signal
 * @param[out] data    RenData object with file list and settings
 * @return     none
 */
static void
event_toggle_rename_exit (GtkCheckButton *check,
                          RenData        *data)
{
    rendata_set_renexit (data, gtk_check_button_get_active (check));
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Remember options on exit CheckBox state changed.
 *
 * Sets rememberopt value in r_files structure based on the value of
 * ChackBox.
 *
 * @param[in]  check   CheckButton which received the signal
 * @param[out] data    RenData object with file list and settings
 * @return     none
 */
static void
event_toggle_remember_options (GtkCheckButton *check,
                               RenData        *data)
{
    rendata_set_rememberopt (data, gtk_check_button_get_active (check));
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Option for adding files from a folder toggled.
 *
 * @param[in]     check  CheckButton which received the signal
 * @param[in,out] data   RenData object with settings
 * @return        none
 */
static void
event_toggle_dirsel (GtkCheckButton *check,
                     RenData        *data)
{
    const int flag   = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (check),
                                                           DIRSEL_FLAG_KEY));
    const int dirsel = rendata_get_dirsel (data);

    rendata_set_dirsel (data, (int8_t) (gtk_check_button_get_active (check) ?
                                        (dirsel | flag) : (dirsel & ~flag)));
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Escape pressed in main window, close it.
 *
 * The shortcut runs after the focused widget, so an open menu takes Escape
 * first and only closes itself.
 *
 * @param[in] widget  Main window
 * @param[in] args    Unused shortcut arguments
 * @param[in] data    Unused user data
 * @return    TRUE, the key press is handled
 */
static gboolean
event_win_escape (GtkWidget *widget,
                  [[maybe_unused]] GVariant *args,
                  [[maybe_unused]] gpointer  data)
{
    event_close (widget, nullptr);
    return TRUE;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Focus the first entry if files were added to an empty list.
 *
 * @param[in] names      RFnames object with file list
 * @param[in] prev_count Number of items before adding
 * @return    none
 */
static void
focus_first_added (RFnames             *names,
                   const uint_fast32_t  prev_count)
{
    if (prev_count == 0)
        rfnames_focus_first (names);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Files chosen in the select files dialog.
 *
 * @param[in]     files     List model of GFile objects
 * @param[in,out] user_data RFnames object with file list
 * @return        none
 */
static void
add_files_finished (GListModel *files,
                    gpointer    user_data)
{
    RFnames             *names      = user_data;
    const uint_fast32_t  prev_count = rfnames_get_cnt (names);
    const guint          count      = g_list_model_get_n_items (files);

    for (guint i = 0; i < count; ++i) {
        GFile *file = g_list_model_get_item (files, i);
        rfnames_add_gfile (names, file);
        g_object_unref (file);
    }
    focus_first_added (names, prev_count);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Add files button pressed.
 *
 * @param[in]     button  Button which received the signal
 * @param[in,out] data    RenData object with file list
 * @return        none
 */
static void
event_click_add_files (GtkWidget *button,
                       RenData   *data)
{
    add_files_dialog (GTK_WINDOW (gtk_widget_get_root (button)),
                      add_files_finished, data->names);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Folder chosen in the select folder dialog.
 *
 * @param[in]     folder    Chosen folder
 * @param[in,out] user_data RenData object with file list and settings
 * @return        none
 */
static void
add_folder_finished (GFile    *folder,
                     gpointer  user_data)
{
    RenData             *data       = user_data;
    const uint_fast32_t  prev_count = rfnames_get_cnt (data->names);
    const int            options    = rendata_get_dirsel (data);

    if (options > 0)
        get_folder_content (data->names, folder, options);

    focus_first_added (data->names, prev_count);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Add files from folder button pressed.
 *
 * @param[in]     button  Button which received the signal
 * @param[in,out] data    RenData object with settings
 * @return        none
 */
static void
event_click_add_folder_files (GtkWidget *button,
                              RenData   *data)
{
    GtkWidget *popover = gtk_widget_get_ancestor (button, GTK_TYPE_POPOVER);

    if (popover != nullptr)
        gtk_popover_popdown (GTK_POPOVER (popover));

    add_files_folder_dialog (GTK_WINDOW (gtk_widget_get_root (button)),
                             add_folder_finished, data);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Application info button pressed.
 *
 * @param[in] button  Button which received the signal
 * @param[in] data    Unused user data
 * @return    none
 */
static void
event_click_about (GtkWidget *button,
                   [[maybe_unused]] gpointer data)
{
    about_app_dialog (GTK_WINDOW (gtk_widget_get_root (button)));
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Toolbar menu that a list action is shown in.
 */
typedef enum ListMenu : uint_fast8_t {
    LIST_MENU_NONE,    /**< Not in a menu, run by a toolbar button */
    LIST_MENU_SELECT,  /**< SEL menu */
    LIST_MENU_RESTORE, /**< RES menu */
    LIST_MENU_REMOVE   /**< DEL menu */
} ListMenu;
/*----------------------------------------------------------------------------*/
/**
 * @brief  Action working on the file list.
 */
typedef struct {
    const char  *name;              /**< Action name, without "win." */
    const char  *label;             /**< Menu label */
    ListMenu     menu;              /**< Menu showing the action */
    void       (*run) (RFnames *);  /**< Function to run */
} ListAction;
/*----------------------------------------------------------------------------*/
static const ListAction list_actions[] = {
    {"select-toggle",     nullptr, LIST_MENU_NONE, rfnames_select_unselect_all},
    {"restore-selected",  nullptr, LIST_MENU_NONE, rfnames_restore_selected},
    {"remove-selected",   nullptr, LIST_MENU_NONE, rfnames_remove_selected},
    {"sort",              nullptr, LIST_MENU_NONE, rfnames_sort},

    {"select-files",      "Select all files",         LIST_MENU_SELECT,
                          rfnames_select_files},
    {"select-folders",    "Select all directories",   LIST_MENU_SELECT,
                          rfnames_select_folders},
    {"select-symlinks",   "Select all symlinks",      LIST_MENU_SELECT,
                          rfnames_select_symlinks},
    {"select-hidden",     "Select all hidden",        LIST_MENU_SELECT,
                          rfnames_select_hidden},
    {"select-invert",     "Invert selection",         LIST_MENU_SELECT,
                          rfnames_select_invert},
    {"unselect-files",    "Unselect all files",       LIST_MENU_SELECT,
                          rfnames_unselect_files},
    {"unselect-folders",  "Unselect all directories", LIST_MENU_SELECT,
                          rfnames_unselect_folders},
    {"unselect-symlinks", "Unselect all symlinks",    LIST_MENU_SELECT,
                          rfnames_unselect_symlinks},
    {"unselect-hidden",   "Unselect all hidden",      LIST_MENU_SELECT,
                          rfnames_unselect_hidden},

    {"restore-all",       "Restore all",              LIST_MENU_RESTORE,
                          rfnames_restore_all},
    {"restore-files",     "Restore all files",        LIST_MENU_RESTORE,
                          rfnames_restore_all_files},
    {"restore-folders",   "Restore all directories",  LIST_MENU_RESTORE,
                          rfnames_restore_all_folders},
    {"restore-symlinks",  "Restore all symlinks",     LIST_MENU_RESTORE,
                          rfnames_restore_all_symlinks},
    {"restore-hidden",    "Restore all hidden",       LIST_MENU_RESTORE,
                          rfnames_restore_all_hidden},

    {"remove-all",        "Remove all",               LIST_MENU_REMOVE,
                          rfnames_remove_all},
    {"remove-files",      "Remove all files",         LIST_MENU_REMOVE,
                          rfnames_remove_all_files},
    {"remove-folders",    "Remove all directories",   LIST_MENU_REMOVE,
                          rfnames_remove_all_folders},
    {"remove-symlinks",   "Remove all symlinks",      LIST_MENU_REMOVE,
                          rfnames_remove_all_symlinks},
    {"remove-hidden",     "Remove all hidden",        LIST_MENU_REMOVE,
                          rfnames_remove_all_hidden},
};
/*----------------------------------------------------------------------------*/
/**
 * @brief  List action activated, run its function.
 *
 * @param[in]     action  Activated action
 * @param[in]     param   Unused action parameter
 * @param[in,out] names   RFnames object with file list
 * @return        none
 */
static void
event_list_action (GSimpleAction *action,
                   [[maybe_unused]] GVariant *param,
                   RFnames       *names)
{
    const char *name = g_action_get_name (G_ACTION (action));

    for (size_t i = 0; i < G_N_ELEMENTS (list_actions); ++i) {
        if (strcmp (list_actions[i].name, name) == 0) {
            list_actions[i].run (names);
            break;
        }
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Add all list actions to the window.
 *
 * @param[in,out] window  Application window
 * @param[in]     names   RFnames object with file list
 * @return        none
 */
static void
add_list_actions (GtkWidget *window,
                  RFnames   *names)
{
    for (size_t i = 0; i < G_N_ELEMENTS (list_actions); ++i) {
        GSimpleAction *action = g_simple_action_new (list_actions[i].name,
                                                     nullptr);
        g_signal_connect (action, "activate",
                          G_CALLBACK (event_list_action), names);
        g_action_map_add_action (G_ACTION_MAP (window), G_ACTION (action));
        g_object_unref (action);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create GtkImage widget with desired icon/image.
 *
 * @param[in] icon Icon number
 * @return    Image widget or nullptr
 */
static GtkWidget *
create_image_widget (const IconImg icon)
{
    GtkWidget  *image   = nullptr; /* GtkImage widget to return */
    GdkTexture *texture = nullptr; /* Texture with graphics */

    if (icon < W_ICON_COUNT && (texture = get_image (icon)) != nullptr) {
        image = gtk_image_new_from_paintable (GDK_PAINTABLE (texture));
        gtk_image_set_pixel_size (GTK_IMAGE (image),
                                  gdk_texture_get_width (texture));
        g_object_unref (texture);
    }
    return image;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create radio buttons with their container.
 *
 * Buttons get the values 2, 1, 0 from top to bottom, see radio_get_value().
 * Any other current value activates the top button.
 *
 * @param[in]     labels   Button labels, from top to bottom
 * @param[in]     current  Current option value
 * @param[in]     callback Function connected to "toggled" signal
 * @param[in,out] data     RenData object passed to callback
 * @return        Grid with radio buttons
 */
static GtkWidget *
create_radio_box (const char *const  labels[static 3],
                  const int8_t       current,
                  GCallback          callback,
                  RenData           *data)
{
    GtkWidget    *grid   = gtk_grid_new ();
    GtkWidget    *first  = nullptr;
    const int8_t  active = (current == 0 || current == 1) ? current : 2;

    gtk_grid_set_column_spacing (GTK_GRID (grid), 4);

    for (int row = 0; row < 3; ++row) {
        const int8_t  value = (int8_t) (2 - row);
        GtkWidget    *radio = gtk_check_button_new_with_label (labels[row]);

        g_object_set_data (G_OBJECT (radio), RADIO_VALUE_KEY,
                           GINT_TO_POINTER ((int) value));
        if (first == nullptr) {
            first = radio;
        }
        else {
            gtk_check_button_set_group (GTK_CHECK_BUTTON (radio),
                                        GTK_CHECK_BUTTON (first));
        }
        gtk_check_button_set_active (GTK_CHECK_BUTTON (radio), value == active);
        g_signal_connect (radio, "toggled", callback, data);
        gtk_grid_attach (GTK_GRID (grid), radio, 0, row, 1, 1);
    }
    return grid;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create entry for an option text.
 *
 * @param[in]     text     Initial text
 * @param[in]     callback Function connected to "changed" signal
 * @param[in,out] data     RenData object passed to callback
 * @return        Entry widget
 */
static GtkWidget *
create_option_entry (const char *text,
                     GCallback   callback,
                     RenData    *data)
{
    GtkWidget *entry = gtk_entry_new ();

    gtk_entry_set_max_length (GTK_ENTRY (entry), FN_LEN);
    gtk_editable_set_width_chars (GTK_EDITABLE (entry), 6);
    gtk_editable_set_text (GTK_EDITABLE (entry), text);
    /* Enter activates the Rename button */
    gtk_entry_set_activates_default (GTK_ENTRY (entry), TRUE);
    g_signal_connect (entry, "changed", callback, data);

    return entry;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create spin button for an option value.
 *
 * @param[in]     value    Initial value
 * @param[in]     upper    Maximum value
 * @param[in]     tooltip  Tooltip text
 * @param[in]     callback Function connected to "value-changed" signal
 * @param[in,out] data     RenData object passed to callback
 * @return        Spin button widget
 */
static GtkWidget *
create_option_spin (const double  value,
                    const double  upper,
                    const char   *tooltip,
                    GCallback     callback,
                    RenData      *data)
{
    GtkAdjustment *adjustment = gtk_adjustment_new (value, 0.0, upper,
                                                    1.0, 5.0, 0.0);
    GtkWidget     *spin       = gtk_spin_button_new (adjustment, 1.0, 0);

    /* Enter activates the Rename button */
    gtk_spin_button_set_activates_default (GTK_SPIN_BUTTON (spin), TRUE);
    gtk_widget_set_tooltip_text (spin, tooltip);
    g_signal_connect (spin, "value-changed", callback, data);

    return spin;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Creates upcase / lowercase container.
 *
 * @param[out]    container Pointer to container
 * @param[in,out] data      RenData object with file list and settings
 * @return        none
 */
static void
create_upcase_lowercase_box (GtkWidget **container,
                             RenData    *data)
{
    static const char *const labels[] = {
        "No change", "To lowercase", "To uppercase"
    };

    *container = create_radio_box (labels, rendata_get_uplo (data),
                                   G_CALLBACK (event_case_radio_active), data);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Creates space to underscore and vice versa container.
 *
 * @param[out]    container Pointer to container
 * @param[in,out] data      RenData object with file list and settings
 * @return        none
 */
static void
create_spaces_to_underscores_box (GtkWidget **container,
                                  RenData    *data)
{
    static const char *const labels[] = {
        "No change", "Space to underscore", "Underscore to space"
    };

    *container = create_radio_box (labels, rendata_get_spaces (data),
                                   G_CALLBACK (event_spaces_radio_active),
                                   data);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Creates Apply to name/ext/both container.
 *
 * @param[out]    container Pointer to container
 * @param[in,out] data      RenData object with file list and settings
 * @return        none
 */
static void
create_apply_to_names_ext_box (GtkWidget **container,
                               RenData    *data)
{
    static const char *const labels[] = {
        "Apply to name and ext", "Apply to name", "Apply to ext"
    };

    *container = create_radio_box (labels, rendata_get_applyto (data),
                                   G_CALLBACK (event_apply_radio_active), data);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Creates replace string with other string container.
 *
 * @param[out]    container Pointer to container
 * @param[in,out] data      RenData object with file list and settings
 * @return        none
 */
static void
create_replace_str_with_str_box (GtkWidget **container,
                                 RenData    *data)
{
    GtkWidget *from_entry;    /* Replace from entry */
    GtkWidget *to_entry;      /* Replace to entry */
    GtkWidget *replace_label; /* Description label */
    GtkWidget *with_label;    /* Description label */

    from_entry = create_option_entry (
            rreplace_get_from (rendata_get_rreplace (data)),
            G_CALLBACK (event_replace_from_entry_changed), data);
    to_entry = create_option_entry (
            rreplace_get_to (rendata_get_rreplace (data)),
            G_CALLBACK (event_replace_to_entry_changed), data);

    replace_label = gtk_label_new ("Replace text");
    with_label = gtk_label_new ("with");

    *container = gtk_grid_new ();

    gtk_grid_set_column_spacing (GTK_GRID (*container), 4);

    gtk_grid_attach (GTK_GRID (*container), replace_label, 0, 0, 3, 1);
    gtk_grid_attach_next_to (GTK_GRID (*container),
            from_entry, replace_label, GTK_POS_BOTTOM, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (*container),
            with_label, from_entry, GTK_POS_RIGHT, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (*container),
            to_entry, with_label, GTK_POS_RIGHT, 1, 1);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Creates container with widgets to delete chars in file name string.
 *
 * @param[out]    container Pointer to container
 * @param[in,out] data      RenData object with file list and settings
 * @return        none
 */
static void
create_delete_chars_box (GtkWidget **container,
                         RenData    *data)
{
    GtkWidget *del_cnt;    /* Delete chars count spin button */
    GtkWidget *del_pos;    /* Delete from position spin button */
    GtkWidget *desc_label; /* Description label */

    desc_label = gtk_label_new ("Delete text:");
    del_cnt = create_option_spin (rdelete_get_cnt (rendata_get_rdelete (data)),
                                  FN_LEN, "Count",
                                  G_CALLBACK (event_delete_cnt_changed), data);
    del_pos = create_option_spin (rdelete_get_pos (rendata_get_rdelete (data)),
                                  FN_LEN, "At position",
                                  G_CALLBACK (event_delete_pos_changed), data);

    *container = gtk_grid_new ();

    gtk_grid_set_column_spacing (GTK_GRID (*container), 4);

    gtk_grid_attach (GTK_GRID (*container), desc_label, 0, 0, 2, 1);
    gtk_grid_attach_next_to (GTK_GRID (*container),
            del_cnt, desc_label, GTK_POS_BOTTOM, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (*container),
            del_pos, del_cnt, GTK_POS_RIGHT, 1, 1);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Creates container with widgets to insert string in file name.
 *
 * @param[out]    container Pointer to container
 * @param[in,out] data      RenData object with file list and settings
 * @return        none
 */
static void
create_insert_string_box (GtkWidget **container,
                          RenData    *data)
{
    GtkWidget *desc_label; /* Description label */
    GtkWidget *ins_entry;  /* Insert text entry */
    GtkWidget *ins_pos;    /* Insert text position spin button */

    desc_label = gtk_label_new ("Insert text:");
    ins_entry = create_option_entry (
            rinsovr_get_text (rendata_get_rinsert (data)),
            G_CALLBACK (event_insert_string_entry_changed), data);
    ins_pos = create_option_spin (rinsovr_get_pos (rendata_get_rinsert (data)),
                                  FN_LEN, "At position",
                                  G_CALLBACK (event_insert_pos_changed), data);

    *container = gtk_grid_new ();

    gtk_grid_set_column_spacing (GTK_GRID (*container), 4);

    gtk_grid_attach (GTK_GRID (*container), desc_label, 0, 0, 2, 1);
    gtk_grid_attach_next_to (GTK_GRID (*container),
            ins_entry, desc_label, GTK_POS_BOTTOM, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (*container),
            ins_pos, ins_entry, GTK_POS_RIGHT, 1, 1);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Creates container with widgets to overwrite string in file name.
 *
 * @param[out]    container Pointer to container
 * @param[in,out] data      RenData object with file list and settings
 * @return        none
 */
static void
create_overwrite_string_box (GtkWidget **container,
                             RenData    *data)
{
    GtkWidget *desc_label; /* Description label */
    GtkWidget *ovr_entry;  /* Insert text entry */
    GtkWidget *ovr_pos;    /* Insert text position spin button */

    desc_label = gtk_label_new ("Overwrite text:");
    ovr_entry = create_option_entry (
            rinsovr_get_text (rendata_get_roverwr (data)),
            G_CALLBACK (event_overwrite_string_entry_changed), data);
    ovr_pos = create_option_spin (rinsovr_get_pos (rendata_get_roverwr (data)),
                                  FN_LEN, "At position",
                                  G_CALLBACK (event_overwrite_pos_changed),
                                  data);

    *container = gtk_grid_new ();

    gtk_grid_set_column_spacing (GTK_GRID (*container), 4);

    gtk_grid_attach (GTK_GRID (*container), desc_label, 0, 0, 2, 1);
    gtk_grid_attach_next_to (GTK_GRID (*container),
            ovr_entry, desc_label, GTK_POS_BOTTOM, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (*container),
            ovr_pos, ovr_entry, GTK_POS_RIGHT, 1, 1);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Creates container with widgets to number file names.
 *
 * @param[out]    container Pointer to container
 * @param[in,out] data      RenData object with file list and settings
 * @return        none
 */
static void
create_number_string_box (GtkWidget **container,
                          RenData    *data)
{
    GtkWidget *check;          /* Number names checkbox */
    GtkWidget *num_start_spin; /* Numbering start spin button */
    GtkWidget *num_pos_spin;   /* Numbering position spin button */

    check = gtk_check_button_new_with_label ("Number files");
    gtk_check_button_set_active (GTK_CHECK_BUTTON (check),
            rnumber_get_opt (rendata_get_rnumber (data)) != 0);

    num_start_spin = create_option_spin (
            rnumber_get_start (rendata_get_rnumber (data)),
            1000, "Start numbering from",
            G_CALLBACK (event_number_start_changed), data);
    num_pos_spin = create_option_spin (
            rnumber_get_pos (rendata_get_rnumber (data)),
            FN_LEN, "Number posistion in name",
            G_CALLBACK (event_number_pos_changed), data);

    g_signal_connect (G_OBJECT (check), "toggled",
                      G_CALLBACK (event_toggle_number_names), data);

    *container = gtk_grid_new ();

    gtk_grid_set_column_spacing (GTK_GRID (*container), 4);

    gtk_grid_attach (GTK_GRID (*container), check, 0, 0, 2, 1);
    gtk_grid_attach_next_to (GTK_GRID (*container),
            num_start_spin, check, GTK_POS_BOTTOM, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (*container),
            num_pos_spin, num_start_spin, GTK_POS_RIGHT, 1, 1);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Creates Rename - Close - Exit after rename container.
 *
 * @param[out]    container Pointer to container
 * @param[out]    rename    Pointer to Rename button, the window default
 * @param[in,out] data      RenData object with file list and settings
 * @return        none
 */
static void
create_rename_close_exit_box (GtkWidget **container,
                              GtkWidget **rename,
                              RenData    *data)
{
    GtkWidget *ok_button;     /* Rename button */
    GtkWidget *cancel_button; /* Close button */
    GtkWidget *renexit;       /* Exit after rename check button */
    GtkWidget *remember;      /* Remember options on exit check button */

    ok_button = gtk_button_new_with_label ("Rename");
    cancel_button = gtk_button_new_with_label ("Close");
    *rename = ok_button;

    g_signal_connect (G_OBJECT (ok_button), "clicked",
                      G_CALLBACK (event_click_rename), data);
    g_signal_connect (G_OBJECT (cancel_button), "clicked",
                      G_CALLBACK (event_close), nullptr);

    renexit = gtk_check_button_new_with_label ("Exit after rename");

    gtk_check_button_set_active (GTK_CHECK_BUTTON (renexit),
            rendata_get_renexit (data));

    g_signal_connect (G_OBJECT (renexit), "toggled",
                      G_CALLBACK (event_toggle_rename_exit), data);

    remember = gtk_check_button_new_with_label ("Remember options on exit");

    gtk_check_button_set_active (GTK_CHECK_BUTTON (remember),
            rendata_get_rememberopt (data));

    g_signal_connect (G_OBJECT (remember), "toggled",
                      G_CALLBACK (event_toggle_remember_options), data);

    *container = gtk_grid_new ();

    gtk_grid_set_column_spacing (GTK_GRID (*container), 4);

    gtk_grid_attach (GTK_GRID (*container), ok_button, 0, 0, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (*container),
            cancel_button, ok_button, GTK_POS_RIGHT, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (*container),
            renexit, cancel_button, GTK_POS_RIGHT, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (*container),
            remember, renexit, GTK_POS_RIGHT, 1, 1);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create flat toolbar button showing an icon.
 *
 * The label is the button's accessible name, and is shown only if the icon
 * can not be loaded.
 *
 * @param[in] icon    Icon number
 * @param[in] label   Button label
 * @param[in] tooltip Button tooltip
 * @return    Button widget
 */
static GtkWidget *
create_tool_button (const IconImg  icon,
                    const char    *label,
                    const char    *tooltip)
{
    GtkWidget *button = gtk_button_new ();
    GtkWidget *image  = create_image_widget (icon);

    if (image != nullptr) {
        gtk_button_set_child (GTK_BUTTON (button), image);
        gtk_accessible_update_property (GTK_ACCESSIBLE (button),
                                        GTK_ACCESSIBLE_PROPERTY_LABEL, label,
                                        -1);
    }
    else {
        gtk_button_set_label (GTK_BUTTON (button), label);
    }
    gtk_button_set_has_frame (GTK_BUTTON (button), FALSE);
    gtk_widget_set_tooltip_text (button, tooltip);

    return button;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Join a button and a menu arrow with a popover into a split button.
 *
 * @param[in] button  Main button
 * @param[in] popover Popover opened by the arrow
 * @return    Box with both buttons
 */
static GtkWidget *
create_split_button (GtkWidget *button,
                     GtkWidget *popover)
{
    GtkWidget *box         = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
    GtkWidget *menu_button = gtk_menu_button_new ();

    gtk_widget_add_css_class (box, "linked");
    gtk_menu_button_set_has_frame (GTK_MENU_BUTTON (menu_button), FALSE);
    gtk_menu_button_set_popover (GTK_MENU_BUTTON (menu_button), popover);
    gtk_box_append (GTK_BOX (box), button);
    gtk_box_append (GTK_BOX (box), menu_button);

    return box;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create popover menu with the list actions of one toolbar menu.
 *
 * @param[in] list_menu Menu to create
 * @return    Popover widget
 */
static GtkWidget *
create_list_popover (const ListMenu list_menu)
{
    GMenu     *menu = g_menu_new ();
    GtkWidget *popover;

    for (size_t i = 0; i < G_N_ELEMENTS (list_actions); ++i) {
        if (list_actions[i].menu == list_menu) {
            g_autofree char *action = g_strconcat ("win.", list_actions[i].name,
                                                   nullptr);
            g_menu_append (menu, list_actions[i].label, action);
        }
    }
    popover = gtk_popover_menu_new_from_model (G_MENU_MODEL (menu));
    g_object_unref (menu);

    return popover;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create popover of the ADD button.
 *
 * Holds the button adding files from a folder and the options it uses. The
 * popover stays open while the options are changed.
 *
 * @param[in,out] data RenData object with settings
 * @return        Popover widget
 */
static GtkWidget *
create_add_popover (RenData *data)
{
    static const struct {
        const char *label;
        int         flag;
    } options[] = {
        {"Select files",                      FOLDER_SELECT_FILES},
        {"Select directories",                FOLDER_SELECT_FOLDERS},
        {"Select symlinks",                   FOLDER_SELECT_SYMLINKS},
        {"Select hidden files / directories", FOLDER_SELECT_HIDDEN},
        {"Scan folders recursively",          FOLDER_SCAN_RECURSIVELY},
    };
    GtkWidget *popover   = gtk_popover_new ();
    GtkWidget *box       = gtk_box_new (GTK_ORIENTATION_VERTICAL, 4);
    GtkWidget *button    = gtk_button_new ();
    GtkWidget *button_in = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 6);
    GtkWidget *image     = create_image_widget (W_ICON_ADD_DIR);

    if (rendata_get_dirsel (data) < 1)
        rendata_set_dirsel (data, FOLDER_SELECT_FILES);

    if (image != nullptr)
        gtk_box_append (GTK_BOX (button_in), image);

    gtk_box_append (GTK_BOX (button_in),
                    gtk_label_new ("Add files from directory"));
    gtk_button_set_child (GTK_BUTTON (button), button_in);
    gtk_button_set_has_frame (GTK_BUTTON (button), FALSE);
    g_signal_connect (button, "clicked",
                      G_CALLBACK (event_click_add_folder_files), data);

    gtk_box_append (GTK_BOX (box), button);
    gtk_box_append (GTK_BOX (box),
                    gtk_separator_new (GTK_ORIENTATION_HORIZONTAL));

    for (size_t i = 0; i < G_N_ELEMENTS (options); ++i) {
        GtkWidget *check = gtk_check_button_new_with_label (options[i].label);

        g_object_set_data (G_OBJECT (check), DIRSEL_FLAG_KEY,
                           GINT_TO_POINTER (options[i].flag));
        gtk_check_button_set_active (GTK_CHECK_BUTTON (check),
                (rendata_get_dirsel (data) & options[i].flag) != 0);
        g_signal_connect (check, "toggled",
                          G_CALLBACK (event_toggle_dirsel), data);
        gtk_box_append (GTK_BOX (box), check);
    }
    gtk_popover_set_child (GTK_POPOVER (popover), box);

    return popover;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Creates toolbar with add, remove, sort buttons.
 *
 * @param[out]    container Pointer to container
 * @param[in,out] data      RenData object with file list and settings
 * @return        none
 */
static void
create_toolbar (GtkWidget **container,
                RenData    *data)
{
    GtkWidget *button;

    *container = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 2);

    /* ADD button with popover */
    button = create_tool_button (W_ICON_ADD, "ADD", "Add files");
    g_signal_connect (button, "clicked",
                      G_CALLBACK (event_click_add_files), data);
    gtk_box_append (GTK_BOX (*container),
                    create_split_button (button, create_add_popover (data)));

    /* SEL button with menu */
    button = create_tool_button (W_ICON_SELECT, "SEL", "Select / unselect all");
    gtk_actionable_set_action_name (GTK_ACTIONABLE (button),
                                    "win.select-toggle");
    gtk_box_append (GTK_BOX (*container),
                    create_split_button (button,
                            create_list_popover (LIST_MENU_SELECT)));

    /* RES button with menu */
    button = create_tool_button (W_ICON_REVERT, "RES",
                                 "Restore original names for selected items");
    gtk_actionable_set_action_name (GTK_ACTIONABLE (button),
                                    "win.restore-selected");
    gtk_box_append (GTK_BOX (*container),
                    create_split_button (button,
                            create_list_popover (LIST_MENU_RESTORE)));

    /* DEL button with menu */
    button = create_tool_button (W_ICON_REMOVE, "DEL",
                                 "Remove selected items from list");
    gtk_actionable_set_action_name (GTK_ACTIONABLE (button),
                                    "win.remove-selected");
    gtk_box_append (GTK_BOX (*container),
                    create_split_button (button,
                            create_list_popover (LIST_MENU_REMOVE)));

    /* SRT button */
    button = create_tool_button (W_ICON_SORT, "SRT", "Sort names");
    gtk_actionable_set_action_name (GTK_ACTIONABLE (button), "win.sort");
    gtk_box_append (GTK_BOX (*container), button);

    gtk_box_append (GTK_BOX (*container),
                    gtk_separator_new (GTK_ORIENTATION_VERTICAL));

    button = create_tool_button (W_ICON_INFO, "( i )", "Application info");
    g_signal_connect (button, "clicked",
                      G_CALLBACK (event_click_about), nullptr);
    gtk_box_append (GTK_BOX (*container), button);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Creates file entry box, file name entries, set r_files properties.
 *
 * Searches for files in command line passed values, creates file name entries,
 * strings for buffering original and changed file names.
 * Creates container and puts entries in it.
 *
 * @param[in]     files        An array of GFiles
 * @param[in]     file_count   Length of files
 * @param[in,out] data         RenData object with file list and settings
 * @param[out]    container    Pointer to entry box container pointer
 * @return        none
 */
static void
create_file_name_entries (GFile         **files,
                          uint_fast32_t   file_count,
                          RenData        *data,
                          GtkWidget     **container)
{
    GtkWidget *list_view; /* List with a row for each visible file */

    list_view = rfnames_create_view (data->names);

    for (uint_fast32_t i = 0; i < file_count; ++i) {
        if (g_file_query_exists (files[i], nullptr)) {
            rfnames_add_gfile (data->names, files[i]);
        }
    }
    /* Make scrollbars */
    *container = gtk_scrolled_window_new ();
    gtk_widget_set_vexpand (*container, TRUE);
    gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (*container), list_view);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Creates main window, its properties and connects window signals.
 *
 * @param[out]    window       Pointer to a app window container
 * @param[in,out] application  GtkApplication item
 * @param[in,out] data         RenData object with file list and settings
 * @return        none
 */
static void
create_window (GtkWidget        **window,
               GtkApplication    *application,
               RenData           *data)
{
    GtkEventController *shortcuts;

    /* Create window widget */
    *window = gtk_application_window_new (application);

    /* Set window properties */
    gtk_window_set_title (GTK_WINDOW (*window), APP_NAME " v" APP_VER);
    gtk_window_set_default_size (GTK_WINDOW (*window), WIN_WIDTH, WIN_HEIGHT);

    /* Icon installed with the application */
    gtk_window_set_default_icon_name ("sfrename");

    add_list_actions (*window, data->names);

    /* Close window with Escape */
    shortcuts = gtk_shortcut_controller_new ();
    gtk_shortcut_controller_add_shortcut (
            GTK_SHORTCUT_CONTROLLER (shortcuts),
            gtk_shortcut_new (
                    gtk_keyval_trigger_new (GDK_KEY_Escape, 0),
                    gtk_callback_action_new (event_win_escape,
                                             nullptr, nullptr)));
    gtk_widget_add_controller (*window, shortcuts);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create horizontal separator with vertical margins.
 *
 * @return  Separator widget
 */
static GtkWidget *
create_separator (void)
{
    GtkWidget *sep = gtk_separator_new (GTK_ORIENTATION_HORIZONTAL);

    gtk_widget_set_margin_top (sep, 4);
    gtk_widget_set_margin_bottom (sep, 4);

    return sep;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Application shutdown signal.
 *
 * @param[in]  application  GtkApplication item
 * @param[out] data         RenData object with file list and settings
 * @return     none
 */
static void
shutdown ([[maybe_unused]] GtkApplication *application,
          RenData        *data)
{
    rconfig_save (data);
    rendata_free (data);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Open files in application signal.
 *
 * @param[in,out] application GtkApplication item
 * @param[in,out] files       An array of GFiles to open
 * @param[in]     file_count  The length of the files array
 * @param[in]     hint        A hint (or ""), but never nullptr
 * @param[in,out] data        RenData object with file list and settings
 * @return        none
 */
static void
open (GtkApplication  *application,
      GFile          **files,
      int              file_count,
      [[maybe_unused]] const char *hint,
      RenData         *data)
{
    GtkWidget *window;          /* Appliation window */
    GtkWidget *toolbar;         /* Top window tool bar */
    GtkWidget *entry_box;       /* File name list widget */
    GtkWidget *vbox;            /* Main window pack widget */
    GtkWidget *uplc_box;        /* Upcase / lowercase widget */
    GtkWidget *undersc_box;     /* Spaces to underscores widget */
    GtkWidget *replace_str_box; /* Replace text widget */
    GtkWidget *del_str_box;     /* Delete text widget */
    GtkWidget *ins_str_box;     /* Insert text widget */
    GtkWidget *ovr_str_box;     /* Overwrite text widget */
    GtkWidget *number_box;      /* Number names widget */
    GtkWidget *apply_box;       /* Apply to names / extenstions widget */
    GtkWidget *udusc_box;       /* Widget to pack all settings */
    GtkWidget *okcl_box;        /* Rename / Close widget */
    GtkWidget *rename_button;   /* Button activated by Enter */
    GtkWidget *sep;             /* Separator widget */

    /* Create file name entries and set r_files properties */
    create_file_name_entries (files , (uint_fast32_t) file_count, data,
                              &entry_box);
    /* Create main window and set properties */
    create_window (&window, application, data);
    create_toolbar (&toolbar, data);
    create_upcase_lowercase_box (&uplc_box, data);
    create_spaces_to_underscores_box (&undersc_box, data);
    create_apply_to_names_ext_box (&apply_box, data);
    create_replace_str_with_str_box (&replace_str_box, data);
    create_delete_chars_box (&del_str_box, data);
    create_insert_string_box (&ins_str_box, data);
    create_overwrite_string_box (&ovr_str_box, data);
    create_number_string_box (&number_box, data);

    /* OK, Close box */
    create_rename_close_exit_box (&okcl_box, &rename_button, data);

    /* Box for up down, underscore and apply to boxes */
    udusc_box = gtk_grid_new ();

    gtk_grid_set_column_spacing (GTK_GRID (udusc_box), 4);
    gtk_grid_set_row_spacing (GTK_GRID (udusc_box), 4);

    gtk_grid_attach (GTK_GRID (udusc_box), apply_box, 0, 0, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (udusc_box),
            uplc_box, apply_box, GTK_POS_RIGHT, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (udusc_box),
            undersc_box, uplc_box, GTK_POS_RIGHT, 1, 1);

    sep = gtk_separator_new (GTK_ORIENTATION_HORIZONTAL);

    gtk_grid_attach (GTK_GRID (udusc_box), sep, 0, 1, 3, 1);

    gtk_grid_attach (GTK_GRID (udusc_box), replace_str_box, 0, 2, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (udusc_box),
            ins_str_box, replace_str_box, GTK_POS_RIGHT, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (udusc_box),
            del_str_box, ins_str_box, GTK_POS_RIGHT, 1, 1);

    sep = gtk_separator_new (GTK_ORIENTATION_HORIZONTAL);

    gtk_grid_attach (GTK_GRID (udusc_box), sep, 0, 3, 3, 1);

    gtk_grid_attach (GTK_GRID (udusc_box), ovr_str_box, 1, 4, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (udusc_box),
            number_box, ovr_str_box, GTK_POS_RIGHT, 1, 1);

    /* Main application widgets box */
    vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 4);

    gtk_widget_set_margin_start (vbox, 10);
    gtk_widget_set_margin_end (vbox, 10);
    gtk_widget_set_margin_top (vbox, 10);
    gtk_widget_set_margin_bottom (vbox, 10);

    gtk_box_append (GTK_BOX (vbox), toolbar);
    gtk_box_append (GTK_BOX (vbox), gtk_label_new ("New file / directory name"));
    gtk_box_append (GTK_BOX (vbox), create_separator ());
    gtk_box_append (GTK_BOX (vbox), entry_box);
    gtk_box_append (GTK_BOX (vbox), create_separator ());
    gtk_box_append (GTK_BOX (vbox), udusc_box);
    gtk_box_append (GTK_BOX (vbox), create_separator ());
    gtk_box_append (GTK_BOX (vbox), okcl_box);

    gtk_window_set_child (GTK_WINDOW (window), vbox);
    gtk_window_set_default_widget (GTK_WINDOW (window), rename_button);

    /* Apply settings (possibly loaded from config) to file name entries */
    file_names_update_changes (data);

    gtk_window_present (GTK_WINDOW (window));

    /* Set focus on first entry if file count is greater than 0 */
    focus_first_added (data->names, 0);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Application activate signal.
 *
 * @param[in,out] application  GtkApplication item
 * @param[in,out] data         RenData object with file list and settings
 * @return        none
 */
static void
activate (GtkApplication *application,
          RenData        *data)
{
    open (application, nullptr, 0, nullptr, data);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief Main function.
 *
 * @param[in] argc Arguments passed to the program from the environment in which
 *                 the program is run
 * @param[in] argv Pointer to the first element of an array of pointers that
 *                 represent the arguments passed to the program
 * @return         Return value
 */
int
main (int    argc,
      char **argv)
{
    GtkApplication *app;
    RenData        *data;
    int             status;

    data = rendata_new ();
    rconfig_load (data);

    /* Non-unique: every run has its own file list and settings. A unique
     * application would hand new files to the running process, which has one
     * RenData shared by all of its windows. */
    app = gtk_application_new ("org.nongnu.SmallFileRenamer",
                               G_APPLICATION_HANDLES_OPEN |
                               G_APPLICATION_NON_UNIQUE);

    g_signal_connect (app, "shutdown", G_CALLBACK (shutdown), data);
    g_signal_connect (app, "activate", G_CALLBACK (activate), data);
    g_signal_connect (app, "open",     G_CALLBACK (open),     data);

    g_set_application_name (APP_NAME);

    status = g_application_run (G_APPLICATION (app), argc, argv);

    g_object_unref (app);

    return status;
}
/*----------------------------------------------------------------------------*/

