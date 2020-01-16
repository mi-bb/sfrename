/** 
 *  _____           _ _    _____ _ _        _____                           
 * |   __|_____ ___| | |  |   __|_| |___   | __  |___ ___ ___ _____ ___ ___ 
 * |__   |     | .'| | |  |   __| | | -_|  |    -| -_|   | .'|     | -_|  _|
 * |_____|_|_|_|__,|_|_|  |__|  |_|_|___|  |__|__|___|_|_|__,|_|_|_|___|_|  
 *                                                                          
 * @file  sfrename.c
 * @copyright Copyright (C) 2019-2020 Michal Babik
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
 * @date December 9, 2019
 *
 * @version 1.1.6
 *
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <gtk/gtk.h>
#include <gio/gio.h>
#include <gdk/gdkkeysyms.h>
#include "strfn.h"
#include "rendata.h"
#include "namefn.h"
#include "defs.h"
/*----------------------------------------------------------------------------*/
/** 
 * @struct RFiles
 *
 * @brief  Basic program data structure
 *
 * @var    RFiles::entry
 * @briref List with GtkEntry widgets for file names
 *
 * @var    RFiles::rd_data
 * @briref Structure with file rename data
 */ 
typedef struct
RFiles {
    GtkWidget **entry;
    RenData     rd_data;
} RFiles;

/*----------------------------------------------------------------------------*/
/**
 * @brief  RFiles initialization.
 *
 * @param[out] r_files Pointer to RFiles with all file names and settings
 * @return     none
 */
