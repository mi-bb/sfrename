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
 * @date November 15, 2020
 *
 * @version 1.3.0
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <ctype.h>
#include <inttypes.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include <gio/gio.h>
#include <gdk/gdkkeysyms.h>
#include <unistd.h>
#include "dialogs.h"
#include "string_functions.h"
#include "rendata.h"
#include "rconfig.h"
#include "namefn.h"
#include "images.h"
#include "defs.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if file can be renamed and rename it.
 *
 * @param[in] old_name Old file name
 * @param[in] new_name New file name
 * @param[in] path     Path to directory where the file is located
 * @return    Renaming status
 */
static RenResult
file_check_and_rename (const char *old_name,
                       const char *new_name,
                       const char *path)
{
    char      *new_path = nullptr;           /* Full path for new file */
    char      *old_path = nullptr;           /* Full path for old file */
    RenResult  result   = REN_OK;            /* Renaming result */
    size_t     dir_len  = strlen (path);     /* Length of file dir path */
    size_t     old_len  = strlen (old_name); /* Length of old file name */
    size_t     new_len  = strlen (new_name); /* Length of new file name */

    /* Check if new file name is different than old */
    if (old_len == new_len && strcmp (old_name, new_name) == 0)
        return REN_NC; /* No chgange */

    /* Alloc memory for full paths of old and new file name */
    new_path = malloc ((dir_len + new_len + 1) * sizeof (char));
    old_path = malloc ((dir_len + old_len + 1) * sizeof (char));

    if (new_path == nullptr || old_path == nullptr) {
        fputs ("Alloc error\n", stderr);
        exit (EXIT_FAILURE);
    }
    memcpy (new_path, path, dir_len);
    memcpy (old_path, path, dir_len);
    memcpy (new_path + dir_len, new_name, new_len);
    memcpy (old_path + dir_len, old_name, old_len);
    new_path [dir_len + new_len] = '\0';
    old_path [dir_len + old_len] = '\0';

    /* Check if file with new file name don't exist */
    if (access (new_path, F_OK) != 0) {
        result = rename (old_path, new_path) == 0 ? 
                REN_OK :     /* OK */
                REN_NOT_REN; /* Could not rename */
    }
    else {
        result = REN_EXISTS;  /* File exists */
    }
    free (new_path);
    free (old_path);

    return result;
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
    RFitem        *item;  /* RFitem object to process */
    uint_fast32_t  i = 0; /* i */

    i = rfnames_get_cnt (rendata_get_rfnames (data));

    /* Check if file is checked and skip to next one if it is not */
    while (i--) {
        item = data->names->items[i];

        if (!rfitem_get_checked (item))
            continue;

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
        rfitem_entry_check_and_update (item, rfitem_get_new_name (item));
    }
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
    GError          *g_err;
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
    do {
        g_file_enumerator_iterate (f_enum, &f_info, &file, nullptr, &g_err);
        if (f_info != nullptr) {
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
                    rfnames_add_gfile_to_file_box (names, file);
                }
                if (options & FOLDER_SCAN_RECURSIVELY) {
                    enumerate_folder (names, file, options);
                }
            }
            if (f_type == G_FILE_TYPE_REGULAR &&
                (options & FOLDER_SELECT_FILES)) {
                rfnames_add_gfile_to_file_box (names, file);
            }
        }
    }
    while (f_info != nullptr);

    g_object_unref (f_enum);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Checks if path is a folder and calls enumerate_folder if it is.
 *
 * @param[out] names    RFnames item with file list
 * @param[in]  dir_path Directory path
 * @param[in]  options  Options with info which items to add (files, folders)
 * @return     none
 */
static void
get_folder_content (RFnames    *names,
                    const char *dir_path,
                    const int   options)
{
    GFile     *dir;
    GError    *g_err;
    GFileInfo *f_info;
    GFileType  f_type;

    dir = g_file_new_for_path (dir_path);
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
    g_object_unref (dir);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get active RadioButton in a group of RadioButton items.
 *
 * @param[in] radiob  RadioButton to check
 * @return    Returns Active Radiobutton index or -1 if something went wrong
 * @retval    -1      Something went wrong
 * @retval    >=0     Active RadioButton index
 */
static int8_t
get_radio_active (GtkRadioButton *radiob)
{
    int8_t                 i = -1;      /* active RadioButton index */
    GtkRadioButton *tmp_butt = nullptr; /* temp RadioButton */
    GSList         *tmp_list = nullptr; /* get RadioButton group list */

    tmp_list = gtk_radio_button_get_group (radiob);

    while (tmp_list != nullptr) {
        ++i;
        tmp_butt = tmp_list->data; /* get current list value (RadioButton) */
        tmp_list = tmp_list->next;

        if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (tmp_butt)))
            break;
    }
    tmp_butt = nullptr; /* nulling temp button */
    return i;
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
    GtkWidget *gw_toplevel = gtk_widget_get_toplevel (widget);
    gtk_widget_destroy (gw_toplevel);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Rename button clicked.
 *
 * Reads value of each Entry with file name and tries to rename file.
 * If File can not be renamed it prints the reason.
 * If exit after rename is selected, application is closed after rename.
 *
 * @param[in]     widget  Button which received the signal
 * @param[in,out] data RenData object with file list and settings
 * @return     none
 */
