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
 * @version 1.2.9
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
#include "dlgs.h"
#include "strfn.h"
#include "rendata.h"
#include "rconfig.h"
#include "namefn.h"
#include "imgs.h"
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
    char        *s_new   = nullptr;           /* Full path for new file */
    char        *s_old   = nullptr;           /* Full path for old file */
    RenResult    i_res   = REN_OK;            /* Renaming result */
    size_t       ui_dlen = strlen (path);     /* Length of file dir path */
    size_t       ui_olen = strlen (old_name); /* Length of old file name */
    size_t       ui_nlen = strlen (new_name); /* Length of new file name */

    /* Check if new file name is different than old */
    if (ui_olen == ui_nlen && strcmp (old_name, new_name) == 0)
        return REN_NC; /* No chgange */

    /* Alloc memory for full paths of old and new file name */
    s_new = malloc ((ui_dlen + ui_nlen + 1) * sizeof (char));
    s_old = malloc ((ui_dlen + ui_olen + 1) * sizeof (char));

    if (s_new == nullptr || s_old == nullptr) {
        fputs ("Alloc error\n", stderr);
        exit (EXIT_FAILURE);
    }
    memcpy (s_new, path, ui_dlen);
    memcpy (s_old, path, ui_dlen);
    memcpy (s_new + ui_dlen, new_name, ui_nlen);
    memcpy (s_old + ui_dlen, old_name, ui_olen);
    s_new [ui_dlen + ui_nlen] = '\0';
    s_old [ui_dlen + ui_olen] = '\0';

    /* Check if file with new file name don't exist */
    if (access (s_new, F_OK) != 0) {
        i_res = rename (s_old, s_new) == 0 ? 
                REN_OK :     /* OK */
                REN_NOT_REN; /* Could not rename */
    }
    else {
        i_res = REN_EXISTS;  /* File exists */
    }
    free (s_new);
    free (s_old);

    return i_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Apply changes to all file names in buffer and update entries.
 *
 * @param[in,out] rd_data RenData object with file list and settings
 * @return        none
 */