static void
rfiles_init (RFiles *r_files)
{
    rendata_init (&r_files->rd_data);

    r_files->entry = NULL;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free allocated memory.
 *
 * @param[in,out] r_files Pointer to RFiles with all file names and settings
 * @return        none
 */
static void
rfiles_free (RFiles *r_files)
{
    rendata_free (&r_files->rd_data);

    if (r_files->entry != NULL)
        g_free (r_files->entry);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if file can be renamed and rename it.
 *
 * @param[in] old_name Old file name
 * @param[in] new_name New file name
 * @return    Renaming status
 */
static uint8_t
file_check_and_rename (const char *old_name,
                       const char *new_name)
{
    uint8_t ui_res = 0;

    /* Check if new file name is different than old */
    if (strcmp (old_name, new_name) != 0) {
        /* Check if file with new file name don't exist */
        if (access (new_name, F_OK) != 0) {
            /* Renaming file */
            if (rename (old_name, new_name) == 0)
                ui_res = REN_OK;      // OK
            else
                ui_res = REN_NOT_REN; // Could not rename
        }
        else
            ui_res = REN_EXISTS;      // File exists
    }
    else
        ui_res = REN_NC;              // No chgange
    return ui_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Checks if the new entry text is equal to the previous value and
 *         changes it to a new value if it is not.
 *
 * @param[in,out] widget  Pointer to entry widget
 * @param[in]     new_str Pointer to a new entry value
 * @return        none
 */
static void
entry_check_and_update (GtkWidget  *widget,
                        const char *new_str)
{
    const char *en = gtk_entry_get_text (GTK_ENTRY (widget));

    if (strcmp (new_str, en) != 0)
        gtk_entry_set_text (GTK_ENTRY (widget), new_str);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Apply changes to all file names in buffer and update entries.
 *
 * @param[in,out] r_files Pointer to RFiles with all file names and settings
 * @return        none
 */
static void
file_names_update_changes (RFiles *r_files)
{
    for (uint16_t i = 0; i < r_files->rd_data.names.cnt; ++i) {
        /* clear file name */
        memset (r_files->rd_data.names.s_new[i], '\0', FN_LEN+1);

        /* set old name as tooltip */
        gtk_widget_set_tooltip_text (r_files->entry[i],
                                     r_files->rd_data.names.s_org[i]);

        /* copy original name to new to process */
        strcpy (r_files->rd_data.names.s_new[i],
                r_files->rd_data.names.s_org[i]);

        name_to_upcase_lowercase (&r_files->rd_data, i);
        name_spaces_underscores (&r_files->rd_data, i);
        name_delete_chars (&r_files->rd_data, i);
        name_replace_strings (&r_files->rd_data, i);
        name_insert_string (&r_files->rd_data, i);
        name_overwrite_string (&r_files->rd_data, i);
        name_number_string (&r_files->rd_data, i);

        entry_check_and_update (r_files->entry[i],
                                r_files->rd_data.names.s_new[i]);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get active RadioButton in a group of RadioButton items.
 *
 * @param[in] radiob  Pointer to RadioButton
 * @return    Returns Active Radiobutton index or -1 if something went wrong
 * @retval    -1      Something went wrong
 * @retval    >=0     Active RadioButton index
 */
static int8_t
get_radio_active (GtkRadioButton *radiob)
{
    int8_t          i        = -1;   // active RadioButton index
    GtkRadioButton *tmp_butt = NULL; // temp RadioButton
    GSList         *tmp_list = NULL; // get RadioButton group list

    tmp_list = gtk_radio_button_get_group (radiob);

    while (tmp_list != NULL) {
        ++i;
        tmp_butt = tmp_list->data; // getting current list value (RadioButton)
        tmp_list = tmp_list->next;

        if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (tmp_butt)))
            break;
    }
    tmp_butt = NULL; // nulling temp button
    return i;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Destroy program window event.
 *
 * @param[in] widget     The object which received the signal
 * @param[in] data       User data set when the signal handler was connected
 * @return    none
 */
static void
event_close (GtkWidget *widget,
             gpointer   data __attribute__ ((unused)))
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
 * @param[in,out] r_files Pointer to RFiles with all file names and settings
 * @param[in]     widget  Pointer to button widget
 * @return        none
 */
static void
event_click_rename (GtkWidget *widget,
                    RFiles    *r_files)
{
    const char *a            = NULL;
    uint8_t     ui_renamed   = 0;    // Renaming result
    uint8_t     ui_ren_count = 0;    // Number of renamed files

    for (int i = 0; i < r_files->rd_data.names.cnt; ++i) {
        a = gtk_entry_get_text (GTK_ENTRY (r_files->entry[i]));

        ui_renamed = file_check_and_rename (r_files->rd_data.names.s_org[i], a);

        switch (ui_renamed) {

            case REN_OK:
                printf ("File: %s renamed to: %s\n",
                        r_files->rd_data.names.s_org[i], a);

                /* copy new name to original in buffer */
                strcpy (r_files->rd_data.names.s_org[i], a); 
                ++ui_ren_count;
                break;

            case REN_NC:
                printf ("No change in file: %s\n",
                        r_files->rd_data.names.s_org[i]);
                break;

            case REN_NOT_REN:
                printf ("File: %s could not be renamed\n",
                        r_files->rd_data.names.s_org[i]);
                break;

            case REN_EXISTS:
                printf ("File: %s already exists\n", a);
                break;

            default:
                break;
        }
        if (ui_renamed != REN_OK && ui_renamed != REN_NC) {
            /* Revert old file names to new */
            strcpy (r_files->rd_data.names.s_new[i],
                    r_files->rd_data.names.s_org[i]);

            /* Update file name in entry */
            entry_check_and_update (r_files->entry[i],
                                    r_files->rd_data.names.s_new[i]);
        }
    }
    printf ("Renamed %d files of %d\n",
            ui_ren_count, r_files->rd_data.names.cnt);

    /* exit application if "Exit after rename" checkbox was selected */
    if (r_files->rd_data.renexit)
        event_close (widget, NULL);
    else
        file_names_update_changes (r_files);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Insert string text position in SpinButton changed.
 *
 * Copies SpinButton value to RFiles and calls to update file name changes if
 * insert string text is not empty.
 *
 * @param[in]     sp_button Pointer to SpinButton widget
 * @param[in,out] r_files   Pointer to RFiles with all file names and settings
 * @return        none
 */
static void
event_insert_pos_changed (GtkSpinButton *sp_button,
                          RFiles        *r_files)
{
    r_files->rd_data.ins.pos =
        (uint8_t) gtk_spin_button_get_value_as_int (sp_button);

    if (strcmp (r_files->rd_data.ins.s_text, "") != 0)
        file_names_update_changes (r_files);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Insert string text in Entry changed.
 *
 * Copies string to insert to RFiles and calls to update file
 * name changes.
 *
 * @param[in]     widget    Pointer to Entry widget
 * @param[in,out] r_files   Pointer to RFiles with all file names and settings
 * @return        none
 */
static void
event_insert_string_entry_changed (GtkWidget *widget,
                                   RFiles    *r_files)
{
    const char *s_en = NULL;

    s_en = gtk_entry_get_text (GTK_ENTRY (widget));

    memset (r_files->rd_data.ins.s_text, '\0',
            sizeof (r_files->rd_data.ins.s_text));

    memcpy (r_files->rd_data.ins.s_text, s_en, get_valid_length (s_en, FN_LEN));

    file_names_update_changes (r_files);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Overwrite string text position in SpinButton changed.
 *
 * Copies SpinButton value to RFiles and calls to update file name changes if
 * overwrite string text is not empty.
 *
 * @param[in]     sp_button Pointer to SpinButton widget
 * @param[in,out] r_files   Pointer to RFiles with all file names and settings
 * @return        none
 */
static void
event_overwrite_pos_changed (GtkSpinButton *sp_button,
                             RFiles        *r_files)
{
    r_files->rd_data.overwrite.pos =
        (uint8_t) gtk_spin_button_get_value_as_int (sp_button);

    if (strcmp (r_files->rd_data.overwrite.s_text, "") != 0)
        file_names_update_changes (r_files);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Overwrite string text in Entry changed.
 *
 * Copies string to overwrite to RFiles and calls to update file
 * name changes.
 *
 * @param[in]     widget    Pointer to Entry widget
 * @param[in,out] r_files   Pointer to RFiles with all file names and settings
 * @return        none
 */
static void
event_overwrite_string_entry_changed (GtkWidget *widget,
                                      RFiles    *r_files)
{
    const char *s_en = NULL;

    s_en = gtk_entry_get_text (GTK_ENTRY (widget));

    memset (r_files->rd_data.overwrite.s_text, '\0',
            sizeof (r_files->rd_data.overwrite.s_text));

    memcpy (r_files->rd_data.overwrite.s_text, s_en,
            get_valid_length (s_en, FN_LEN));

    file_names_update_changes (r_files);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Delete char number of chars to delete in SpinButton changed.
 *
 * Reads SpinButon value of chars to delete and calls to update file name
 * changes.
 *
 * @param[in]     sp_button Pointer to SpinButton
 * @param[in,out] r_files   Pointer to RFiles with all file names and settings
 * @return        none
 */
static void
event_delete_cnt_changed (GtkSpinButton *sp_button,
                          RFiles        *r_files)
{
    r_files->rd_data.del.cnt =
        (uint8_t) gtk_spin_button_get_value_as_int (sp_button);

    file_names_update_changes (r_files);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Delete char position velue in SpinButton changed.
 *
 * Reads SpinButon value and if number of chars to delete is greater then 0
 * it calls to update file name changes.
 *
 * @param[in]     sp_button Pointer to SpinButton
 * @param[in,out] r_files   Pointer to RFiles with all file names and settings
 * @return        none
 */
static void
event_delete_pos_changed (GtkSpinButton *sp_button,
                          RFiles        *r_files)
{
    r_files->rd_data.del.pos =
        (uint8_t) gtk_spin_button_get_value_as_int (sp_button);

    if (r_files->rd_data.del.cnt > 0)
        file_names_update_changes (r_files);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Upper/lower case RadioButton changed.
 *
 * Reads which RadioButton is selected and runs file name update changes
 * function.
 *
 * @param[in,out] r_files Pointer to RFiles with all file names and settings
 * @param[in]     radiob  Pointer to RadioButton
 * @return        none
 */
static void
event_case_radio_active (GtkRadioButton *radiob,
                         RFiles         *r_files)
{
    static uint8_t dbl = 0; // to remove double toggling

    if (dbl ^= 1) {
        r_files->rd_data.uplo = get_radio_active (radiob);
        file_names_update_changes (r_files);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Space/underscore replace RadioButton changed.
 *
 * Reads which RadioButton is selected and runs file name update changes
 * function.
 *
 * @param[in,out] r_files Pointer to RFiles with all file names and settings
 * @param[in]     radiob  Pointer to RadioButton
 * @return        none
 */
static void
event_spaces_radio_active (GtkRadioButton *radiob,
                           RFiles         *r_files)
{
    static uint8_t dbl = 0; // to remove double toggling

    if (dbl ^= 1) {
        r_files->rd_data.spaces = get_radio_active (radiob);
        file_names_update_changes (r_files);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Changed "replace from" value in replace strings section.
 *
 * Copies "replace from" value to RFiles and runs file name update changes
 * function.
 *
 * @param[in,out] r_files Pointer to RFiles with all file names and settings
 * @param[in]     widget  Pointer to entry widget with changed value
 * @return        none
 */
static void
event_replace_from_entry_changed (GtkWidget *widget,
                                  RFiles    *r_files)
{
    const char *s_en = gtk_entry_get_text (GTK_ENTRY(widget));

    memset (r_files->rd_data.replace.s_from, '\0',
            sizeof (r_files->rd_data.replace.s_from));

    memcpy (r_files->rd_data.replace.s_from,
            s_en, get_valid_length (s_en, FN_LEN));

    file_names_update_changes (r_files);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Changed "replace to" value in replace strings section.
 *
 * Copies "replace to" value to RFiles, checks if "replace from" string is not
 * empty and calls update changes function.
 *
 * @param[in,out] r_files Pointer to RFiles with all file names and settings
 * @param[in]     widget  Pointer to entry widget with changed value
 * @return        none
 */
static void
event_replace_to_entry_changed (GtkWidget *widget,
                                RFiles    *r_files)
{
    const char *s_en = gtk_entry_get_text (GTK_ENTRY(widget));

    memset (r_files->rd_data.replace.s_to, '\0',
            sizeof (r_files->rd_data.replace.s_to));

    memcpy (r_files->rd_data.replace.s_to, s_en,
            get_valid_length (s_en, FN_LEN));

    if (strcmp (r_files->rd_data.replace.s_from, "") != 0)
        file_names_update_changes (r_files);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Changed state of RadioButton responsible for applying changes to
 *         file name, file extension or both.
 *
 * Reads which RadioButton is selected, rename files in file name buffer,
 * check for strings replacement and update entries if needed.
 *
 * @param[in,out] r_files Pointer to RFiles with all file names and settings
 * @param[in]     radiob  Pointer to RadioButton
 * @return none
 */
static void
event_apply_radio_active (GtkRadioButton *radiob,
                          RFiles         *r_files)
{
    static uint8_t dbl = 0; // to remove double toggling

    if (dbl ^= 1) {
        /* read apply to names/ext active RadioButton */
        r_files->rd_data.applyto = get_radio_active (radiob);

        file_names_update_changes (r_files);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Number names CheckBox state changed.
 *
 * @param[out] r_files Pointer to RFiles with all file names and settings
 * @param[in]  toggleb Pointer to ToggleButton
 * @return     none
 */
static void
event_toggle_number_names (GtkToggleButton *toggleb,
                           RFiles          *r_files)
{
    r_files->rd_data.number.opt =
        (uint8_t) gtk_toggle_button_get_active (toggleb);

    file_names_update_changes (r_files);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Numbering strings start value in SpinButton changed.
 *
 * @param[in]     sp_button Pointer to SpinButton
 * @param[in,out] r_files   Pointer to RFiles with all file names and settings
 * @return        none
 */
static void
event_number_start_changed (GtkSpinButton *sp_button,
                            RFiles        *r_files)
{
    r_files->rd_data.number.start = 
        (uint32_t) gtk_spin_button_get_value_as_int (sp_button);

    if (r_files->rd_data.number.opt)
        file_names_update_changes (r_files);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Numbering strings position in string SpinButton changed.
 *
 * @param[in]     sp_button Pointer to SpinButton
 * @param[in,out] r_files   Pointer to RFiles with all file names and settings
 * @return        none
 */
static void
event_number_pos_changed (GtkSpinButton *sp_button,
                            RFiles        *r_files)
{
    r_files->rd_data.number.pos =
        (uint8_t) gtk_spin_button_get_value_as_int (sp_button);

    if (r_files->rd_data.number.opt)
        file_names_update_changes (r_files);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Exit after rename CheckBox state changed.
 *
 * Sets renexit value in r_files structure based on the value of ChackBox.
 *
 * @param[out] r_files Pointer to RFiles with all file names and settings
 * @param[in]  toggleb Pointer to ToggleButton
 * @return     none
 */
static void
event_toggle_rename_exit (GtkToggleButton *toggleb,
                          RFiles          *r_files)
{
    r_files->rd_data.renexit = (int8_t) gtk_toggle_button_get_active (toggleb);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  React to key pressed in main window.
 *
 * @param[in] widget    The object which received the signal
 * @param[in] event     The event which triggered this signal
 * @param[in] user_data User data set when the signal handler was connected
 */
static gboolean
event_win_key_press (GtkWidget   *widget,
                     GdkEventKey *event,
                     gpointer     user_data)
{
    /* Catch Esc key in main window and exit */
    if (event->keyval == GDK_KEY_Escape)
        event_close (widget, NULL);

    /* Catch Enter and preform rename */
    if (event->keyval == GDK_KEY_Return) {
        RFiles *r_files = user_data;

        event_click_rename (widget, r_files);
    }
    return FALSE;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Creates upcase / lowercase container.
 *
 * @param[out]    gw_container Pointer to container pointer
 * @param[in,out] r_files      Pointer to RFiles with all file names and settings
 * @return        none
 */
static void
create_upcase_lowercase_box (GtkWidget **gw_container,
                             RFiles     *r_files)
{
    GtkWidget *gw_ncc; // No change radio button
    GtkWidget *gw_lcc; // Lovercase radio button
    GtkWidget *gw_upc; // Uppercase radio button

    gw_ncc = gtk_radio_button_new_with_label (NULL, "No change");
    gw_lcc = gtk_radio_button_new_with_label_from_widget (
             GTK_RADIO_BUTTON (gw_ncc), "To lowercase");
    gw_upc = gtk_radio_button_new_with_label_from_widget (
             GTK_RADIO_BUTTON (gw_ncc), "To uppercase");

    g_signal_connect (G_OBJECT(gw_ncc),
            "toggled", G_CALLBACK (event_case_radio_active), r_files);
    g_signal_connect (G_OBJECT (gw_lcc),
            "toggled", G_CALLBACK (event_case_radio_active), r_files);
    g_signal_connect (G_OBJECT (gw_upc),
            "toggled", G_CALLBACK (event_case_radio_active), r_files);

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
 * @param[out]    gw_container Pointer to container pointer
 * @param[in,out] r_files      Pointer to RFiles with all file names and
 *                             settings
 * @return        none
 */
static void
create_spaces_to_underscores_box (GtkWidget **gw_container,
                                  RFiles     *r_files)
{
    GtkWidget *gw_sptounc; // No change radio button
    GtkWidget *gw_sptou;   // Space to unserscore radio button
    GtkWidget *gw_utosp;   // Underscore to space radio button

    gw_sptounc = gtk_radio_button_new_with_label (NULL, "No change");
    gw_sptou   = gtk_radio_button_new_with_label_from_widget (
                 GTK_RADIO_BUTTON (gw_sptounc), "Space to underscore");
    gw_utosp   = gtk_radio_button_new_with_label_from_widget(
                 GTK_RADIO_BUTTON (gw_sptounc), "Underscore to space");

    g_signal_connect (G_OBJECT (gw_sptounc),
            "toggled", G_CALLBACK (event_spaces_radio_active), r_files);
    g_signal_connect (G_OBJECT (gw_sptou),
            "toggled", G_CALLBACK (event_spaces_radio_active), r_files);
    g_signal_connect (G_OBJECT (gw_utosp),
            "toggled", G_CALLBACK (event_spaces_radio_active), r_files);

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
 * @param[out]    gw_container Pointer to container pointer
 * @param[in,out] r_files      Pointer to RFiles with all file names and
 *                             settings
 * @return        none
 */
static void
create_apply_to_names_ext_box (GtkWidget **gw_container,
                               RFiles     *r_files)
{
    GtkWidget *gw_appne; // Apply to name and extension radio button
    GtkWidget *gw_appn;  // Apply to name only radio button
    GtkWidget *gw_appe;  // Apply to extension only radio button

    gw_appne = gtk_radio_button_new_with_label (NULL, "Apply to name and ext");
    gw_appn  = gtk_radio_button_new_with_label_from_widget (
               GTK_RADIO_BUTTON (gw_appne), "Apply to name");
    gw_appe  = gtk_radio_button_new_with_label_from_widget (
               GTK_RADIO_BUTTON (gw_appne), "Apply to ext");

    g_signal_connect (G_OBJECT (gw_appne),
            "toggled", G_CALLBACK (event_apply_radio_active), r_files);
    g_signal_connect (G_OBJECT (gw_appn),
            "toggled", G_CALLBACK (event_apply_radio_active), r_files);
    g_signal_connect (G_OBJECT (gw_appe),
            "toggled", G_CALLBACK (event_apply_radio_active), r_files);

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
 * @param[out]    gw_container Pointer to container pointer
 * @param[in,out] r_files      Pointer to RFiles with all file names and
 *                             settings
 * @return        none
 */
static void
create_replace_str_with_str_box (GtkWidget **gw_container,
                                 RFiles     *r_files)
{
    GtkWidget *gw_sfrom; // Replace from entry
    GtkWidget *gw_sto;   // Replace to entry
    GtkWidget *gw_lab1;  // Description label
    GtkWidget *gw_lab2;  // Description label

    gw_sfrom = gtk_entry_new ();
    gw_sto   = gtk_entry_new ();

    gtk_entry_set_max_length (GTK_ENTRY (gw_sfrom), FN_LEN);
    gtk_entry_set_width_chars (GTK_ENTRY (gw_sfrom), 6);
    gtk_entry_set_max_length (GTK_ENTRY (gw_sto), FN_LEN);
    gtk_entry_set_width_chars (GTK_ENTRY (gw_sto), 6);

    gw_lab1 = gtk_label_new ("Replace text");
    gw_lab2 = gtk_label_new ("with");

    g_signal_connect (G_OBJECT (gw_sfrom),
            "changed", G_CALLBACK (event_replace_from_entry_changed), r_files);
    g_signal_connect (G_OBJECT (gw_sto),
            "changed", G_CALLBACK (event_replace_to_entry_changed), r_files);

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
 * @param[out]    gw_container Pointer to container pointer
 * @param[in,out] r_files      Pointer to RFiles with all file names and
 *                             settings
 * @return        none
 */
static void
create_delete_chars_box (GtkWidget **gw_container,
                         RFiles     *r_files)
{
    GtkWidget     *gw_del_cnt;     // Delete chars count spin button
    GtkWidget     *gw_del_pos;     // Delete from position spin button
    GtkWidget     *gw_lab;         // Description label
    GtkAdjustment *gw_del_adj_cnt; // Adjustment for spin button
    GtkAdjustment *gw_del_adj_pos; // Adjustment for spin button

    gw_lab         = gtk_label_new ("Delete text:");
    gw_del_adj_cnt = gtk_adjustment_new (0.0, 0.0, FN_LEN, 1.0, 5.0, 0.0);
    gw_del_cnt     = gtk_spin_button_new (gw_del_adj_cnt, 1.0, 0);

    gtk_widget_set_tooltip_text (gw_del_cnt, "Count");

    gw_del_adj_pos = gtk_adjustment_new (0.0, 0.0, FN_LEN, 1.0, 5.0, 0.0);

    gw_del_pos = gtk_spin_button_new (gw_del_adj_pos, 1.0, 0);

    gtk_widget_set_tooltip_text (gw_del_pos, "At position");

    g_signal_connect (G_OBJECT (gw_del_cnt),
            "value-changed", G_CALLBACK (event_delete_cnt_changed), r_files);
    g_signal_connect (G_OBJECT (gw_del_pos),
            "value-changed", G_CALLBACK (event_delete_pos_changed), r_files);

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
 * @param[out]    gw_container Pointer to container pointer
 * @param[in,out] r_files      Pointer to RFiles with all file names and
 *                             settings
 * @return        none
 */
static void
create_insert_string_box (GtkWidget **gw_container,
                          RFiles     *r_files)
{
    GtkWidget     *gw_lab;         // Description label
    GtkWidget     *gw_ins_entry;   // Insert text entry
    GtkWidget     *gw_ins_pos;     // Insert text position spin button
    GtkAdjustment *gw_ins_adj_pos; // Adjustment for spin button

    gw_lab = gtk_label_new ("Insert text:");
    gw_ins_entry = gtk_entry_new ();

    gtk_entry_set_max_length (GTK_ENTRY (gw_ins_entry), FN_LEN);
    gtk_entry_set_width_chars (GTK_ENTRY (gw_ins_entry), 6);

    gw_ins_adj_pos = gtk_adjustment_new (0.0, 0.0, FN_LEN, 1.0, 5.0, 0.0);
    gw_ins_pos = gtk_spin_button_new (gw_ins_adj_pos, 1.0, 0);

    gtk_widget_set_tooltip_text (gw_ins_pos, "At position");

    g_signal_connect (G_OBJECT (gw_ins_pos),
            "value-changed", G_CALLBACK (event_insert_pos_changed), r_files);
    g_signal_connect (G_OBJECT (gw_ins_entry),
            "changed", G_CALLBACK (event_insert_string_entry_changed), r_files);

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
 * @param[out]    gw_container Pointer to container pointer
 * @param[in,out] r_files      Pointer to RFiles with all file names and
 *                             settings
 * @return        none
 */
static void
create_overwrite_string_box (GtkWidget **gw_container,
                             RFiles     *r_files)
{
    GtkWidget     *gw_lab;         // Description label
    GtkWidget     *gw_ovr_entry;   // Insert text entry
    GtkWidget     *gw_ovr_pos;     // Insert text position spin button
    GtkAdjustment *gw_ovr_adj_pos; // Adjustment for spin button

    gw_lab = gtk_label_new ("Overwrite text:");
    gw_ovr_entry = gtk_entry_new ();

    gtk_entry_set_max_length (GTK_ENTRY (gw_ovr_entry), FN_LEN);
    gtk_entry_set_width_chars (GTK_ENTRY (gw_ovr_entry), 6);

    gw_ovr_adj_pos = gtk_adjustment_new (0.0, 0.0, FN_LEN, 1.0, 5.0, 0.0);
    gw_ovr_pos = gtk_spin_button_new (gw_ovr_adj_pos, 1.0, 0);

    gtk_widget_set_tooltip_text (gw_ovr_pos, "At position");

    g_signal_connect (G_OBJECT (gw_ovr_pos),
            "value-changed", G_CALLBACK (event_overwrite_pos_changed), r_files);
    g_signal_connect (G_OBJECT (gw_ovr_entry),
            "changed", G_CALLBACK (event_overwrite_string_entry_changed),
                                   r_files);

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
 * @param[out]    gw_container Pointer to container pointer
 * @param[in,out] r_files      Pointer to RFiles with all file names and
 *                             settings
 * @return        none
 */
static void
create_number_string_box (GtkWidget **gw_container,
                          RFiles     *r_files)
{
    GtkWidget     *gw_check;              // Number names checkbox
    GtkWidget     *gw_num_start_spin;     // Numbering start spin button
    GtkAdjustment *gw_num_start_spin_adj; // Adjustment for spin button
    GtkWidget     *gw_num_pos_spin;       // Numbering start spin button
    GtkAdjustment *gw_num_pos_spin_adj;   // Adjustment for spin button


    gw_check = gtk_check_button_new_with_label ("Number files");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gw_check), FALSE);

    gw_num_start_spin_adj = gtk_adjustment_new (0.0, 0.0, 1000, 1.0, 5.0, 0.0);
    gw_num_start_spin = gtk_spin_button_new (gw_num_start_spin_adj, 1.0, 0);

    gw_num_pos_spin_adj = gtk_adjustment_new (0.0, 0.0, FN_LEN, 1.0, 5.0, 0.0);
    gw_num_pos_spin = gtk_spin_button_new (gw_num_pos_spin_adj, 1.0, 0);

    gtk_widget_set_tooltip_text (gw_num_start_spin, "Start numbering from");
    gtk_widget_set_tooltip_text (gw_num_pos_spin, "Number posistion in name");

    g_signal_connect (G_OBJECT (gw_num_start_spin),
            "value-changed", G_CALLBACK (event_number_start_changed), r_files);
    g_signal_connect (G_OBJECT (gw_num_pos_spin),
            "value-changed", G_CALLBACK (event_number_pos_changed), r_files);
    g_signal_connect (G_OBJECT (gw_check),
            "toggled", G_CALLBACK (event_toggle_number_names), r_files);

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
 * @param[out]    gw_container Pointer to container pointer
 * @param[in,out] r_files      Pointer to RFiles with all file names and
 *                             settings
 * @return        none
 */
static void
create_rename_close_exit_box (GtkWidget **gw_container,
                              RFiles     *r_files)
{
    GtkWidget *gw_but_ok;  // Rename button
    GtkWidget *gw_but_cc;  // Close button
    GtkWidget *gw_renexit; // Exit after rename check button

    gw_but_ok = gtk_button_new_with_label ("Rename");
    gw_but_cc = gtk_button_new_with_label ("Close");

    g_signal_connect (G_OBJECT (gw_but_ok),
            "clicked", G_CALLBACK (event_click_rename), r_files);
    g_signal_connect (G_OBJECT (gw_but_cc),
            "clicked", G_CALLBACK (event_close), NULL);

    gw_renexit = gtk_check_button_new_with_label ("Exit after rename");

    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (gw_renexit), TRUE);

    g_signal_connect (G_OBJECT (gw_renexit),
            "toggled", G_CALLBACK (event_toggle_rename_exit), r_files);

    *gw_container = gtk_grid_new ();

    gtk_grid_set_column_spacing (GTK_GRID (*gw_container), 4);

    gtk_grid_attach (GTK_GRID (*gw_container), gw_but_ok, 0, 0, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (*gw_container),
            gw_but_cc, gw_but_ok, GTK_POS_RIGHT, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (*gw_container),
            gw_renexit, gw_but_cc, GTK_POS_RIGHT, 1, 1);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Creates file entry box, file name entries, set r_files properties.
 *
 * Searches for files in command line passed values, creates file name entries,
 * strings for buffering original and changed file names.
 * Creates container and puts entries in it.
 *
 * @param[in]     files        an array of GFiles
 * @param[in]     n_files      length of files
 * @param[in,out] r_files      Pointer to RFiles with all file names and
 *                             settings
 * @param[out]    gw_container Pointer to entry box container pointer
 * @return        Number of passed file names
 */
static int16_t
create_file_name_entries (GFile     **files,
                          gint        n_files,
                          RFiles     *r_files,
                          GtkWidget **gw_container)
{
    GtkWidget     *gw_entry_box; // Box fo file name Entry fields
    GtkWidget     *gw_vp;        // ViewPort
    GtkAdjustment *ga_h;         // Adjustment for scrolled window and viewport
    GtkAdjustment *ga_v;         // Adjustment for scrolled window and viewport
    uint16_t       ui_cn = 0;    // File names count

    /* Allocate memory for pointers to entries and file name strings */
    r_files->entry     = g_malloc ((size_t) n_files * sizeof (GtkWidget*));
    r_files->rd_data.names.s_org = g_malloc ((size_t) n_files * sizeof (char*));
    r_files->rd_data.names.s_new = g_malloc ((size_t) n_files * sizeof (char*));

    /* Create box for file name entries */
    gw_entry_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 4);

    for (int i = 0; i < n_files; ++i) {

        char *ch_fname = g_file_get_basename (files[i]);

        if (strcmp (ch_fname, "..") == 0) {
            g_free (ch_fname);
            continue;
        }
        if (access (ch_fname, F_OK) == 0) { // check if file exists
            /* Allocate memory for original and new file name strings */
            r_files->rd_data.names.s_org[ui_cn] = g_slice_alloc0 (
                    (FN_LEN + 1) * sizeof (char));
            r_files->rd_data.names.s_new[ui_cn] = g_slice_alloc0 (
                    (FN_LEN + 1) * sizeof (char));

            /* Copy verified file names to original and new file name string */
            strcpy (r_files->rd_data.names.s_org[ui_cn], ch_fname);
            strcpy (r_files->rd_data.names.s_new[ui_cn], ch_fname);

            /* Create entry and set max length to defined file name length */
            r_files->entry[ui_cn] = gtk_entry_new ();

            gtk_entry_set_max_length (GTK_ENTRY (r_files->entry[ui_cn]),
                                      FN_LEN);
            /* Set entry file names */
            gtk_entry_set_text (GTK_ENTRY (r_files->entry[ui_cn]),
                                r_files->rd_data.names.s_org[ui_cn]);
            /* Add entry to the container */
            gtk_box_pack_start (GTK_BOX (gw_entry_box),
                                r_files->entry[ui_cn], FALSE, FALSE, 0);
            ++ui_cn;
        }
        g_free (ch_fname);
    }
    /* Make scrollbars */
    *gw_container = gtk_scrolled_window_new (NULL, NULL);

    ga_h = gtk_scrolled_window_get_hadjustment(
            GTK_SCROLLED_WINDOW (*gw_container));
    ga_v = gtk_scrolled_window_get_vadjustment(
            GTK_SCROLLED_WINDOW (*gw_container));

    gw_vp = gtk_viewport_new (ga_h, ga_v);

    gtk_container_add (GTK_CONTAINER (gw_vp), gw_entry_box);
    gtk_container_add (GTK_CONTAINER (*gw_container), gw_vp);

    /* If file count is smaller than passed arguments, decrease number of
     * entries, original and new file name strings */
    if (ui_cn < n_files) {
        r_files->entry = g_realloc (
                r_files->entry, ui_cn * sizeof (GtkWidget*));
        r_files->rd_data.names.s_org = g_realloc (
                r_files->rd_data.names.s_org, ui_cn * sizeof (char*));
        r_files->rd_data.names.s_new = g_realloc (
                r_files->rd_data.names.s_new, ui_cn * sizeof (char*));
    }
    r_files->rd_data.names.cnt = ui_cn; // set file count value in r_files
    return 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Creates main window, its properties and connects window signals.
 *
 * @param[in,out] window  Pointer to a window pointer
 * @param[in,out] application  Pointer to GtkApplication
 * @param[in]     r_files Pointer to RFiles with all file names and settings
 * @return        none
 */
static void
create_window (GtkWidget        **window,
               GtkApplication    *application,
               RFiles            *r_files)
{
    /* Create window widget */
    *window = gtk_application_window_new (application);

    /* Set window properties */
    gtk_window_set_title (GTK_WINDOW (*window), APP_NAME " v" APP_VER);

    gtk_container_set_border_width (GTK_CONTAINER (*window), 10);
    gtk_window_set_default_size (GTK_WINDOW (*window), WIN_WIDTH, WIN_HEIGHT);
    gtk_window_set_position (GTK_WINDOW (*window), GTK_WIN_POS_CENTER);

    /* Connect window events */
    g_signal_connect (G_OBJECT (*window),
            "key-press-event", G_CALLBACK (event_win_key_press), r_files);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Application startup signal.
 *
 * @param[in,out] application  Pointer to GtkApplication
 * @param[in,out] r_files      Pointer to RFiles with all file names and
 *                             settings
 * @return        none
 */
static void
startup (GtkApplication *application __attribute__ ((unused)),
         RFiles         *r_files)
{
    rfiles_init (r_files);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Application shutdown signal.
 *
 * @param[in,out] application  Pointer to GtkApplication
 * @param[in,out] r_files      Pointer to RFiles with all file names and
 *                             settings
 * @return        none
 */
static void
shutdown (GtkApplication *application __attribute__ ((unused)),
          RFiles         *r_files)
{
    rfiles_free (r_files);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Application activate signal.
 *
 * @param[in,out] application  Pointer to GtkApplication
 * @return        none
 */
static void
activate (GtkApplication *application __attribute__ ((unused)))
{
    printf ("No files to open\n");
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Open files in application signal.
 *
 * @param[in,out] application  Pointer to GtkApplication
 * @param[in,out] files        An array of GFiles to open
 * @param[in]     n_files      The length of the files array
 * @param[in]     hint         A hint (or ""), but never NULL
 * @param[in,out] r_files      Pointer to RFiles with all file names and
 *                             settings
 * @return        none
 */
static void
open (GtkApplication  *application,
      GFile          **files,
      int              n_files,
      const char      *hint __attribute__ ((unused)),
      RFiles          *r_files)
{
    GtkWidget *window;             // Appliation window
    GtkWidget *gw_entry_box;       // File name list widget
    GtkWidget *gw_vbox;            // Main window pack widget
    GtkWidget *gw_uplc_box;        // Upcase / lowercase widget
    GtkWidget *gw_undersc_box;     // Spaces to underscores widget
    GtkWidget *gw_replace_str_box; // Replace text widget
    GtkWidget *gw_del_str_box;     // Delete text widget
    GtkWidget *gw_ins_str_box;     // Insert text widget
    GtkWidget *gw_ovr_str_box;     // Overwrite text widget
    GtkWidget *gw_number_box;      // Number names widget
    GtkWidget *gw_apply_box;       // Apply to names / extenstions widget
    GtkWidget *gw_udusc_box;       // Widget to pack all settings
    GtkWidget *gw_okcl_box;        // Rename / Close widget
    GtkWidget *gw_sep;             // Separator widget

    /* Create file name entries and set r_files properties */
    create_file_name_entries (files , n_files, r_files, &gw_entry_box);
    if (r_files->rd_data.names.cnt < 1) {
        printf ("No files to open\n");
        return;
    }
    /* Create main window and set properties */
    create_window (&window, application, r_files);
    create_upcase_lowercase_box (&gw_uplc_box, r_files);
    create_spaces_to_underscores_box (&gw_undersc_box, r_files);
    create_apply_to_names_ext_box (&gw_apply_box, r_files);
    create_replace_str_with_str_box (&gw_replace_str_box, r_files);
    create_delete_chars_box (&gw_del_str_box, r_files);
    create_insert_string_box (&gw_ins_str_box, r_files);
    create_overwrite_string_box (&gw_ovr_str_box, r_files);
    create_number_string_box (&gw_number_box, r_files);

    /* OK, Close box */
    create_rename_close_exit_box (&gw_okcl_box, r_files);

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

    gtk_box_pack_start (GTK_BOX (gw_vbox),
            gtk_label_new("Enter new file / directory name"), FALSE, FALSE, 0);
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
    gtk_widget_show_all (window);
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
/*----------------------------------------------------------------------------*/
int
main (int argc, char **argv)
{
    GtkApplication  *app;
    int              status;
    RFiles           r_files;

    app = gtk_application_new ("org.nongnu.SmallFileRenamer",
                           G_APPLICATION_HANDLES_OPEN);

    g_signal_connect (app, "startup", G_CALLBACK (startup), &r_files);
    g_signal_connect (app, "shutdown", G_CALLBACK (shutdown), &r_files);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
    g_signal_connect (app, "open", G_CALLBACK (open), &r_files);

    g_set_application_name (APP_NAME);

    status = g_application_run (G_APPLICATION (app), argc, argv);

    g_object_unref (app);

    return status;
}
/*----------------------------------------------------------------------------*/