static void
event_click_rename (GtkWidget *widget,
                    RenData   *data)
{
    const char    *old_name      = nullptr; /* Old file name */
    const char    *new_name      = nullptr; /* New file name */
    const char    *path          = nullptr; /* Path to file's directory */
    RenResult      result        = REN_OK;  /* Renaming result */
    uint_fast32_t  renamed_count = 0;       /* Number of renamed files */
    uint_fast32_t  names_count   = 0;       /* Number of files to rename */
    RFitem        *item;                    /* Processed RFitem object */

    names_count = rfnames_get_cnt (rendata_get_rfnames (data));

    for (uint_fast32_t i = 0; i < names_count; ++i) {
        item = data->names->items[i];

        new_name = rfitem_entry_get_text (item);
        old_name = rfitem_get_org_name (item);
        path = rfitem_get_path (item);

        result = file_check_and_rename (old_name, new_name, path);

        switch (result) {

            case REN_OK:
                printf ("File: %s renamed to: %s\n", old_name, new_name);
                /* copy new name to original in buffer */
                rfitem_set_org_name_from_new_name (item);
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
        }
        if (result != REN_OK && result != REN_NC) {
            /* Revert old file names to new */
            rfitem_set_new_name_from_org_name (item);

            /* Update file name in entry */
            rfitem_entry_check_and_update (item, old_name);
        }
    }
    printf ("Renamed %" PRIdFAST32 " files of %" PRIdFAST32 "\n",
            renamed_count, names_count);

    /* exit application if "Exit after rename" checkbox was selected */
    if (rendata_get_renexit (data))
        event_close (widget, nullptr);
    else
        file_names_update_changes (data);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Insert string text position in SpinButton changed.
 *
 * Copies SpinButton value to RFiles and calls to update file name changes if
 * insert string text is not empty.
 *
 * @param[in]  sp_button SpinButton which received the signal
 * @param[out] data   RenData object with file list and settings
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
 * @param[out] data RenData object with file list and settings
 * @return     none
 */
static void
event_insert_string_entry_changed (GtkWidget *widget,
                                   RenData   *data)
{
    const char *text = gtk_entry_get_text (GTK_ENTRY (widget));

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
 * @param[out] data   RenData object with file list and settings
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
 * @param[out] data RenData object with file list and settings
 * @return     none
 */
static void
event_overwrite_string_entry_changed (GtkWidget *widget,
                                      RenData   *data)
{
    const char *text = gtk_entry_get_text (GTK_ENTRY (widget));

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
 * @param[out] data   RenData object with file list and settings
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
 * @param[out] data   RenData object with file list and settings
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
 * @brief  Upper/lower case RadioButton changed.
 *
 * Reads which RadioButton is selected and runs file name update changes
 * function.
 *
 * @param[in]  radiob  RadioButton which received the signal
 * @param[out] data RenData object with file list and settings
 * @return     none
 */
static void
event_case_radio_active (GtkRadioButton *radiob,
                         RenData        *data)
{
    static int8_t dbl = 0; /* to remove double toggling */

    if (dbl ^= 1) {
        rendata_set_uplo (data, get_radio_active (radiob));
        file_names_update_changes (data);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Space/underscore replace RadioButton changed.
 *
 * Reads which RadioButton is selected and runs file name update changes
 * function.
 *
 * @param[in]  radiob  RadioButton which received the signal
 * @param[out] data RenData object with file list and settings
 * @return     none
 */
static void
event_spaces_radio_active (GtkRadioButton *radiob,
                           RenData        *data)
{
    static int8_t dbl = 0; /* to remove double toggling */

    if (dbl ^= 1) {
        rendata_set_spaces (data, get_radio_active (radiob));
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
 * @param[out] data RenData object with file list and settings
 * @return     none
 */
static void
event_replace_from_entry_changed (GtkWidget *widget,
                                  RenData   *data)
{
    const char *text = gtk_entry_get_text (GTK_ENTRY (widget));

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
 * @param[out] data RenData object with file list and settings
 * @return     none
 */
static void
event_replace_to_entry_changed (GtkWidget *widget,
                                RenData   *data)
{
    const char *text = gtk_entry_get_text (GTK_ENTRY (widget));

    rreplace_set_to (data->replace, text);

    if (!rreplace_empty_from (rendata_get_rreplace (data)))
        file_names_update_changes (data);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Changed state of RadioButton responsible for applying changes to
 *         file name, file extension or both.
 *
 * Reads which RadioButton is selected, renames files in file name buffer,
 * checks for strings replacement and updates entries if needed.
 *
 * @param[in]  radiob  RadioButton which received the signal
 * @param[out] data RenData object with file list and settings
 * @return     none
 */
static void
event_apply_radio_active (GtkRadioButton *radiob,
                          RenData        *data)
{
    static int8_t dbl = 0; /* to remove double toggling */

    if (dbl ^= 1) {
        /* read apply to names/ext active RadioButton */
        rendata_set_applyto (data, get_radio_active (radiob));

        file_names_update_changes (data);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Number names CheckBox state changed.
 *
 * @param[in]  toggleb ToggleButton which received the signal
 * @param[out] data RenData object with file list and settings
 * @return     none
 */
static void
event_toggle_number_names (GtkToggleButton *toggleb,
                           RenData         *data)
{
    rnumber_set_opt (data->number,
            (int8_t) gtk_toggle_button_get_active (toggleb));

    file_names_update_changes (data);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Numbering strings start value in SpinButton changed.
 *
 * @param[in]  sp_button SpinButton which received the signal
 * @param[out] data   RenData object with file list and settings
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
 * @param[out] data   RenData object with file list and settings
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
 * @param[in]  toggleb ToggleButton which received the signal
 * @param[out] data RenData object with file list and settings
 * @return     none
 */
static void
event_toggle_rename_exit (GtkToggleButton *toggleb,
                          RenData         *data)
{
    rendata_set_renexit (data,
            gtk_toggle_button_get_active (toggleb));
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Remember options on exit CheckBox state changed.
 *
 * Sets rememberopt value in r_files structure based on the value of
 * ChackBox.
 *
 * @param[in]  toggleb ToggleButton which received the signal
 * @param[out] data RenData object with file list and settings
 * @return     none
 */
static void
event_toggle_remember_options (GtkToggleButton *toggleb,
                               RenData         *data)
{
    rendata_set_rememberopt (data,
            gtk_toggle_button_get_active (toggleb));
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  React to key pressed in main window.
 *
 * @param[in]     widget    The object which received the signal
 * @param[in]     event     The event which triggered this signal
 * @param[in,out] data   RenData object with file list and settings
 */
static gboolean
event_win_key_press (GtkWidget   *widget,
                     GdkEventKey *event,
                     RenData     *data)
{
    /* Catch Esc key in main window and exit */
    if (event->keyval == GDK_KEY_Escape)
        event_close (widget, nullptr);

    /* Catch Enter and preform rename */
    if (event->keyval == GDK_KEY_Return) {
        event_click_rename (widget, data);
    }
    return FALSE;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Add files button pressed.
 *
 * @param[in,out] names RFnames object with file list
 * @return        none
 */
static void
event_click_add_files (RFnames *names)
{
    GSList        *gs_files   = nullptr; /* File list returned by dialog */
    GSList        *gs_fn      = nullptr; /* File list copy */
    uint_fast32_t  prev_count = 0;       /* Previous number of file items */

    prev_count  = rfnames_get_cnt (names);
    gs_files = add_files_dialog (nullptr);
    gs_fn    = gs_files;

    while (gs_fn) {
        const char *file_path = gs_fn->data;
        rfnames_add_sfile_to_file_box (names, file_path);
        gs_fn = gs_fn->next;
    }
    g_slist_free_full (gs_files, g_free);

    /* Select first entry after adding to empty list */
    if (prev_count == 0 && rfnames_get_cnt (names) > 0) {
        gtk_widget_grab_focus (names->items[0]->entry);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Add files from folder button pressed.
 *
 * @param[in,out] data RenData object with settings
 * @return        none
 */
static void
event_click_add_folder_files (RenData *data)
{
    int     options    = 0;       /* Options for select files from directory */
    size_t  prev_count = 0;       /* Previous number of file items on list */
    char   *folder     = nullptr; /* Folder path */

    prev_count = rfnames_get_cnt (rendata_get_rfnames (data));
    options   = rendata_get_dirsel (data);
    options   = options < 1 ? 1 : options;
    folder   = add_files_folder_dialog (nullptr, &options);

    if (folder != nullptr) {
        if (options > 0) {
            get_folder_content (data->names, folder, options);
            rendata_set_dirsel (data, (int8_t) options);
        }
        free (folder);
    }
    gtk_widget_show_all (data->names->file_box);

    /* Select first entry after adding to empty list */
    if (prev_count == 0 && rfnames_get_cnt (rendata_get_rfnames (data)) > 0) {
        gtk_widget_grab_focus (data->names->items[0]->entry);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create GtkImage widget with desired icon/image.
 *
 * @param[in] icon Icon number
 * @return    Imge widget
 */
static GtkWidget *
create_image_widget (const IconImg icon)
{
    GtkWidget *gw_img = nullptr; /* GtkImage widget to return */
    GdkPixbuf *gd_pix = nullptr; /* Pigbuf to load graphics */

    if (icon < W_ICON_COUNT && (gd_pix = get_image (icon)) != nullptr) {
        gw_img = gtk_image_new_from_pixbuf (gd_pix);
        g_object_unref (gd_pix);
    }
    return gw_img;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create GtkMenuItem widget with image, text, and tooltip.
 *
 * @param[in] label Menu entry label
 * @param[in] hint  Menu entry tooltip
 * @param[in] icon  Icon number
 * @return    Result menu entry widget
 */
static GtkWidget *
create_img_menu_item (const char    *label,
                      const char    *hint,
                      const IconImg  icon)
{
    GtkWidget *gw_item; /* GtkMenuItem widget to return */
    GtkWidget *gw_box;  /* Box to pack image and label for menu item */
    GtkWidget *gw_img;  /* GtkImage widget for graphics */

    gw_item = gtk_menu_item_new ();
    gw_box  = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 2);

    if ((gw_img = create_image_widget (icon)) != nullptr) {
        gtk_box_pack_start (GTK_BOX (gw_box), gw_img, FALSE, FALSE, 4);
    }
    if (label != nullptr && label[0] != '\0') {
        GtkWidget *gw_lab = gtk_label_new (label);
        /*gtk_container_add (GTK_CONTAINER (gw_box), gw_lab);*/
        gtk_box_pack_start (GTK_BOX (gw_box), gw_lab, FALSE, FALSE, 4);
    }
    if (hint != nullptr && hint[0] != '\0') {
        gtk_widget_set_tooltip_text (gw_item, hint);
    }
    gtk_container_add (GTK_CONTAINER (gw_item), gw_box);
    return gw_item;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Creates upcase / lowercase container.
 *
 * @param[out]    gw_container Pointer to container
 * @param[in,out] data      RenData object with file list and settings
 * @return        none
 */
static void
create_upcase_lowercase_box (GtkWidget **gw_container,
                             RenData    *data)
{
    GtkWidget *gw_ncc; /* No change radio button */
    GtkWidget *gw_lcc; /* Lovercase radio button */
    GtkWidget *gw_upc; /* Uppercase radio button */

    gw_ncc = gtk_radio_button_new_with_label (nullptr, "No change");
    gw_lcc = gtk_radio_button_new_with_label_from_widget (
             GTK_RADIO_BUTTON (gw_ncc), "To lowercase");
    gw_upc = gtk_radio_button_new_with_label_from_widget (
             GTK_RADIO_BUTTON (gw_ncc), "To uppercase");

    switch (rendata_get_uplo (data)) {
    case 0:  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gw_upc), TRUE); break;
    case 1:  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gw_lcc), TRUE); break;
    default: gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gw_ncc), TRUE); break;
    }

    g_signal_connect (G_OBJECT(gw_ncc), "toggled",
                      G_CALLBACK (event_case_radio_active), data);
    g_signal_connect (G_OBJECT (gw_lcc), "toggled",
                      G_CALLBACK (event_case_radio_active), data);
    g_signal_connect (G_OBJECT (gw_upc), "toggled",
                      G_CALLBACK (event_case_radio_active), data);

    /* To uppercase lowercase box */
    *gw_container = gtk_grid_new ();

    gtk_grid_set_column_spacing (GTK_GRID (*gw_container), 4);

    gtk_grid_attach (GTK_GRID (*gw_container), gw_ncc, 0, 0, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (*gw_container),
            gw_lcc, gw_ncc, GTK_POS_BOTTOM, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (*gw_container),
            gw_upc, gw_lcc, GTK_POS_BOTTOM, 1, 1);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Creates space to underscore and vice versa container.
 *
 * @param[out]    gw_container Pointer to container
 * @param[in,out] data      RenData object with file list and settings
 * @return        none
 */
static void
create_spaces_to_underscores_box (GtkWidget **gw_container,
                                  RenData    *data)
{
    GtkWidget *gw_sptounc; /* No change radio button */
    GtkWidget *gw_sptou;   /* Space to unserscore radio button */
    GtkWidget *gw_utosp;   /* Underscore to space radio button */

    gw_sptounc = gtk_radio_button_new_with_label (nullptr, "No change");
    gw_sptou   = gtk_radio_button_new_with_label_from_widget (
                 GTK_RADIO_BUTTON (gw_sptounc), "Space to underscore");
    gw_utosp   = gtk_radio_button_new_with_label_from_widget(
                 GTK_RADIO_BUTTON (gw_sptounc), "Underscore to space");

    switch (rendata_get_spaces (data)) {
    case 0:  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gw_utosp), TRUE); break;
    case 1:  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gw_sptou), TRUE); break;
    default: gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gw_sptounc), TRUE); break;
    }

    g_signal_connect (G_OBJECT (gw_sptounc), "toggled",
                      G_CALLBACK (event_spaces_radio_active), data);
    g_signal_connect (G_OBJECT (gw_sptou), "toggled",
                      G_CALLBACK (event_spaces_radio_active), data);
    g_signal_connect (G_OBJECT (gw_utosp), "toggled",
                      G_CALLBACK (event_spaces_radio_active), data);

    /* Space to underscores box */
    *gw_container = gtk_grid_new ();

    gtk_grid_set_column_spacing (GTK_GRID (*gw_container), 4);

    gtk_grid_attach (GTK_GRID (*gw_container), gw_sptounc, 0, 0, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (*gw_container),
            gw_sptou, gw_sptounc, GTK_POS_BOTTOM, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (*gw_container),
            gw_utosp, gw_sptou, GTK_POS_BOTTOM, 1, 1);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Creates Apply to name/ext/both container.
 *
 * @param[out]    gw_container Pointer to container
 * @param[in,out] data      RenData object with file list and settings
 * @return        none
 */
static void
create_apply_to_names_ext_box (GtkWidget **gw_container,
                               RenData    *data)
{
    GtkWidget *gw_appne; /* Apply to name and extension radio button */
    GtkWidget *gw_appn;  /* Apply to name only radio button */
    GtkWidget *gw_appe;  /* Apply to extension only radio button */

    gw_appne = gtk_radio_button_new_with_label (nullptr,
                                                "Apply to name and ext");
    gw_appn  = gtk_radio_button_new_with_label_from_widget (
               GTK_RADIO_BUTTON (gw_appne), "Apply to name");
    gw_appe  = gtk_radio_button_new_with_label_from_widget (
               GTK_RADIO_BUTTON (gw_appne), "Apply to ext");

    switch (rendata_get_applyto (data)) {
    case 0:  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gw_appe), TRUE); break;
    case 1:  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gw_appn), TRUE); break;
    default: gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gw_appne), TRUE); break;
    }

    g_signal_connect (G_OBJECT (gw_appne), "toggled",
                      G_CALLBACK (event_apply_radio_active), data);
    g_signal_connect (G_OBJECT (gw_appn), "toggled",
                      G_CALLBACK (event_apply_radio_active), data);
    g_signal_connect (G_OBJECT (gw_appe), "toggled",
                      G_CALLBACK (event_apply_radio_active), data);

    /* Apply to names and extensions box */
    *gw_container = gtk_grid_new ();

    gtk_grid_set_column_spacing (GTK_GRID (*gw_container), 4);

    gtk_grid_attach (GTK_GRID (*gw_container), gw_appne, 0, 0, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (*gw_container),
            gw_appn, gw_appne, GTK_POS_BOTTOM, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (*gw_container),
            gw_appe, gw_appn, GTK_POS_BOTTOM, 1, 1);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Creates replace string with other string container.
 *
 * @param[out]    gw_container Pointer to container
 * @param[in,out] data      RenData object with file list and settings
 * @return        none
 */
static void
create_replace_str_with_str_box (GtkWidget **gw_container,
                                 RenData    *data)
{
    GtkWidget *gw_sfrom; /* Replace from entry */
    GtkWidget *gw_sto;   /* Replace to entry */
    GtkWidget *gw_lab1;  /* Description label */
    GtkWidget *gw_lab2;  /* Description label */

    gw_sfrom = gtk_entry_new ();
    gw_sto   = gtk_entry_new ();

    gtk_entry_set_max_length (GTK_ENTRY (gw_sfrom), FN_LEN);
    gtk_entry_set_width_chars (GTK_ENTRY (gw_sfrom), 6);
    gtk_entry_set_max_length (GTK_ENTRY (gw_sto), FN_LEN);
    gtk_entry_set_width_chars (GTK_ENTRY (gw_sto), 6);

    gtk_entry_set_text (GTK_ENTRY (gw_sfrom),
                        rreplace_get_from (rendata_get_rreplace (data)));
    gtk_entry_set_text (GTK_ENTRY (gw_sto),
                        rreplace_get_to (rendata_get_rreplace (data)));

    gw_lab1 = gtk_label_new ("Replace text");
    gw_lab2 = gtk_label_new ("with");

    g_signal_connect (G_OBJECT (gw_sfrom), "changed",
                      G_CALLBACK (event_replace_from_entry_changed), data);
    g_signal_connect (G_OBJECT (gw_sto), "changed",
                      G_CALLBACK (event_replace_to_entry_changed), data);

    *gw_container = gtk_grid_new ();

    gtk_grid_set_column_spacing (GTK_GRID (*gw_container), 4);

    gtk_grid_attach (GTK_GRID (*gw_container), gw_lab1, 0, 0, 3, 1);
    gtk_grid_attach_next_to (GTK_GRID (*gw_container),
            gw_sfrom, gw_lab1, GTK_POS_BOTTOM, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (*gw_container),
            gw_lab2, gw_sfrom, GTK_POS_RIGHT, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (*gw_container),
            gw_sto, gw_lab2, GTK_POS_RIGHT, 1, 1);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Creates container with widgets to delete chars in file name string.
 *
 * @param[out]    gw_container Pointer to container
 * @param[in,out] data      RenData object with file list and settings
 * @return        none
 */
static void
create_delete_chars_box (GtkWidget **gw_container,
                         RenData    *data)
{
    GtkWidget     *gw_del_cnt;     /* Delete chars count spin button */
    GtkWidget     *gw_del_pos;     /* Delete from position spin button */
    GtkWidget     *gw_lab;         /* Description label */
    GtkAdjustment *gw_del_adj_cnt; /* Adjustment for spin button */
    GtkAdjustment *gw_del_adj_pos; /* Adjustment for spin button */

    gw_lab         = gtk_label_new ("Delete text:");
    gw_del_adj_cnt = gtk_adjustment_new (
            rdelete_get_cnt (rendata_get_rdelete (data)),
            0.0, FN_LEN, 1.0, 5.0, 0.0);
    gw_del_cnt     = gtk_spin_button_new (gw_del_adj_cnt, 1.0, 0);

    gtk_widget_set_tooltip_text (gw_del_cnt, "Count");

    gw_del_adj_pos = gtk_adjustment_new (
            rdelete_get_pos (rendata_get_rdelete (data)),
            0.0, FN_LEN, 1.0, 5.0, 0.0);
    gw_del_pos     = gtk_spin_button_new (gw_del_adj_pos, 1.0, 0);

    gtk_widget_set_tooltip_text (gw_del_pos, "At position");

    g_signal_connect (G_OBJECT (gw_del_cnt), "value-changed",
                      G_CALLBACK (event_delete_cnt_changed), data);
    g_signal_connect (G_OBJECT (gw_del_pos), "value-changed",
                      G_CALLBACK (event_delete_pos_changed), data);

    *gw_container = gtk_grid_new ();

    gtk_grid_set_column_spacing (GTK_GRID (*gw_container), 4);

    gtk_grid_attach (GTK_GRID (*gw_container), gw_lab, 0, 0, 2, 1);
    gtk_grid_attach_next_to (GTK_GRID (*gw_container),
            gw_del_cnt, gw_lab, GTK_POS_BOTTOM, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (*gw_container),
            gw_del_pos, gw_del_cnt, GTK_POS_RIGHT, 1, 1);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Creates container with widgets to insert string in file name.
 *
 * @param[out]    gw_container Pointer to container
 * @param[in,out] data      RenData object with file list and settings
 * @return        none
 */
static void
create_insert_string_box (GtkWidget **gw_container,
                          RenData    *data)
{
    GtkWidget     *gw_lab;         /* Description label */
    GtkWidget     *gw_ins_entry;   /* Insert text entry */
    GtkWidget     *gw_ins_pos;     /* Insert text position spin button */
    GtkAdjustment *gw_ins_adj_pos; /* Adjustment for spin button */

    gw_lab       = gtk_label_new ("Insert text:");
    gw_ins_entry = gtk_entry_new ();

    gtk_entry_set_max_length (GTK_ENTRY (gw_ins_entry), FN_LEN);
    gtk_entry_set_width_chars (GTK_ENTRY (gw_ins_entry), 6);

    gtk_entry_set_text (GTK_ENTRY (gw_ins_entry),
                        rinsovr_get_text (rendata_get_rinsert (data)));

    gw_ins_adj_pos = gtk_adjustment_new (
            rinsovr_get_pos (rendata_get_rinsert (data)),
            0.0, FN_LEN, 1.0, 5.0, 0.0);
    gw_ins_pos     = gtk_spin_button_new (gw_ins_adj_pos, 1.0, 0);

    gtk_widget_set_tooltip_text (gw_ins_pos, "At position");

    g_signal_connect (G_OBJECT (gw_ins_pos), "value-changed",
                      G_CALLBACK (event_insert_pos_changed), data);
    g_signal_connect (G_OBJECT (gw_ins_entry), "changed",
                      G_CALLBACK (event_insert_string_entry_changed), data);

    *gw_container = gtk_grid_new ();

    gtk_grid_set_column_spacing (GTK_GRID (*gw_container), 4);

    gtk_grid_attach (GTK_GRID (*gw_container), gw_lab, 0, 0, 2, 1);
    gtk_grid_attach_next_to (GTK_GRID (*gw_container),
            gw_ins_entry, gw_lab, GTK_POS_BOTTOM, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (*gw_container),
            gw_ins_pos, gw_ins_entry, GTK_POS_RIGHT, 1, 1);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Creates container with widgets to overwrite string in file name.
 *
 * @param[out]    gw_container Pointer to container
 * @param[in,out] data      RenData object with file list and settings
 * @return        none
 */
static void
create_overwrite_string_box (GtkWidget **gw_container,
                             RenData    *data)
{
    GtkWidget     *gw_lab;         /* Description label */
    GtkWidget     *gw_ovr_entry;   /* Insert text entry */
    GtkWidget     *gw_ovr_pos;     /* Insert text position spin button */
    GtkAdjustment *gw_ovr_adj_pos; /* Adjustment for spin button */

    gw_lab       = gtk_label_new ("Overwrite text:");
    gw_ovr_entry = gtk_entry_new ();

    gtk_entry_set_max_length (GTK_ENTRY (gw_ovr_entry), FN_LEN);
    gtk_entry_set_width_chars (GTK_ENTRY (gw_ovr_entry), 6);

    gtk_entry_set_text (GTK_ENTRY (gw_ovr_entry),
                        rinsovr_get_text (rendata_get_roverwr (data)));

    gw_ovr_adj_pos = gtk_adjustment_new (
            rinsovr_get_pos (rendata_get_roverwr (data)),
            0.0, FN_LEN, 1.0, 5.0, 0.0);
    gw_ovr_pos     = gtk_spin_button_new (gw_ovr_adj_pos, 1.0, 0);

    gtk_widget_set_tooltip_text (gw_ovr_pos, "At position");

    g_signal_connect (G_OBJECT (gw_ovr_pos), "value-changed",
                      G_CALLBACK (event_overwrite_pos_changed), data);
    g_signal_connect (G_OBJECT (gw_ovr_entry), "changed",
            G_CALLBACK (event_overwrite_string_entry_changed), data);

    *gw_container = gtk_grid_new ();

    gtk_grid_set_column_spacing (GTK_GRID (*gw_container), 4);

    gtk_grid_attach (GTK_GRID (*gw_container), gw_lab, 0, 0, 2, 1);
    gtk_grid_attach_next_to (GTK_GRID (*gw_container),
            gw_ovr_entry, gw_lab, GTK_POS_BOTTOM, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (*gw_container),
            gw_ovr_pos, gw_ovr_entry, GTK_POS_RIGHT, 1, 1);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Creates container with widgets to number file names.
 *
 * @param[out]    gw_container Pointer to container
 * @param[in,out] data      RenData object with file list and settings
 * @return        none
 */
static void
create_number_string_box (GtkWidget **gw_container,
                          RenData    *data)
{
    GtkWidget     *gw_check;              /* Number names checkbox */
    GtkWidget     *gw_num_start_spin;     /* Numbering start spin button */
    GtkAdjustment *gw_num_start_spin_adj; /* Adjustment for spin button */
    GtkWidget     *gw_num_pos_spin;       /* Numbering start spin button */
    GtkAdjustment *gw_num_pos_spin_adj;   /* Adjustment for spin button */

    gw_check = gtk_check_button_new_with_label ("Number files");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gw_check),
            rnumber_get_opt (rendata_get_rnumber (data)) != 0);

    gw_num_start_spin_adj = gtk_adjustment_new (
            rnumber_get_start (rendata_get_rnumber (data)),
            0.0, 1000, 1.0, 5.0, 0.0);
    gw_num_start_spin     = gtk_spin_button_new (gw_num_start_spin_adj, 1.0, 0);

    gw_num_pos_spin_adj   = gtk_adjustment_new (
            rnumber_get_pos (rendata_get_rnumber (data)),
            0.0, FN_LEN, 1.0, 5.0, 0.0);
    gw_num_pos_spin       = gtk_spin_button_new (gw_num_pos_spin_adj, 1.0, 0);

    gtk_widget_set_tooltip_text (gw_num_start_spin, "Start numbering from");
    gtk_widget_set_tooltip_text (gw_num_pos_spin, "Number posistion in name");

    g_signal_connect (G_OBJECT (gw_num_start_spin), "value-changed",
                      G_CALLBACK (event_number_start_changed), data);
    g_signal_connect (G_OBJECT (gw_num_pos_spin), "value-changed",
                      G_CALLBACK (event_number_pos_changed), data);
    g_signal_connect (G_OBJECT (gw_check), "toggled",
                      G_CALLBACK (event_toggle_number_names), data);

    *gw_container = gtk_grid_new ();

    gtk_grid_set_column_spacing (GTK_GRID (*gw_container), 4);

    gtk_grid_attach (GTK_GRID (*gw_container), gw_check, 0, 0, 2, 1);
    gtk_grid_attach_next_to (GTK_GRID (*gw_container),
            gw_num_start_spin, gw_check, GTK_POS_BOTTOM, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (*gw_container),
            gw_num_pos_spin, gw_num_start_spin, GTK_POS_RIGHT, 1, 1);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Creates Rename - Close - Exit after rename container.
 *
 * @param[out]    gw_container Pointer to container
 * @param[in,out] data      RenData object with file list and settings
 * @return        none
 */
static void
create_rename_close_exit_box (GtkWidget **gw_container,
                              RenData    *data)
{
    GtkWidget *gw_but_ok;   /* Rename button */
    GtkWidget *gw_but_cc;   /* Close button */
    GtkWidget *gw_renexit;  /* Exit after rename check button */
    GtkWidget *gw_remember; /* Remember options on exit check button */

    gw_but_ok = gtk_button_new_with_label ("Rename");
    gw_but_cc = gtk_button_new_with_label ("Close");

    g_signal_connect (G_OBJECT (gw_but_ok), "clicked",
                      G_CALLBACK (event_click_rename), data);
    g_signal_connect (G_OBJECT (gw_but_cc), "clicked",
                      G_CALLBACK (event_close), nullptr);

    gw_renexit = gtk_check_button_new_with_label ("Exit after rename");

    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gw_renexit),
            rendata_get_renexit (data));

    g_signal_connect (G_OBJECT (gw_renexit), "toggled",
                      G_CALLBACK (event_toggle_rename_exit), data);

    gw_remember = gtk_check_button_new_with_label ("Remember options on exit");

    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gw_remember),
            rendata_get_rememberopt (data));

    g_signal_connect (G_OBJECT (gw_remember), "toggled",
                      G_CALLBACK (event_toggle_remember_options), data);

    *gw_container = gtk_grid_new ();

    gtk_grid_set_column_spacing (GTK_GRID (*gw_container), 4);

    gtk_grid_attach (GTK_GRID (*gw_container), gw_but_ok, 0, 0, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (*gw_container),
            gw_but_cc, gw_but_ok, GTK_POS_RIGHT, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (*gw_container),
            gw_renexit, gw_but_cc, GTK_POS_RIGHT, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (*gw_container),
            gw_remember, gw_renexit, GTK_POS_RIGHT, 1, 1);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Creates toolbar with add, remove, sort buttons.
 *
 * @param[out]    gw_container Pointer to container
 * @param[in,out] data      RenData object with file list and settings
 * @return        none
 */
static void
create_toolbar (GtkWidget **gw_container,
                RenData    *data)
{
    GtkToolItem *ti_menu_button;
    GtkWidget   *gw_menu;
    GtkWidget   *menu_item;

    *gw_container = gtk_toolbar_new ();

    /* Creating ADD button with menu */
    ti_menu_button = gtk_menu_tool_button_new (
            create_image_widget (W_ICON_ADD), "ADD");
    gtk_tool_item_set_tooltip_text (ti_menu_button, "Add files");
    gtk_toolbar_insert (GTK_TOOLBAR (*gw_container), ti_menu_button, -1);
    g_signal_connect_swapped (ti_menu_button, "clicked",
            G_CALLBACK (event_click_add_files), data->names);
    /* Menu for ADD button */
    gw_menu = gtk_menu_new ();
    menu_item = create_img_menu_item ("Add files from directory",
                                      nullptr,
                                      W_ICON_ADD_DIR);
    gtk_menu_shell_append (GTK_MENU_SHELL (gw_menu), menu_item);
    g_signal_connect_swapped (menu_item, "activate",
            G_CALLBACK (event_click_add_folder_files), data);
    /* Assign menu to menu tool button and show menu */
    gtk_menu_tool_button_set_menu (GTK_MENU_TOOL_BUTTON (ti_menu_button),
                                   gw_menu);
    gtk_widget_show_all (gw_menu);

    /* SEL button with menu */
    ti_menu_button = gtk_menu_tool_button_new (
            create_image_widget (W_ICON_SELECT), "SEL");
    gtk_tool_item_set_tooltip_text (ti_menu_button, "Select / unselect all");
    gtk_toolbar_insert (GTK_TOOLBAR (*gw_container), ti_menu_button, -1);
    g_signal_connect_swapped (ti_menu_button, "clicked",
            G_CALLBACK (rfnames_select_unselect_all), data->names);
    /* Menu for SEL button */
    gw_menu = gtk_menu_new ();
    /* Select all files menu entry */
    menu_item = create_img_menu_item ("Select all files",
                                      nullptr,
                                      W_ICON_SELECT);
    gtk_menu_shell_append (GTK_MENU_SHELL (gw_menu), menu_item);
    g_signal_connect_swapped (menu_item, "activate",
            G_CALLBACK (rfnames_select_files), data->names);
    /* Select all directories menu entry */
    menu_item = create_img_menu_item ("Select all directories",
                                      nullptr,
                                      W_ICON_SELECT);
    gtk_menu_shell_append (GTK_MENU_SHELL (gw_menu), menu_item);
    g_signal_connect_swapped (menu_item, "activate",
            G_CALLBACK (rfnames_select_folders), data->names);
    /* Select all symlink type files/folders */
    menu_item = create_img_menu_item ("Select all symlinks",
                                      nullptr,
                                      W_ICON_SELECT);
    gtk_menu_shell_append (GTK_MENU_SHELL (gw_menu), menu_item);
    g_signal_connect_swapped (menu_item, "activate",
            G_CALLBACK (rfnames_select_symlinks), data->names);
    /* Select all hidden type files/folders */
    menu_item = create_img_menu_item ("Select all hidden",
                                      nullptr,
                                      W_ICON_SELECT);
    gtk_menu_shell_append (GTK_MENU_SHELL (gw_menu), menu_item);
    g_signal_connect_swapped (menu_item, "activate",
            G_CALLBACK (rfnames_select_hidden), data->names);
    /* Invert selection menu entry */
    menu_item = create_img_menu_item ("Invert selection",
                                      nullptr,
                                      W_ICON_SELECT_BL);
    gtk_menu_shell_append (GTK_MENU_SHELL (gw_menu), menu_item);
    g_signal_connect_swapped (menu_item, "activate",
            G_CALLBACK (rfnames_select_invert), data->names);
    /* Unselect all files menu entry */
    menu_item = create_img_menu_item ("Unselect all files",
                                      nullptr,
                                      W_ICON_SELECT_BW);
    gtk_menu_shell_append (GTK_MENU_SHELL (gw_menu), menu_item);
    g_signal_connect_swapped (menu_item, "activate",
            G_CALLBACK (rfnames_unselect_files), data->names);
    /* Unselect all directories menu entry */
    menu_item = create_img_menu_item ("Unselect all directories",
                                      nullptr,
                                      W_ICON_SELECT_BW);
    gtk_menu_shell_append (GTK_MENU_SHELL (gw_menu), menu_item);
    g_signal_connect_swapped (menu_item, "activate",
            G_CALLBACK (rfnames_unselect_folders), data->names);
    /* Unselect all symlink type files/folders */
    menu_item = create_img_menu_item ("Unselect all symlinks",
                                      nullptr,
                                      W_ICON_SELECT_BW);
    gtk_menu_shell_append (GTK_MENU_SHELL (gw_menu), menu_item);
    g_signal_connect_swapped (menu_item, "activate",
            G_CALLBACK (rfnames_unselect_symlinks), data->names);
    /* Unselect all hidden type files/folders */
    menu_item = create_img_menu_item ("Unselect all hidden",
                                      nullptr,
                                      W_ICON_SELECT_BW);
    gtk_menu_shell_append (GTK_MENU_SHELL (gw_menu), menu_item);
    g_signal_connect_swapped (menu_item, "activate",
            G_CALLBACK (rfnames_unselect_hidden), data->names);
    /* Assign menu to menu tool button and show menu */
    gtk_menu_tool_button_set_menu (GTK_MENU_TOOL_BUTTON (ti_menu_button),
                                   gw_menu);
    gtk_widget_show_all (gw_menu);

    /* RES button with menu */
    ti_menu_button = gtk_menu_tool_button_new (
            create_image_widget (W_ICON_REVERT), "RES");
    gtk_tool_item_set_tooltip_text (ti_menu_button,
            "Restore original names for selected items");
    gtk_toolbar_insert (GTK_TOOLBAR (*gw_container), ti_menu_button, -1);
    g_signal_connect_swapped (ti_menu_button, "clicked",
            G_CALLBACK (rfnames_restore_selected), data->names);
    /* Menu for RES button */
    gw_menu = gtk_menu_new ();
    /* Restore all original names menu entry */
    menu_item = create_img_menu_item ("Restore all",
                                      nullptr,
                                      W_ICON_REVERT);
    gtk_menu_shell_append (GTK_MENU_SHELL (gw_menu), menu_item);
    g_signal_connect_swapped (menu_item, "activate",
            G_CALLBACK (rfnames_restore_all), data->names);
    /* Restore original names for all files menu entry */
    menu_item = create_img_menu_item ("Restore all files",
                                      nullptr,
                                      W_ICON_REVERT);
    gtk_menu_shell_append (GTK_MENU_SHELL (gw_menu), menu_item);
    g_signal_connect_swapped (menu_item, "activate",
            G_CALLBACK (rfnames_restore_all_files), data->names);
    /* Restore original names for all folders menu entry */
    menu_item = create_img_menu_item ("Restore all directories",
                                      nullptr,
                                      W_ICON_REVERT);
    gtk_menu_shell_append (GTK_MENU_SHELL (gw_menu), menu_item);
    g_signal_connect_swapped (menu_item, "activate",
            G_CALLBACK (rfnames_restore_all_folders), data->names);
    /* Restore original names for all symlinks menu entry */
    menu_item = create_img_menu_item ("Restore all symlinks",
                                      nullptr,
                                      W_ICON_REVERT);
    gtk_menu_shell_append (GTK_MENU_SHELL (gw_menu), menu_item);
    g_signal_connect_swapped (menu_item, "activate",
            G_CALLBACK (rfnames_restore_all_symlinks), data->names);
    /* Restore original names for all hidden files/dirs menu entry */
    menu_item = create_img_menu_item ("Restore all hidden",
                                      nullptr,
                                      W_ICON_REVERT);
    gtk_menu_shell_append (GTK_MENU_SHELL (gw_menu), menu_item);
    g_signal_connect_swapped (menu_item, "activate",
            G_CALLBACK (rfnames_restore_all_hidden), data->names);
    /* Assign menu to menu tool button and show menu */
    gtk_menu_tool_button_set_menu (GTK_MENU_TOOL_BUTTON (ti_menu_button),
                                   gw_menu);
    gtk_widget_show_all (gw_menu);

    /* DEL button with menu */
    ti_menu_button = gtk_menu_tool_button_new (
            create_image_widget (W_ICON_REMOVE), "DEL");
    gtk_tool_item_set_tooltip_text (ti_menu_button,
            "Remove selected items from list");
    gtk_toolbar_insert (GTK_TOOLBAR (*gw_container), ti_menu_button, -1);
    g_signal_connect_swapped (ti_menu_button, "clicked",
            G_CALLBACK (rfnames_remove_selected), data->names);
    /* Menu for DEL button */
    gw_menu = gtk_menu_new ();
    /* Delete all items from list */
    menu_item = create_img_menu_item ("Remove all",
                                      nullptr,
                                      W_ICON_REMOVE);
    gtk_menu_shell_append (GTK_MENU_SHELL (gw_menu), menu_item);
    g_signal_connect_swapped (menu_item, "activate",
            G_CALLBACK (rfnames_remove_all), data->names);
    /* Delete all files from list */
    menu_item = create_img_menu_item ("Remove all files",
                                      nullptr,
                                      W_ICON_REMOVE);
    gtk_menu_shell_append (GTK_MENU_SHELL (gw_menu), menu_item);
    g_signal_connect_swapped (menu_item, "activate",
            G_CALLBACK (rfnames_remove_all_files), data->names);
    /* Delete all folders from list */
    menu_item = create_img_menu_item ("Remove all directories",
                                      nullptr,
                                      W_ICON_REMOVE);
    gtk_menu_shell_append (GTK_MENU_SHELL (gw_menu), menu_item);
    g_signal_connect_swapped (menu_item, "activate",
            G_CALLBACK (rfnames_remove_all_folders), data->names);
    /* Delete all symlinks from list */
    menu_item = create_img_menu_item ("Remove all symlinks",
                                      nullptr,
                                      W_ICON_REMOVE);
    gtk_menu_shell_append (GTK_MENU_SHELL (gw_menu), menu_item);
    g_signal_connect_swapped (menu_item, "activate",
            G_CALLBACK (rfnames_remove_all_symlinks), data->names);
    /* Delete all hidden files/folders from list */
    menu_item = create_img_menu_item ("Remove all hidden",
                                      nullptr,
                                      W_ICON_REMOVE);
    gtk_menu_shell_append (GTK_MENU_SHELL (gw_menu), menu_item);
    g_signal_connect_swapped (menu_item, "activate",
            G_CALLBACK (rfnames_remove_all_hidden), data->names);
    /* Assign menu to menu tool button and show menu */
    gtk_menu_tool_button_set_menu (GTK_MENU_TOOL_BUTTON (ti_menu_button),
                                   gw_menu);
    gtk_widget_show_all (gw_menu);

    /* SRT button with menu */
    ti_menu_button = gtk_tool_button_new (
            create_image_widget (W_ICON_SORT), "SRT");
    gtk_tool_item_set_tooltip_text (ti_menu_button, "Sort names");
    gtk_toolbar_insert (GTK_TOOLBAR (*gw_container), ti_menu_button, -1);
    g_signal_connect_swapped (ti_menu_button, "clicked",
            G_CALLBACK (rfnames_sort), data->names);

    ti_menu_button = gtk_separator_tool_item_new ();
    gtk_toolbar_insert (GTK_TOOLBAR (*gw_container), ti_menu_button, -1);

    ti_menu_button = gtk_tool_button_new (
            create_image_widget (W_ICON_INFO), "( i )");
    gtk_tool_item_set_tooltip_text (ti_menu_button, "Application info");
    gtk_toolbar_insert (GTK_TOOLBAR (*gw_container), ti_menu_button, -1);
    g_signal_connect_swapped (ti_menu_button, "clicked",
            G_CALLBACK (about_app_dialog), nullptr);
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
 * @param[out]    gw_container Pointer to entry box container pointer
 * @return        none
 */
static void
create_file_name_entries (GFile         **files,
                          uint_fast32_t   file_count,
                          RenData        *data,
                          GtkWidget     **gw_container)
{
    GtkWidget     *gw_entry_box; /* Box fo file name Entry fields */
    GtkWidget     *gw_vp;        /* ViewPort */
    GtkAdjustment *ga_h;         /* Adjustment for scrolled win and viewport */
    GtkAdjustment *ga_v;         /* Adjustment for scrolled win and viewport */

    /* Create box for file name entries */
    gw_entry_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    data->names->file_box = gw_entry_box;

    for (uint_fast32_t i = 0; i < file_count; ++i) {
        if (g_file_query_exists (files[i], FALSE)) {
            rfnames_add_gfile_to_file_box (data->names, files[i]);
        }
    }
    /* Make scrollbars */
    *gw_container = gtk_scrolled_window_new (nullptr, nullptr);

    ga_h = gtk_scrolled_window_get_hadjustment(
            GTK_SCROLLED_WINDOW (*gw_container));
    ga_v = gtk_scrolled_window_get_vadjustment(
            GTK_SCROLLED_WINDOW (*gw_container));

    gw_vp = gtk_viewport_new (ga_h, ga_v);

    gtk_container_add (GTK_CONTAINER (gw_vp), gw_entry_box);
    gtk_container_add (GTK_CONTAINER (*gw_container), gw_vp);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Creates main window, its properties and connects window signals.
 *
 * @param[out]    window       Pointer to a app window container
 * @param[in,out] application  GtkApplication item
 * @param[in,out] data      RenData object with file list and settings
 * @return        none
 */
static void
create_window (GtkWidget        **window,
               GtkApplication    *application,
               RenData           *data)
{
    GdkPixbuf *gd_pix = nullptr; /* Pixbuf for default window icon */

    /* Create window widget */
    *window = gtk_application_window_new (application);

    /* Set window properties */
    gtk_window_set_title (GTK_WINDOW (*window), APP_NAME " v" APP_VER);
    gtk_container_set_border_width (GTK_CONTAINER (*window), 10);
    gtk_window_set_default_size (GTK_WINDOW (*window), WIN_WIDTH, WIN_HEIGHT);
    gtk_window_set_position (GTK_WINDOW (*window), GTK_WIN_POS_CENTER);

    /* Set default application icon */
    if ((gd_pix = get_image (W_ICON_ABOUT)) != nullptr) {
        gtk_window_set_default_icon (gd_pix);
        g_object_unref (gd_pix);
    }
    /* Connect window events */
    g_signal_connect (G_OBJECT (*window), "key-press-event",
                      G_CALLBACK (event_win_key_press), data);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Application shutdown signal.
 *
 * @param[in]  application  GtkApplication item
 * @param[out] data      RenData object with file list and settings
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
    GtkWidget *window;             /* Appliation window */
    GtkWidget *gw_toolbar;         /* Top window tool bar */
    GtkWidget *gw_entry_box;       /* File name list widget */
    GtkWidget *gw_vbox;            /* Main window pack widget */
    GtkWidget *gw_uplc_box;        /* Upcase / lowercase widget */
    GtkWidget *gw_undersc_box;     /* Spaces to underscores widget */
    GtkWidget *gw_replace_str_box; /* Replace text widget */
    GtkWidget *gw_del_str_box;     /* Delete text widget */
    GtkWidget *gw_ins_str_box;     /* Insert text widget */
    GtkWidget *gw_ovr_str_box;     /* Overwrite text widget */
    GtkWidget *gw_number_box;      /* Number names widget */
    GtkWidget *gw_apply_box;       /* Apply to names / extenstions widget */
    GtkWidget *gw_udusc_box;       /* Widget to pack all settings */
    GtkWidget *gw_okcl_box;        /* Rename / Close widget */
    GtkWidget *gw_sep;             /* Separator widget */

    /* Create file name entries and set r_files properties */
    create_file_name_entries (files , (uint_fast32_t) file_count, data,
                              &gw_entry_box);
    /* Create main window and set properties */
    create_window (&window, application, data);
    create_toolbar (&gw_toolbar, data);
    create_upcase_lowercase_box (&gw_uplc_box, data);
    create_spaces_to_underscores_box (&gw_undersc_box, data);
    create_apply_to_names_ext_box (&gw_apply_box, data);
    create_replace_str_with_str_box (&gw_replace_str_box, data);
    create_delete_chars_box (&gw_del_str_box, data);
    create_insert_string_box (&gw_ins_str_box, data);
    create_overwrite_string_box (&gw_ovr_str_box, data);
    create_number_string_box (&gw_number_box, data);

    /* OK, Close box */
    create_rename_close_exit_box (&gw_okcl_box, data);

    /* Box for up down, underscore and apply to boxes */
    gw_udusc_box = gtk_grid_new ();

    gtk_grid_set_column_spacing (GTK_GRID (gw_udusc_box), 4);
    gtk_grid_set_row_spacing (GTK_GRID (gw_udusc_box), 4);

    gtk_grid_attach (GTK_GRID (gw_udusc_box), gw_apply_box, 0, 0, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (gw_udusc_box),
            gw_uplc_box, gw_apply_box, GTK_POS_RIGHT, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (gw_udusc_box),
            gw_undersc_box, gw_uplc_box, GTK_POS_RIGHT, 1, 1);

    gw_sep = gtk_separator_new (GTK_ORIENTATION_HORIZONTAL);

    gtk_grid_attach (GTK_GRID (gw_udusc_box), gw_sep, 0, 1, 3, 1);

    gtk_grid_attach (GTK_GRID (gw_udusc_box), gw_replace_str_box, 0, 2, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (gw_udusc_box),
            gw_ins_str_box, gw_replace_str_box, GTK_POS_RIGHT, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (gw_udusc_box),
            gw_del_str_box, gw_ins_str_box, GTK_POS_RIGHT, 1, 1);

    gw_sep = gtk_separator_new (GTK_ORIENTATION_HORIZONTAL);

    gtk_grid_attach (GTK_GRID (gw_udusc_box), gw_sep, 0, 3, 3, 1);

    gtk_grid_attach (GTK_GRID (gw_udusc_box), gw_ovr_str_box, 1, 4, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (gw_udusc_box),
            gw_number_box, gw_ovr_str_box, GTK_POS_RIGHT, 1, 1);

    /* Main application widgets box */
    gw_vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 4);

    gtk_box_pack_start (GTK_BOX (gw_vbox), gw_toolbar, FALSE, FALSE,  0);
    gtk_box_pack_start (GTK_BOX (gw_vbox),
            gtk_label_new("New file / directory name"), FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (gw_vbox), 
            gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_vbox), gw_entry_box, TRUE, TRUE,  0);
    gtk_box_pack_start (GTK_BOX (gw_vbox), 
            gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_vbox), gw_udusc_box, FALSE, FALSE, 0);
    gtk_box_pack_start (GTK_BOX (gw_vbox), 
            gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX (gw_vbox), gw_okcl_box, FALSE, FALSE, 0);

    gtk_container_add (GTK_CONTAINER (window), gw_vbox);

    /* Apply settings (possibly loaded from config) to file name entries */
    file_names_update_changes (data);

    /* Set focus on first entry if file count is greater than 0 */
    if (rfnames_get_cnt (rendata_get_rfnames (data)) > 0) {
        gtk_widget_grab_focus (data->names->items[0]->entry);
    }
    gtk_widget_show_all (window);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Application activate signal.
 *
 * @param[in,out] application  GtkApplication item
 * @param[in,out] data      RenData object with file list and settings
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

    app = gtk_application_new ("org.nongnu.SmallFileRenamer",
                               G_APPLICATION_HANDLES_OPEN);

    g_signal_connect (app, "shutdown", G_CALLBACK (shutdown), data);
    g_signal_connect (app, "activate", G_CALLBACK (activate), data);
    g_signal_connect (app, "open",     G_CALLBACK (open),     data);

    g_set_application_name (APP_NAME);

    status = g_application_run (G_APPLICATION (app), argc, argv);

    g_object_unref (app);

    return status;
}
/*----------------------------------------------------------------------------*/