static void
file_names_update_changes (RenData *rd_data)
{
    RFitem        *rf_item; /* RFitem object to process */
    uint_fast32_t  i = 0;   /* i */

    i = rfnames_get_cnt (rendata_get_rfnames (rd_data));

    /* Check if file is checked and skip to next one if it is not */
    while (i--) {
        rf_item = rd_data->names->rf_items[i];

        if (!rfitem_get_checked (rf_item))
            continue;

        /* copy original name to new to process */
        rfitem_set_snew_from_sorg (rf_item);

        /* Execute rename functions */
        name_to_upcase_lowercase (rd_data, i);
        name_spaces_underscores (rd_data, i);
        name_delete_chars (rd_data, i);
        name_replace_strings (rd_data, i);
        name_insert_string (rd_data, i);
        name_overwrite_string (rd_data, i);
        name_number_string (rd_data, i);
        /* Update file name entry if name has changed */
        rfitem_entry_check_and_update (rf_item, rfitem_get_snew (rf_item));
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Scan folder folder content, check for adding and add files, folders.
 *
 * @param[out] rf_names RFnames item with file list
 * @param[in]  gf_dir   Directory to scan
 * @param[in]  i_opt    Options with info which items to add (files, folders)
 * @return     none
 */
static void
enumerate_folder (RFnames   *rf_names,
                  GFile     *gf_dir,
                  const int  i_opt)
{
    GFileEnumerator *f_enum;
    GError          *g_err;
    GFile           *g_file;
    GFileInfo       *f_info;
    GFileType        f_type;

    #ifdef DEBUG
        printf ("Enumerating folder %s\n", g_file_peek_path (gf_dir));
    #endif

    f_enum = g_file_enumerate_children (gf_dir,
                                        "standard::*",
                                        G_FILE_QUERY_INFO_NONE,
                                        nullptr,
                                        &g_err);
    do {
        g_file_enumerator_iterate (f_enum, &f_info, &g_file, nullptr, &g_err);
        if (f_info != nullptr) {
            f_type = g_file_info_get_file_type (f_info);
            /* If hidden file/directory was not set skip adding it */
            if (g_file_info_get_is_hidden (f_info) &&
                !(i_opt & FOLDER_SELECT_HIDDEN)) {
                continue;
            }
            /* If symlink file/directory was not set skip adding it */
            if (g_file_info_get_is_symlink (f_info) &&
                !(i_opt & FOLDER_SELECT_SYMLINKS)) {
                continue;
            }
            if (f_type == G_FILE_TYPE_DIRECTORY) {
                if (i_opt & FOLDER_SELECT_FOLDERS) {
                    rfnames_add_gfile_to_file_box (rf_names, g_file);
                }
                if (i_opt & FOLDER_SCAN_RECURSIVELY) {
                    enumerate_folder (rf_names, g_file, i_opt);
                }
            }
            if (f_type == G_FILE_TYPE_REGULAR &&
                (i_opt & FOLDER_SELECT_FILES)) {
                rfnames_add_gfile_to_file_box (rf_names, g_file);
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
 * @param[out] rf_names RFnames item with file list
 * @param[in]  s_dir    Directory path
 * @param[in]  i_opt    Options with info which items to add (files, folders)
 * @return     none
 */
static void
get_folder_content (RFnames    *rf_names,
                    const char *s_dir,
                    const int   i_opt)
{
    GFile     *gf_dir;
    GError    *g_err;
    GFileInfo *f_info;
    GFileType  f_type;

    gf_dir = g_file_new_for_path (s_dir);
    f_info = g_file_query_info (gf_dir,
                                "standard::*",
                                G_FILE_QUERY_INFO_NONE,
                                nullptr,
                                &g_err);
    if (f_info != nullptr) {
        f_type = g_file_info_get_file_type (f_info);
        g_object_unref (f_info);

        if (f_type == G_FILE_TYPE_DIRECTORY) {
            enumerate_folder (rf_names, gf_dir, i_opt);
        }
    }
    g_object_unref (gf_dir);
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
 * @param[in,out] rd_data RenData object with file list and settings
 * @return     none
 */
static void
event_click_rename (GtkWidget *widget,
                    RenData   *rd_data)
{
    const char    *s_old        = nullptr; /* Old file name */
    const char    *s_new        = nullptr; /* New file name */
    const char    *s_pth        = nullptr; /* Path to file's directory */
    RenResult      i_renamed    = REN_OK;  /* Renaming result */
    uint_fast32_t  ui_ren_count = 0;       /* Number of renamed files */
    uint_fast32_t  ui_names_cnt = 0;       /* Number of files to rename */
    RFitem        *rf_item;                /* Processed RFitem object */

    ui_names_cnt = rfnames_get_cnt (rendata_get_rfnames (rd_data));

    for (uint_fast32_t i = 0; i < ui_names_cnt; ++i) {
        rf_item = rd_data->names->rf_items[i];

        s_new = rfitem_entry_get_text (rf_item);
        s_old = rfitem_get_sorg (rf_item);
        s_pth = rfitem_get_spth (rf_item);

        i_renamed = file_check_and_rename (s_old, s_new, s_pth);

        switch (i_renamed) {

            case REN_OK:
                printf ("File: %s renamed to: %s\n", s_old, s_new);
                /* copy new name to original in buffer */
                rfitem_set_sorg_from_snew (rf_item);
                ++ui_ren_count;
                break;

            case REN_NC:
                printf ("No change in file: %s\n", s_old);
                break;

            case REN_NOT_REN:
                printf ("File: %s could not be renamed\n", s_old);
                break;

            case REN_EXISTS:
                printf ("File: %s already exists\n", s_new);
                break;
        }
        if (i_renamed != REN_OK && i_renamed != REN_NC) {
            /* Revert old file names to new */
            rfitem_set_snew_from_sorg (rf_item);

            /* Update file name in entry */
            rfitem_entry_check_and_update (rf_item, s_old);
        }
    }
    printf ("Renamed %" PRIdFAST32 " files of %" PRIdFAST32 "\n",
            ui_ren_count, ui_names_cnt);

    /* exit application if "Exit after rename" checkbox was selected */
    if (rendata_get_renexit (rd_data))
        event_close (widget, nullptr);
    else
        file_names_update_changes (rd_data);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Insert string text position in SpinButton changed.
 *
 * Copies SpinButton value to RFiles and calls to update file name changes if
 * insert string text is not empty.
 *
 * @param[in]  sp_button SpinButton which received the signal
 * @param[out] rd_data   RenData object with file list and settings
 * @return     none
 */
static void
event_insert_pos_changed (GtkSpinButton *sp_button,
                          RenData       *rd_data)
{
    rinsovr_set_pos (rd_data->ins,
                     (uint8_t) gtk_spin_button_get_value_as_int (sp_button));

    if (!rinsovr_empty (rendata_get_rinsert (rd_data)))
        file_names_update_changes (rd_data);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Insert string text in Entry changed.
 *
 * Copies string to insert to RFiles and calls to update file
 * name changes.
 *
 * @param[in]  widget  Entry widget which received the signal
 * @param[out] rd_data RenData object with file list and settings
 * @return     none
 */
static void
event_insert_string_entry_changed (GtkWidget *widget,
                                   RenData   *rd_data)
{
    const char *s_en = gtk_entry_get_text (GTK_ENTRY (widget));

    rinsovr_set_text (rd_data->ins, s_en);

    file_names_update_changes (rd_data);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Overwrite string text position in SpinButton changed.
 *
 * Copies SpinButton value to RFiles and calls to update file name changes if
 * overwrite string text is not empty.
 *
 * @param[in]  sp_button SpinButton which received the signal
 * @param[out] rd_data   RenData object with file list and settings
 * @return     none
 */
static void
event_overwrite_pos_changed (GtkSpinButton *sp_button,
                             RenData       *rd_data)
{
    rinsovr_set_pos (rd_data->ovrw,
                     (uint8_t) gtk_spin_button_get_value_as_int (sp_button));

    if (!rinsovr_empty (rendata_get_roverwr (rd_data)))
        file_names_update_changes (rd_data);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Overwrite string text in Entry changed.
 *
 * Copies string to overwrite to RFiles and calls to update file
 * name changes.
 *
 * @param[in]  widget  Entry widget which received the signal
 * @param[out] rd_data RenData object with file list and settings
 * @return     none
 */
static void
event_overwrite_string_entry_changed (GtkWidget *widget,
                                      RenData   *rd_data)
{
    const char *s_en = gtk_entry_get_text (GTK_ENTRY (widget));

    rinsovr_set_text (rd_data->ovrw, s_en);

    file_names_update_changes (rd_data);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Delete char number of chars to delete in SpinButton changed.
 *
 * Reads SpinButon value of chars to delete and calls to update file name
 * changes.
 *
 * @param[in]  sp_button SpinButton which received the signal
 * @param[out] rd_data   RenData object with file list and settings
 * @return     none
 */
static void
event_delete_cnt_changed (GtkSpinButton *sp_button,
                          RenData       *rd_data)
{
    rdelete_set_cnt (rd_data->del,
                     (uint8_t) gtk_spin_button_get_value_as_int (sp_button));

    file_names_update_changes (rd_data);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Delete char position velue in SpinButton changed.
 *
 * Reads SpinButon value and if number of chars to delete is greater then 0
 * it calls to update file name changes.
 *
 * @param[in]  sp_button SpinButton which received the signal
 * @param[out] rd_data   RenData object with file list and settings
 * @return     none
 */
static void
event_delete_pos_changed (GtkSpinButton *sp_button,
                          RenData       *rd_data)
{
    rdelete_set_pos (rd_data->del,
                     (uint8_t) gtk_spin_button_get_value_as_int (sp_button));

    if (rdelete_get_cnt (rendata_get_rdelete (rd_data)) > 0)
        file_names_update_changes (rd_data);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Upper/lower case RadioButton changed.
 *
 * Reads which RadioButton is selected and runs file name update changes
 * function.
 *
 * @param[in]  radiob  RadioButton which received the signal
 * @param[out] rd_data RenData object with file list and settings
 * @return     none
 */
static void
event_case_radio_active (GtkRadioButton *radiob,
                         RenData        *rd_data)
{
    static int8_t dbl = 0; /* to remove double toggling */

    if (dbl ^= 1) {
        rendata_set_uplo (rd_data, get_radio_active (radiob));
        file_names_update_changes (rd_data);
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
 * @param[out] rd_data RenData object with file list and settings
 * @return     none
 */
static void
event_spaces_radio_active (GtkRadioButton *radiob,
                           RenData        *rd_data)
{
    static int8_t dbl = 0; /* to remove double toggling */

    if (dbl ^= 1) {
        rendata_set_spaces (rd_data, get_radio_active (radiob));
        file_names_update_changes (rd_data);
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
 * @param[out] rd_data RenData object with file list and settings
 * @return     none
 */
static void
event_replace_from_entry_changed (GtkWidget *widget,
                                  RenData   *rd_data)
{
    const char *s_en = gtk_entry_get_text (GTK_ENTRY (widget));

    rreplace_set_from (rd_data->replace, s_en);

    file_names_update_changes (rd_data);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Changed "replace to" value in replace strings section.
 *
 * Copies "replace to" value to RFiles, checks if "replace from" string is not
 * empty and calls update changes function.
 *
 * @param[in]  widget  Entry widget which received the signal
 * @param[out] rd_data RenData object with file list and settings
 * @return     none
 */
static void
event_replace_to_entry_changed (GtkWidget *widget,
                                RenData   *rd_data)
{
    const char *s_en = gtk_entry_get_text (GTK_ENTRY (widget));

    rreplace_set_to (rd_data->replace, s_en);

    if (!rreplace_empty_from (rendata_get_rreplace (rd_data)))
        file_names_update_changes (rd_data);
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
 * @param[out] rd_data RenData object with file list and settings
 * @return     none
 */
static void
event_apply_radio_active (GtkRadioButton *radiob,
                          RenData        *rd_data)
{
    static int8_t dbl = 0; /* to remove double toggling */

    if (dbl ^= 1) {
        /* read apply to names/ext active RadioButton */
        rendata_set_applyto (rd_data, get_radio_active (radiob));

        file_names_update_changes (rd_data);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Number names CheckBox state changed.
 *
 * @param[in]  toggleb ToggleButton which received the signal
 * @param[out] rd_data RenData object with file list and settings
 * @return     none
 */
static void
event_toggle_number_names (GtkToggleButton *toggleb,
                           RenData         *rd_data)
{
    rnumber_set_opt (rd_data->number,
            (int8_t) gtk_toggle_button_get_active (toggleb));

    file_names_update_changes (rd_data);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Numbering strings start value in SpinButton changed.
 *
 * @param[in]  sp_button SpinButton which received the signal
 * @param[out] rd_data   RenData object with file list and settings
 * @return     none
 */
static void
event_number_start_changed (GtkSpinButton *sp_button,
                            RenData        *rd_data)
{
    rnumber_set_start (rd_data->number, 
        (uint32_t) gtk_spin_button_get_value_as_int (sp_button));

    if (rnumber_get_opt (rendata_get_rnumber (rd_data)))
        file_names_update_changes (rd_data);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Numbering strings position in string SpinButton changed.
 *
 * @param[in]  sp_button SpinButton which received the signal
 * @param[out] rd_data   RenData object with file list and settings
 * @return     none
 */
static void
event_number_pos_changed (GtkSpinButton *sp_button,
                          RenData       *rd_data)
{
    rnumber_set_pos (rd_data->number,
        (uint8_t) gtk_spin_button_get_value_as_int (sp_button));

    if (rnumber_get_opt (rendata_get_rnumber (rd_data)))
        file_names_update_changes (rd_data);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Exit after rename CheckBox state changed.
 *
 * Sets renexit value in r_files structure based on the value of ChackBox.
 *
 * @param[in]  toggleb ToggleButton which received the signal
 * @param[out] rd_data RenData object with file list and settings
 * @return     none
 */
static void
event_toggle_rename_exit (GtkToggleButton *toggleb,
                          RenData         *rd_data)
{
    rendata_set_renexit (rd_data,
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
 * @param[out] rd_data RenData object with file list and settings
 * @return     none
 */
static void
event_toggle_remember_options (GtkToggleButton *toggleb,
                               RenData         *rd_data)
{
    rendata_set_rememberopt (rd_data,
            gtk_toggle_button_get_active (toggleb));
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  React to key pressed in main window.
 *
 * @param[in]     widget    The object which received the signal
 * @param[in]     event     The event which triggered this signal
 * @param[in,out] rd_data   RenData object with file list and settings
 */
static gboolean
event_win_key_press (GtkWidget   *widget,
                     GdkEventKey *event,
                     RenData     *rd_data)
{
    /* Catch Esc key in main window and exit */
    if (event->keyval == GDK_KEY_Escape)
        event_close (widget, nullptr);

    /* Catch Enter and preform rename */
    if (event->keyval == GDK_KEY_Return) {
        event_click_rename (widget, rd_data);
    }
    return FALSE;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Add files button pressed.
 *
 * @param[in,out] rf_names RFnames object with file list
 * @return        none
 */
static void
event_click_add_files (RFnames *rf_names)
{
    GSList       *gs_files = nullptr; /* File list returned by dialog */
    GSList       *gs_fn    = nullptr; /* File list copy */
    uint_fast32_t ui_pcnt  = 0;       /* Previous number of file items */

    ui_pcnt  = rfnames_get_cnt (rf_names);
    gs_files = add_files_dialog (nullptr);
    gs_fn    = gs_files;

    while (gs_fn) {
        const char *s_fn = gs_fn->data;
        rfnames_add_sfile_to_file_box (rf_names, s_fn);
        gs_fn = gs_fn->next;
    }
    g_slist_free_full (gs_files, g_free);

    /* Select first entry after adding to empty list */
    if (ui_pcnt == 0 && rfnames_get_cnt (rf_names) > 0) {
        gtk_widget_grab_focus (rf_names->rf_items[0]->entry);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Add files from folder button pressed.
 *
 * @param[in,out] rd_data RenData object with settings
 * @return        none
 */
static void
event_click_add_folder_files (RenData *rd_data)
{
    int     i_opt   = 0;       /* Options for select files from directory */
    size_t  ui_pcnt = 0;       /* Previous number of file items on list */
    char   *s_dir   = nullptr; /* Folder path */

    ui_pcnt = rfnames_get_cnt (rendata_get_rfnames (rd_data));
    i_opt   = rendata_get_dirsel (rd_data);
    i_opt   = i_opt < 1 ? 1 : i_opt;
    s_dir   = add_files_folder_dialog (nullptr, &i_opt);

    if (s_dir != nullptr) {
        if (i_opt > 0) {
            get_folder_content (rd_data->names, s_dir, i_opt);
            rendata_set_dirsel (rd_data, (int8_t) i_opt);
        }
        free (s_dir);
    }
    gtk_widget_show_all (rd_data->names->file_box);

    /* Select first entry after adding to empty list */
    if (ui_pcnt == 0 && rfnames_get_cnt (rendata_get_rfnames (rd_data)) > 0) {
        gtk_widget_grab_focus (rd_data->names->rf_items[0]->entry);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create GtkImage widget with desired icon/image.
 *
 * @param[in] i_but  Icon number
 * @return    Imge widget
 */
static GtkWidget *
create_image_widget (const IconImg i_but)
{
    GtkWidget *gw_img = nullptr; /* GtkImage widget to return */
    GdkPixbuf *gd_pix = nullptr; /* Pigbuf to load graphics */

    if (i_but < W_ICON_COUNT && (gd_pix = get_image (i_but)) != nullptr) {
        gw_img = gtk_image_new_from_pixbuf (gd_pix);
        g_object_unref (gd_pix);
    }
    return gw_img;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create GtkMenuItem widget with image, text, and tooltip.
 *
 * @param[in] s_label Menu entry label
 * @param[in] s_hint  Menu entry tooltip
 * @param[in] i_but   Icon number
 * @return    Result menu entry widget
 */
static GtkWidget *
create_img_menu_item (const char    *s_label,
                      const char    *s_hint,
                      const IconImg  i_but)
{
    GtkWidget *gw_item; /* GtkMenuItem widget to return */
    GtkWidget *gw_box;  /* Box to pack image and label for menu item */
    GtkWidget *gw_img;  /* GtkImage widget for graphics */

    gw_item = gtk_menu_item_new ();
    gw_box  = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 2);

    if ((gw_img = create_image_widget (i_but)) != nullptr) {
        gtk_box_pack_start (GTK_BOX (gw_box), gw_img, FALSE, FALSE, 4);
    }
    if (s_label != nullptr && s_label[0] != '\0') {
        GtkWidget *gw_lab = gtk_label_new (s_label);
        /*gtk_container_add (GTK_CONTAINER (gw_box), gw_lab);*/
        gtk_box_pack_start (GTK_BOX (gw_box), gw_lab, FALSE, FALSE, 4);
    }
    if (s_hint != nullptr && s_hint[0] != '\0') {
        gtk_widget_set_tooltip_text (gw_item, s_hint);
    }
    gtk_container_add (GTK_CONTAINER (gw_item), gw_box);
    return gw_item;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Creates upcase / lowercase container.
 *
 * @param[out]    gw_container Pointer to container
 * @param[in,out] rd_data      RenData object with file list and settings
 * @return        none
 */
static void
create_upcase_lowercase_box (GtkWidget **gw_container,
                             RenData    *rd_data)
{
    GtkWidget *gw_ncc; /* No change radio button */
    GtkWidget *gw_lcc; /* Lovercase radio button */
    GtkWidget *gw_upc; /* Uppercase radio button */

    gw_ncc = gtk_radio_button_new_with_label (nullptr, "No change");
    gw_lcc = gtk_radio_button_new_with_label_from_widget (
             GTK_RADIO_BUTTON (gw_ncc), "To lowercase");
    gw_upc = gtk_radio_button_new_with_label_from_widget (
             GTK_RADIO_BUTTON (gw_ncc), "To uppercase");

    switch (rendata_get_uplo (rd_data)) {
    case 0:  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gw_upc), TRUE); break;
    case 1:  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gw_lcc), TRUE); break;
    default: gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gw_ncc), TRUE); break;
    }

    g_signal_connect (G_OBJECT(gw_ncc), "toggled",
                      G_CALLBACK (event_case_radio_active), rd_data);
    g_signal_connect (G_OBJECT (gw_lcc), "toggled",
                      G_CALLBACK (event_case_radio_active), rd_data);
    g_signal_connect (G_OBJECT (gw_upc), "toggled",
                      G_CALLBACK (event_case_radio_active), rd_data);

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
 * @param[in,out] rd_data      RenData object with file list and settings
 * @return        none
 */
static void
create_spaces_to_underscores_box (GtkWidget **gw_container,
                                  RenData    *rd_data)
{
    GtkWidget *gw_sptounc; /* No change radio button */
    GtkWidget *gw_sptou;   /* Space to unserscore radio button */
    GtkWidget *gw_utosp;   /* Underscore to space radio button */

    gw_sptounc = gtk_radio_button_new_with_label (nullptr, "No change");
    gw_sptou   = gtk_radio_button_new_with_label_from_widget (
                 GTK_RADIO_BUTTON (gw_sptounc), "Space to underscore");
    gw_utosp   = gtk_radio_button_new_with_label_from_widget(
                 GTK_RADIO_BUTTON (gw_sptounc), "Underscore to space");

    switch (rendata_get_spaces (rd_data)) {
    case 0:  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gw_utosp), TRUE); break;
    case 1:  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gw_sptou), TRUE); break;
    default: gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gw_sptounc), TRUE); break;
    }

    g_signal_connect (G_OBJECT (gw_sptounc), "toggled",
                      G_CALLBACK (event_spaces_radio_active), rd_data);
    g_signal_connect (G_OBJECT (gw_sptou), "toggled",
                      G_CALLBACK (event_spaces_radio_active), rd_data);
    g_signal_connect (G_OBJECT (gw_utosp), "toggled",
                      G_CALLBACK (event_spaces_radio_active), rd_data);

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
 * @param[in,out] rd_data      RenData object with file list and settings
 * @return        none
 */
static void
create_apply_to_names_ext_box (GtkWidget **gw_container,
                               RenData    *rd_data)
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

    switch (rendata_get_applyto (rd_data)) {
    case 0:  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gw_appe), TRUE); break;
    case 1:  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gw_appn), TRUE); break;
    default: gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gw_appne), TRUE); break;
    }

    g_signal_connect (G_OBJECT (gw_appne), "toggled",
                      G_CALLBACK (event_apply_radio_active), rd_data);
    g_signal_connect (G_OBJECT (gw_appn), "toggled",
                      G_CALLBACK (event_apply_radio_active), rd_data);
    g_signal_connect (G_OBJECT (gw_appe), "toggled",
                      G_CALLBACK (event_apply_radio_active), rd_data);

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
 * @param[in,out] rd_data      RenData object with file list and settings
 * @return        none
 */
static void
create_replace_str_with_str_box (GtkWidget **gw_container,
                                 RenData    *rd_data)
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
                        rreplace_get_from (rendata_get_rreplace (rd_data)));
    gtk_entry_set_text (GTK_ENTRY (gw_sto),
                        rreplace_get_to (rendata_get_rreplace (rd_data)));

    gw_lab1 = gtk_label_new ("Replace text");
    gw_lab2 = gtk_label_new ("with");

    g_signal_connect (G_OBJECT (gw_sfrom), "changed",
                      G_CALLBACK (event_replace_from_entry_changed), rd_data);
    g_signal_connect (G_OBJECT (gw_sto), "changed",
                      G_CALLBACK (event_replace_to_entry_changed), rd_data);

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
 * @param[in,out] rd_data      RenData object with file list and settings
 * @return        none
 */
static void
create_delete_chars_box (GtkWidget **gw_container,
                         RenData    *rd_data)
{
    GtkWidget     *gw_del_cnt;     /* Delete chars count spin button */
    GtkWidget     *gw_del_pos;     /* Delete from position spin button */
    GtkWidget     *gw_lab;         /* Description label */
    GtkAdjustment *gw_del_adj_cnt; /* Adjustment for spin button */
    GtkAdjustment *gw_del_adj_pos; /* Adjustment for spin button */

    gw_lab         = gtk_label_new ("Delete text:");
    gw_del_adj_cnt = gtk_adjustment_new (
            rdelete_get_cnt (rendata_get_rdelete (rd_data)),
            0.0, FN_LEN, 1.0, 5.0, 0.0);
    gw_del_cnt     = gtk_spin_button_new (gw_del_adj_cnt, 1.0, 0);

    gtk_widget_set_tooltip_text (gw_del_cnt, "Count");

    gw_del_adj_pos = gtk_adjustment_new (
            rdelete_get_pos (rendata_get_rdelete (rd_data)),
            0.0, FN_LEN, 1.0, 5.0, 0.0);
    gw_del_pos     = gtk_spin_button_new (gw_del_adj_pos, 1.0, 0);

    gtk_widget_set_tooltip_text (gw_del_pos, "At position");

    g_signal_connect (G_OBJECT (gw_del_cnt), "value-changed",
                      G_CALLBACK (event_delete_cnt_changed), rd_data);
    g_signal_connect (G_OBJECT (gw_del_pos), "value-changed",
                      G_CALLBACK (event_delete_pos_changed), rd_data);

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
 * @param[in,out] rd_data      RenData object with file list and settings
 * @return        none
 */
static void
create_insert_string_box (GtkWidget **gw_container,
                          RenData    *rd_data)
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
                        rinsovr_get_text (rendata_get_rinsert (rd_data)));

    gw_ins_adj_pos = gtk_adjustment_new (
            rinsovr_get_pos (rendata_get_rinsert (rd_data)),
            0.0, FN_LEN, 1.0, 5.0, 0.0);
    gw_ins_pos     = gtk_spin_button_new (gw_ins_adj_pos, 1.0, 0);

    gtk_widget_set_tooltip_text (gw_ins_pos, "At position");

    g_signal_connect (G_OBJECT (gw_ins_pos), "value-changed",
                      G_CALLBACK (event_insert_pos_changed), rd_data);
    g_signal_connect (G_OBJECT (gw_ins_entry), "changed",
                      G_CALLBACK (event_insert_string_entry_changed), rd_data);

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
 * @param[in,out] rd_data      RenData object with file list and settings
 * @return        none
 */
static void
create_overwrite_string_box (GtkWidget **gw_container,
                             RenData    *rd_data)
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
                        rinsovr_get_text (rendata_get_roverwr (rd_data)));

    gw_ovr_adj_pos = gtk_adjustment_new (
            rinsovr_get_pos (rendata_get_roverwr (rd_data)),
            0.0, FN_LEN, 1.0, 5.0, 0.0);
    gw_ovr_pos     = gtk_spin_button_new (gw_ovr_adj_pos, 1.0, 0);

    gtk_widget_set_tooltip_text (gw_ovr_pos, "At position");

    g_signal_connect (G_OBJECT (gw_ovr_pos), "value-changed",
                      G_CALLBACK (event_overwrite_pos_changed), rd_data);
    g_signal_connect (G_OBJECT (gw_ovr_entry), "changed",
            G_CALLBACK (event_overwrite_string_entry_changed), rd_data);

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
 * @param[in,out] rd_data      RenData object with file list and settings
 * @return        none
 */
static void
create_number_string_box (GtkWidget **gw_container,
                          RenData    *rd_data)
{
    GtkWidget     *gw_check;              /* Number names checkbox */
    GtkWidget     *gw_num_start_spin;     /* Numbering start spin button */
    GtkAdjustment *gw_num_start_spin_adj; /* Adjustment for spin button */
    GtkWidget     *gw_num_pos_spin;       /* Numbering start spin button */
    GtkAdjustment *gw_num_pos_spin_adj;   /* Adjustment for spin button */

    gw_check = gtk_check_button_new_with_label ("Number files");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gw_check),
            rnumber_get_opt (rendata_get_rnumber (rd_data)) != 0);

    gw_num_start_spin_adj = gtk_adjustment_new (
            rnumber_get_start (rendata_get_rnumber (rd_data)),
            0.0, 1000, 1.0, 5.0, 0.0);
    gw_num_start_spin     = gtk_spin_button_new (gw_num_start_spin_adj, 1.0, 0);

    gw_num_pos_spin_adj   = gtk_adjustment_new (
            rnumber_get_pos (rendata_get_rnumber (rd_data)),
            0.0, FN_LEN, 1.0, 5.0, 0.0);
    gw_num_pos_spin       = gtk_spin_button_new (gw_num_pos_spin_adj, 1.0, 0);

    gtk_widget_set_tooltip_text (gw_num_start_spin, "Start numbering from");
    gtk_widget_set_tooltip_text (gw_num_pos_spin, "Number posistion in name");

    g_signal_connect (G_OBJECT (gw_num_start_spin), "value-changed",
                      G_CALLBACK (event_number_start_changed), rd_data);
    g_signal_connect (G_OBJECT (gw_num_pos_spin), "value-changed",
                      G_CALLBACK (event_number_pos_changed), rd_data);
    g_signal_connect (G_OBJECT (gw_check), "toggled",
                      G_CALLBACK (event_toggle_number_names), rd_data);

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
 * @param[in,out] rd_data      RenData object with file list and settings
 * @return        none
 */
static void
create_rename_close_exit_box (GtkWidget **gw_container,
                              RenData    *rd_data)
{
    GtkWidget *gw_but_ok;   /* Rename button */
    GtkWidget *gw_but_cc;   /* Close button */
    GtkWidget *gw_renexit;  /* Exit after rename check button */
    GtkWidget *gw_remember; /* Remember options on exit check button */

    gw_but_ok = gtk_button_new_with_label ("Rename");
    gw_but_cc = gtk_button_new_with_label ("Close");

    g_signal_connect (G_OBJECT (gw_but_ok), "clicked",
                      G_CALLBACK (event_click_rename), rd_data);
    g_signal_connect (G_OBJECT (gw_but_cc), "clicked",
                      G_CALLBACK (event_close), nullptr);

    gw_renexit = gtk_check_button_new_with_label ("Exit after rename");

    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gw_renexit),
            rendata_get_renexit (rd_data));

    g_signal_connect (G_OBJECT (gw_renexit), "toggled",
                      G_CALLBACK (event_toggle_rename_exit), rd_data);

    gw_remember = gtk_check_button_new_with_label ("Remember options on exit");

    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gw_remember),
            rendata_get_rememberopt (rd_data));

    g_signal_connect (G_OBJECT (gw_remember), "toggled",
                      G_CALLBACK (event_toggle_remember_options), rd_data);

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
 * @param[in,out] rd_data      RenData object with file list and settings
 * @return        none
 */
static void
create_toolbar (GtkWidget **gw_container,
                RenData    *rd_data)
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
            G_CALLBACK (event_click_add_files), rd_data->names);
    /* Menu for ADD button */
    gw_menu = gtk_menu_new ();
    menu_item = create_img_menu_item ("Add files from directory",
                                      nullptr,
                                      W_ICON_ADD_DIR);
    gtk_menu_shell_append (GTK_MENU_SHELL (gw_menu), menu_item);
    g_signal_connect_swapped (menu_item, "activate",
            G_CALLBACK (event_click_add_folder_files), rd_data);
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
            G_CALLBACK (rfnames_select_unselect_all), rd_data->names);
    /* Menu for SEL button */
    gw_menu = gtk_menu_new ();
    /* Select all files menu entry */
    menu_item = create_img_menu_item ("Select all files",
                                      nullptr,
                                      W_ICON_SELECT);
    gtk_menu_shell_append (GTK_MENU_SHELL (gw_menu), menu_item);
    g_signal_connect_swapped (menu_item, "activate",
            G_CALLBACK (rfnames_select_files), rd_data->names);
    /* Select all directories menu entry */
    menu_item = create_img_menu_item ("Select all directories",
                                      nullptr,
                                      W_ICON_SELECT);
    gtk_menu_shell_append (GTK_MENU_SHELL (gw_menu), menu_item);
    g_signal_connect_swapped (menu_item, "activate",
            G_CALLBACK (rfnames_select_folders), rd_data->names);
    /* Select all symlink type files/folders */
    menu_item = create_img_menu_item ("Select all symlinks",
                                      nullptr,
                                      W_ICON_SELECT);
    gtk_menu_shell_append (GTK_MENU_SHELL (gw_menu), menu_item);
    g_signal_connect_swapped (menu_item, "activate",
            G_CALLBACK (rfnames_select_symlinks), rd_data->names);
    /* Select all hidden type files/folders */
    menu_item = create_img_menu_item ("Select all hidden",
                                      nullptr,
                                      W_ICON_SELECT);
    gtk_menu_shell_append (GTK_MENU_SHELL (gw_menu), menu_item);
    g_signal_connect_swapped (menu_item, "activate",
            G_CALLBACK (rfnames_select_hidden), rd_data->names);
    /* Invert selection menu entry */
    menu_item = create_img_menu_item ("Invert selection",
                                      nullptr,
                                      W_ICON_SELECT_BL);
    gtk_menu_shell_append (GTK_MENU_SHELL (gw_menu), menu_item);
    g_signal_connect_swapped (menu_item, "activate",
            G_CALLBACK (rfnames_select_invert), rd_data->names);
    /* Unselect all files menu entry */
    menu_item = create_img_menu_item ("Unselect all files",
                                      nullptr,
                                      W_ICON_SELECT_BW);
    gtk_menu_shell_append (GTK_MENU_SHELL (gw_menu), menu_item);
    g_signal_connect_swapped (menu_item, "activate",
            G_CALLBACK (rfnames_unselect_files), rd_data->names);
    /* Unselect all directories menu entry */
    menu_item = create_img_menu_item ("Unselect all directories",
                                      nullptr,
                                      W_ICON_SELECT_BW);
    gtk_menu_shell_append (GTK_MENU_SHELL (gw_menu), menu_item);
    g_signal_connect_swapped (menu_item, "activate",
            G_CALLBACK (rfnames_unselect_folders), rd_data->names);
    /* Unselect all symlink type files/folders */
    menu_item = create_img_menu_item ("Unselect all symlinks",
                                      nullptr,
                                      W_ICON_SELECT_BW);
    gtk_menu_shell_append (GTK_MENU_SHELL (gw_menu), menu_item);
    g_signal_connect_swapped (menu_item, "activate",
            G_CALLBACK (rfnames_unselect_symlinks), rd_data->names);
    /* Unselect all hidden type files/folders */
    menu_item = create_img_menu_item ("Unselect all hidden",
                                      nullptr,
                                      W_ICON_SELECT_BW);
    gtk_menu_shell_append (GTK_MENU_SHELL (gw_menu), menu_item);
    g_signal_connect_swapped (menu_item, "activate",
            G_CALLBACK (rfnames_unselect_hidden), rd_data->names);
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
            G_CALLBACK (rfnames_restore_selected), rd_data->names);
    /* Menu for RES button */
    gw_menu = gtk_menu_new ();
    /* Restore all original names menu entry */
    menu_item = create_img_menu_item ("Restore all",
                                      nullptr,
                                      W_ICON_REVERT);
    gtk_menu_shell_append (GTK_MENU_SHELL (gw_menu), menu_item);
    g_signal_connect_swapped (menu_item, "activate",
            G_CALLBACK (rfnames_restore_all), rd_data->names);
    /* Restore original names for all files menu entry */
    menu_item = create_img_menu_item ("Restore all files",
                                      nullptr,
                                      W_ICON_REVERT);
    gtk_menu_shell_append (GTK_MENU_SHELL (gw_menu), menu_item);
    g_signal_connect_swapped (menu_item, "activate",
            G_CALLBACK (rfnames_restore_all_files), rd_data->names);
    /* Restore original names for all folders menu entry */
    menu_item = create_img_menu_item ("Restore all directories",
                                      nullptr,
                                      W_ICON_REVERT);
    gtk_menu_shell_append (GTK_MENU_SHELL (gw_menu), menu_item);
    g_signal_connect_swapped (menu_item, "activate",
            G_CALLBACK (rfnames_restore_all_folders), rd_data->names);
    /* Restore original names for all symlinks menu entry */
    menu_item = create_img_menu_item ("Restore all symlinks",
                                      nullptr,
                                      W_ICON_REVERT);
    gtk_menu_shell_append (GTK_MENU_SHELL (gw_menu), menu_item);
    g_signal_connect_swapped (menu_item, "activate",
            G_CALLBACK (rfnames_restore_all_symlinks), rd_data->names);
    /* Restore original names for all hidden files/dirs menu entry */
    menu_item = create_img_menu_item ("Restore all hidden",
                                      nullptr,
                                      W_ICON_REVERT);
    gtk_menu_shell_append (GTK_MENU_SHELL (gw_menu), menu_item);
    g_signal_connect_swapped (menu_item, "activate",
            G_CALLBACK (rfnames_restore_all_hidden), rd_data->names);
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
            G_CALLBACK (rfnames_remove_selected), rd_data->names);
    /* Menu for DEL button */
    gw_menu = gtk_menu_new ();
    /* Delete all items from list */
    menu_item = create_img_menu_item ("Remove all",
                                      nullptr,
                                      W_ICON_REMOVE);
    gtk_menu_shell_append (GTK_MENU_SHELL (gw_menu), menu_item);
    g_signal_connect_swapped (menu_item, "activate",
            G_CALLBACK (rfnames_remove_all), rd_data->names);
    /* Delete all files from list */
    menu_item = create_img_menu_item ("Remove all files",
                                      nullptr,
                                      W_ICON_REMOVE);
    gtk_menu_shell_append (GTK_MENU_SHELL (gw_menu), menu_item);
    g_signal_connect_swapped (menu_item, "activate",
            G_CALLBACK (rfnames_remove_all_files), rd_data->names);
    /* Delete all folders from list */
    menu_item = create_img_menu_item ("Remove all directories",
                                      nullptr,
                                      W_ICON_REMOVE);
    gtk_menu_shell_append (GTK_MENU_SHELL (gw_menu), menu_item);
    g_signal_connect_swapped (menu_item, "activate",
            G_CALLBACK (rfnames_remove_all_folders), rd_data->names);
    /* Delete all symlinks from list */
    menu_item = create_img_menu_item ("Remove all symlinks",
                                      nullptr,
                                      W_ICON_REMOVE);
    gtk_menu_shell_append (GTK_MENU_SHELL (gw_menu), menu_item);
    g_signal_connect_swapped (menu_item, "activate",
            G_CALLBACK (rfnames_remove_all_symlinks), rd_data->names);
    /* Delete all hidden files/folders from list */
    menu_item = create_img_menu_item ("Remove all hidden",
                                      nullptr,
                                      W_ICON_REMOVE);
    gtk_menu_shell_append (GTK_MENU_SHELL (gw_menu), menu_item);
    g_signal_connect_swapped (menu_item, "activate",
            G_CALLBACK (rfnames_remove_all_hidden), rd_data->names);
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
            G_CALLBACK (rfnames_sort), rd_data->names);

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
 * @param[in]     n_files      Length of files
 * @param[in,out] rd_data      RenData object with file list and settings
 * @param[out]    gw_container Pointer to entry box container pointer
 * @return        none
 */
static void
create_file_name_entries (GFile         **files,
                          uint_fast32_t   n_files,
                          RenData        *rd_data,
                          GtkWidget     **gw_container)
{
    GtkWidget     *gw_entry_box; /* Box fo file name Entry fields */
    GtkWidget     *gw_vp;        /* ViewPort */
    GtkAdjustment *ga_h;         /* Adjustment for scrolled win and viewport */
    GtkAdjustment *ga_v;         /* Adjustment for scrolled win and viewport */

    /* Create box for file name entries */
    gw_entry_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    rd_data->names->file_box = gw_entry_box;

    for (uint_fast32_t i = 0; i < n_files; ++i) {
        if (g_file_query_exists (files[i], FALSE)) {
            rfnames_add_gfile_to_file_box (rd_data->names, files[i]);
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
 * @param[in,out] rd_data      RenData object with file list and settings
 * @return        none
 */
static void
create_window (GtkWidget        **window,
               GtkApplication    *application,
               RenData           *rd_data)
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
                      G_CALLBACK (event_win_key_press), rd_data);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Application shutdown signal.
 *
 * @param[in]  application  GtkApplication item
 * @param[out] rd_data      RenData object with file list and settings
 * @return     none
 */
static void
shutdown ([[maybe_unused]] GtkApplication *application,
          RenData        *rd_data)
{
    rconfig_save (rd_data);
    rendata_free (rd_data);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Open files in application signal.
 *
 * @param[in,out] application  GtkApplication item
 * @param[in,out] files        An array of GFiles to open
 * @param[in]     n_files      The length of the files array
 * @param[in]     hint         A hint (or ""), but never nullptr
 * @param[in,out] rd_data      RenData object with file list and settings
 * @return        none
 */
static void
open (GtkApplication  *application,
      GFile          **files,
      int              n_files,
      [[maybe_unused]] const char *hint,
      RenData         *rd_data)
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
    create_file_name_entries (files , (uint_fast32_t) n_files, rd_data,
                              &gw_entry_box);
    /* Create main window and set properties */
    create_window (&window, application, rd_data);
    create_toolbar (&gw_toolbar, rd_data);
    create_upcase_lowercase_box (&gw_uplc_box, rd_data);
    create_spaces_to_underscores_box (&gw_undersc_box, rd_data);
    create_apply_to_names_ext_box (&gw_apply_box, rd_data);
    create_replace_str_with_str_box (&gw_replace_str_box, rd_data);
    create_delete_chars_box (&gw_del_str_box, rd_data);
    create_insert_string_box (&gw_ins_str_box, rd_data);
    create_overwrite_string_box (&gw_ovr_str_box, rd_data);
    create_number_string_box (&gw_number_box, rd_data);

    /* OK, Close box */
    create_rename_close_exit_box (&gw_okcl_box, rd_data);

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

    /* Set focus on first entry if file count is greater than 0 */
    if (rfnames_get_cnt (rendata_get_rfnames (rd_data)) > 0) {
        gtk_widget_grab_focus (rd_data->names->rf_items[0]->entry);
    }
    gtk_widget_show_all (window);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Application activate signal.
 *
 * @param[in,out] application  GtkApplication item
 * @param[in,out] rd_data      RenData object with file list and settings
 * @return        none
 */
static void
activate (GtkApplication *application,
          RenData        *rd_data)
{
    open (application, nullptr, 0, nullptr, rd_data);
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
    RenData        *rd_data;
    int             status;

    rd_data = rendata_new ();
    rconfig_load (rd_data);

    app = gtk_application_new ("org.nongnu.SmallFileRenamer",
                               G_APPLICATION_HANDLES_OPEN);

    g_signal_connect (app, "shutdown", G_CALLBACK (shutdown), rd_data);
    g_signal_connect (app, "activate", G_CALLBACK (activate), rd_data);
    g_signal_connect (app, "open",     G_CALLBACK (open),     rd_data);

    g_set_application_name (APP_NAME);

    status = g_application_run (G_APPLICATION (app), argc, argv);

    g_object_unref (app);

    return status;
}
/*----------------------------------------------------------------------------*/


