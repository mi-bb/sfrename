/** 
 *  _____           _ _    _____ _ _        _____                           
 * |   __|_____ ___| | |  |   __|_| |___   | __  |___ ___ ___ _____ ___ ___ 
 * |__   |     | .'| | |  |   __| | | -_|  |    -| -_|   | .'|     | -_|  _|
 * |_____|_|_|_|__,|_|_|  |__|  |_|_|___|  |__|__|___|_|_|__,|_|_|_|___|_|  
 *                                                                          
 * @file  sfrename.c
 * @copyright Copyright (C) 2019 Michał Bąbik
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
 * @date October 10, 2019
 * @version 1.1.2
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <glib.h>
#include <gtk/gtk.h>
#include <gio/gio.h>
#include <gdk/gdkkeysyms.h>
#include <gmodule.h>

#define FN_LEN 255      //!< Maximum chars in file name string
#define WIN_WIDTH  600  //!< Application window width
#define WIN_HEIGHT 800  //!< Application window height
//------------------------------------------------------------------------------
//  gcc -std=c11 -Wall `pkg-config --cflags gtk+-3.0` -o sfrename sfrename.c
//  `pkg-config --libs gtk+-3.0`
//------------------------------------------------------------------------------
/**
 * @brief  Delete text in name settings
 */
typedef struct
RDelete {
    uint8_t   cnt;               //!< Delete chars count
    uint8_t   pos;               //!< Delete chars starting position
} RDelete;
/**
 * @brief  Insert text in name settings
 */
typedef struct
RInsert {
    char      text[FN_LEN + 1];  //!< String to insert in file name
    uint8_t   pos;               //!< Position to put string
} RInsert;
/**
 * @brief  Replace strings in name settings
 */
typedef struct
RReplace {
    char      from[FN_LEN + 1];  //!< Replace from string
    char      to[FN_LEN + 1];    //!< Replace to string
} RReplace;
/**
 * @brief  File names buffer and entries
 */
typedef struct
RFnames {
    int         cnt;        //!< Count of file names
    char      **org;        //!< Original file names buffer
    char      **new;        //!< New file names buffer
} RFnames;
/** 
 * @brief Basic program data structure
 */ 
typedef struct
RFiles {
    GtkWidget **entry;      //!< Entries with file names
    RFnames     names;      //!< File names, old, new, entries
    RDelete     delete;     //!< Delete chars properties
    RInsert     insert;     //!< Insert text properties
    RReplace    replace;    //!< Replace string with string settings
    uint8_t     uplo;       //!< Upper/lower case option
    uint8_t     spaces;     //!< Spaces/underscores option
    uint8_t     applyto;    //!< Apply to file names/ext or both option
    uint8_t     renexit;    //!< Exit after rename option
} RFiles;
//------------------------------------------------------------------------------
/**
 * @brief  RFiles initialization.
 *
 * @param[out] r_files Pointer to RFiles with all file names and settings
 * @return     none
 */
static void
rfiles_init (RFiles *r_files)
{
    // Setting file list properties
    r_files->uplo = 2;         // upper/lower case setting
    r_files->spaces = 2;       // spaces/underscores setting
    r_files->applyto = 2;      // apply to names/ext/both setting
    r_files->renexit = 1;      // exit after rename setting
    r_files->delete.cnt = 0;   // chars to delete
    r_files->delete.pos = 0;   // delete starting at pos
    r_files->insert.pos = 0;   // insert string at pos
    r_files->names.cnt = 0;    // names count
    r_files->entry = NULL;     // entry list
    r_files->names.org = NULL; // org names list
    r_files->names.new = NULL; // new names list
    // clear replace from and to strings
    memset (r_files->replace.from, 0, sizeof (r_files->replace.from));
    memset (r_files->replace.to, 0, sizeof (r_files->replace.to));
    memset (r_files->insert.text, 0, sizeof (r_files->insert.text));
}
//------------------------------------------------------------------------------
/**
 * @brief  Free allocated memory.
 *
 * @param[in,out] r_files Pointer to RFiles with all file names and settings
 * @return        none
 */
static void
free_stuff (RFiles *r_files)
{
    for (uint16_t j=0; j < r_files->names.cnt; ++j) {
        g_slice_free1 ( (FN_LEN+1) * sizeof (char), r_files->names.org[j]);
        g_slice_free1 ( (FN_LEN+1) * sizeof (char), r_files->names.new[j]);
    }
    if (r_files->names.new != NULL)
        g_free (r_files->names.new);
    if (r_files->names.org != NULL)
        g_free (r_files->names.org);
    if (r_files->entry != NULL)
        g_free (r_files->entry);
}
//------------------------------------------------------------------------------
/**
 * @brief  Check if file can be renamed and rename it.
 *
 * @param[in] old_name Old file name
 * @param[in] new_name New file name
 * @return    Renaming status
 * @retval 0  No change
 * @retval 1  File successfully renamed
 * @retval 2  Could not rename file
 * @retval 3  Such file already exists
 */
static uint8_t
file_check_and_rename (const char *old_name,
                       const char *new_name)
{
    uint8_t ui_res = 0;

    // Check if new file name is different than old
    if (strcmp (old_name, new_name) != 0) {
        // Check if file with new file name don't exist
        if (access (new_name, F_OK) != 0) {
            // Renaming file
            if (rename (old_name, new_name) == 0)
                ui_res = 1; // OK
            else
                ui_res = 2; // Could not rename
        }
        else
            ui_res = 3;     // File exists
    }
    else
        ui_res = 0;         // No chgange
    return ui_res;
}
//------------------------------------------------------------------------------
/**
 * @brief  Get length (not greater than i_max_chars) in bytes to the last utf8
 *         char in string s_str.
 *
 *  Gets length of maximum correct utf8 chars in string. End of string should be
 *  a null character. Returned length can not be greater than i_max_chars,
 *  input string may have any length (not greater that uint16_t range).
 *  Utf8 chars can be multi byte and you can cut it in half with a simple split.
 *
 * @param[in] s_str       Input string
 * @param[in] i_max_chars Maximum possible bytes of length
 * @return    Length in bytes of correct chars up to i_max_chars value
 */
static uint16_t
utf8_get_last_char_len (const char *s_str,
                        uint16_t    i_max_chars)
{
    char     *x     = NULL;
    uint16_t  i_len = strlen (s_str);

    while (i_len > i_max_chars) {
        x = (char*) s_str + i_len;
        x = g_utf8_find_prev_char (s_str, x);
        i_len = x-s_str;
    }
    return i_len;
}
//------------------------------------------------------------------------------
/** 
 * @brief  Replace one string with another in a given src_dest string.
 *
 * Function searches in src_dest string for s_fr and replaces it with s_to
 * string. The final result is copied into src_dest string.
 *
 * @param[in,out] src_dest Pointer to source and destination string to change
 * @param[in]     s_fr     Pointer to string with "change from" text
 * @param[in]     s_to     Pointer to string with "change to" text
 * @return        none
 */ 
static void
string_replace_in (char *src_dest,
                   char *s_fr,
                   char *s_to)
{
    char    head[FN_LEN+1]; // temp file name
    char    *srcdst = src_dest; // copy src_dest pointer
    char    *pn     = NULL; // find string pointer
    uint16_t i      = 0;

    memset (head, 0, sizeof (head)); // zeroing file name
    pn = strstr (srcdst, s_fr); // find the first occurence of "replace from"
    if (pn == NULL) return; // exit function if "replace from" was not found
    while (pn != NULL) {  // while there are "replace from" in source string
        for (; srcdst != pn; srcdst++) {
            if (i < FN_LEN)
                head[i++] = *srcdst;
        }
        for (uint16_t j = 0; j < strlen (s_to); ++j) {
            if (i < FN_LEN)
                head[i++] = s_to[j];
        }
        srcdst = pn + strlen (s_fr); // change source pointer to "after found"
        pn = strstr (srcdst, s_fr); // find another "to replace" str in src
    }
    for (; *srcdst; srcdst++) {
        if (i < FN_LEN)
            head[i++] = *srcdst;
    }
    strcpy (src_dest, head); // copy temp string to source / dest string
}
//------------------------------------------------------------------------------
/** 
 * @brief  Detele chars in string.
 *
 * @param[in,out] src_dst Pointer to source and destination string to change
 * @param[in]     ch_cnt  Number of chars to delete
 * @param[in]     ch_pos  Delete starting position
 * @return        none
 */ 
static void
string_delete_chars (char *src_dst,
                     char *ch_cnt,
                     char *ch_pos)
{
    uint16_t  i_cnt  = 0;
    uint16_t  i_pos  = 0;
    uint16_t  i_len  = 0;
    uint16_t  i_olen = 0;
    char     *ch_po  = NULL;
    char     *ch_cn  = NULL;

    i_cnt = atoi (ch_cnt);
    i_pos = atoi (ch_pos);
    i_len = strlen (src_dst);

    if (i_cnt == 0) return;
    if (g_utf8_validate (src_dst, -1, NULL)) {
        i_olen = i_len;
        i_len = g_utf8_strlen (src_dst, -1);
        if (i_cnt > i_len)
            i_cnt = i_len;
        if (i_pos > i_len)
            i_pos = i_len;

        if (i_pos + i_cnt > i_len) {
            if (i_pos == i_len)
                i_pos = i_len - i_cnt;
            else
                i_cnt = i_len - i_pos;
        }
        ch_po = g_utf8_offset_to_pointer (src_dst, i_pos);
        ch_cn = g_utf8_offset_to_pointer (src_dst, i_pos + i_cnt);
        memmove (ch_po, ch_cn, i_olen - i_pos - (ch_cn - ch_po) + 1);
    }
}
//------------------------------------------------------------------------------
/**
 * @brief  Insert string to existing string at specified position.
 *
 * @param[in,out] s_dst  Pointer to source and destination string to change
 * @param[in]     s_src  String to insert
 * @param[in]     ch_pos Position to insert the string
 * @return        none
 */
static void
string_insert_string (char *s_dst,
                      char *s_src,
                      char *ch_pos)
{
    char     *x       = NULL;
    char     *s_tm    = NULL;
    char      s_tmp[FN_LEN+1];
    uint16_t  i_pos   = 0;
    uint16_t  i_srl   = 0;
    uint16_t  i_dsl   = 0;
    uint16_t  i_s_len = 0;

    i_pos   = atoi (ch_pos);
    i_srl   = strlen (s_src);
    i_dsl   = strlen (s_dst);

    if (i_srl == 0) return;
    memset (s_tmp, 0, sizeof (s_tmp));
    if (g_utf8_validate (s_dst, -1, NULL)) {
        i_s_len = utf8_get_last_char_len (s_src, FN_LEN - i_dsl);
        x = g_utf8_offset_to_pointer (s_dst, i_pos);
        i_pos = x - s_dst;
        i_dsl = g_utf8_strlen (s_dst, -1);
        if (i_pos > i_dsl)
            i_pos = i_dsl;
        g_utf8_strncpy (s_tmp, s_dst, i_pos);
        strncat (s_tmp, s_src, i_s_len);
        if (i_pos < i_dsl) {
            s_tm = g_utf8_substring (s_dst, 
                    i_pos, g_utf8_strlen (s_dst, -1));
            g_strlcat (s_tmp, s_tm, FN_LEN + 1);
            g_free (s_tm);
        }
        g_strlcpy (s_dst, s_tmp, FN_LEN + 1);
    }
}
//------------------------------------------------------------------------------
/**
 * @brief  Convert all chars in a given string to lower case.
 *
 * @param[in,out] src_dst Pointer to str with text to change
 * @param         unused1 Only for compatibility with process_filename function
 * @param         unused2 Only for compatibility with process_filename function
 * @return        none
 */
static void
string_to_lower (char *src_dst,
                 char *unused1,
                 char *unused2)
{
    char *ts = NULL;

    if (g_utf8_validate (src_dst, -1, NULL)) {
        ts = g_utf8_strdown (src_dst, -1);
        g_utf8_strncpy (src_dst, ts, g_utf8_strlen (ts, -1));
        g_free (ts);
    }
}
//------------------------------------------------------------------------------
/**
 * @brief  Convert all chars in a given string to upper case.
 *
 * @param[in,out] src_dst Pointer to str with text to change
 * @param         unused1 Only for compatibility with process_filename function
 * @param         unused2 Only for compatibility with process_filename function
 * @return        none
 */
static void
string_to_upper (char *src_dst,
                 char *unused1,
                 char *unused2)
{
    char *ts = NULL;

    if (g_utf8_validate (src_dst, -1, NULL)) {
        ts = g_utf8_strup(src_dst, -1);
        g_utf8_strncpy (src_dst, ts, g_utf8_strlen (ts, -1));
        g_free (ts);
    }
}
//------------------------------------------------------------------------------
/**
 * @brief  Find file name and extension and store in appropriate strings.
 *
 * Function finds file extension by searching for a first from right comma sign
 * and copy what's left before it to name and right after comma to ext.
 *
 * @param[in]  f_name_ext Pointer to string containing file name with extension
 *                        to process
 * @param[out] f_name     Destination pointer to file name only string
 * @param[out] f_ext      Destination pointer to file extension string
 * @return     none
 */
static void
string_extract_name_ext (const char *f_name_ext,
                         char       *f_name,
                         char       *f_ext)
{
    const char *pn = NULL;

    pn = strrchr (f_name_ext, '.'); // find first dot from right

    // if file is hidden or has no ext copy whole src name as f_name
    if ((pn == NULL) || (pn == f_name_ext))
        strcpy (f_name, f_name_ext); 
    else { // should be file name with ext
        strcpy (f_ext, pn); // copy extension to f_ext
        strncpy (f_name, f_name_ext, pn - f_name_ext); // copy name to f_name
    }
}
//------------------------------------------------------------------------------
/**
 * @brief  Join file name with its extension.
 *
 * @param[out] f_name_ext Pointer to string where file name and extension will
 *                        be written
 * @param[in]  f_name     Destination pointer to file name string
 * @param[in]  f_ext      Destination pointer to file extension string
 * @return     none
 */
static void
string_combine_name_ext (char *f_name_ext,
                         char *f_name,
                         char *f_ext)
{
    uint16_t i_lc = 0;

    memset (f_name_ext, 0, FN_LEN + 1);

    if (strcmp (f_ext, "") != 0) { // estenstion present
        i_lc = utf8_get_last_char_len (f_name, FN_LEN - strlen(f_ext));
        strncpy (f_name_ext, f_name, i_lc);
        strcat (f_name_ext, f_ext); // append extension to source/result string
    }
    else { // no extenstion
        strncpy (f_name_ext, f_name, utf8_get_last_char_len (f_name, FN_LEN));
    }
}
//------------------------------------------------------------------------------
/**
 * @brief  Function to process file name str using a pointed function.
 *
 * Function checks whether it should perform change whole file name, its name
 * or ext and and calls on it passed as value "fun" function.
 *
 * @param[in]     fun     Pointer to a string change function 
 * @param[in,out] src_dst Pointer to processed string
 * @param[in]     s_fr    Pointer to string with "change from" text
 * @param[in]     s_to    Pointer to string with "change to" text
 * @param[in]     ne      Value tells to change file name, ext or both
 * @return        none
 */
static void
string_process_filename (void        (*fun) (char*, char*, char*),
                         char         *src_dst,
                         char         *s_fr,
                         char         *s_to,
                         const uint8_t ne)
{
    char f_name[FN_LEN + 1]; // temp name
    char f_ext[50];          // lets hope there are no larger extensions

    if (ne == 2)
        (*fun) (src_dst, s_fr, s_to); // change text in name and ext
    else { // change text in name or ext
        memset (f_name, 0, sizeof (f_name)); // zeroing file name
        memset (f_ext, 0, sizeof (f_ext)); // zeroing file ext
        // get name and ext to separate strings
        string_extract_name_ext (src_dst, f_name, f_ext);
        if (ne == 1) (*fun) (f_name, s_fr, s_to); // change name only
        if (ne == 0) (*fun) (f_ext,  s_fr, s_to); // change ext only
        string_combine_name_ext (src_dst, f_name, f_ext); // join name and ext
    }
}
//------------------------------------------------------------------------------
/**
 * @brief  Delete chars in specified file name.
 *
 * Prepares data and calls delete chars in string function.
 *
 * @param[in,out] r_files Pointer to RFiles with all file names and settings
 * @param[in]     i       Index of file name to change
 * @return        none
 */
static void
name_delete_chars (RFiles        *r_files,
                   const uint16_t i)
{
    char tmp1[10];
    char tmp2[10];

    memset (tmp1, 0, sizeof (tmp1)); // zeroing tmp1
    memset (tmp2, 0, sizeof (tmp2)); // zeroing tmp2

    if (r_files->delete.cnt == 0) return; // exit if no chars to delete
    sprintf (tmp1, "%d", r_files->delete.cnt);
    sprintf (tmp2, "%d", r_files->delete.pos);
    string_process_filename (string_delete_chars,
            r_files->names.new[i], tmp1, tmp2, r_files->applyto);
}
//------------------------------------------------------------------------------
/**
 * @brief  Insert string in specified file name at some position.
 *
 * Prepares data and calls insert string into string function.
 *
 * @param[in,out] r_files Pointer to RFiles with all file names and settings
 * @param[in]     i       Index of file name to change
 * @return        none
 */
static void
name_insert_string (RFiles        *r_files,
                    const uint16_t i)
{
    char tmp[10];

    memset (tmp, 0, sizeof (tmp)); // zeroing tmp

    if (strcmp (r_files->insert.text, "") == 0) return;
    sprintf (tmp, "%d", r_files->insert.pos);
    string_process_filename (string_insert_string,
            r_files->names.new[i], r_files->insert.text, tmp, r_files->applyto);
}
//------------------------------------------------------------------------------
/**
 * @brief  Change specified file name to upcase or lowercase.
 *
 * @param[in,out] r_files Pointer to RFiles with all file names and settings
 * @param[in]     i       Index of file name to change
 * @return        none
 */
static void
name_to_upcase_lowercase (RFiles        *r_files,
                          const uint16_t i)
{
    // to uppercase
    if (r_files->uplo == 0)
        string_process_filename (string_to_upper,
                r_files->names.new[i], NULL, NULL, r_files->applyto);
    // to lowercase
    if (r_files->uplo == 1)
        string_process_filename (string_to_lower,
                r_files->names.new[i], NULL, NULL, r_files->applyto);
}
//------------------------------------------------------------------------------
/**
 * @brief  Replace spaces to underscores or vice versa in specified file name to
 *         other.
 *
 * @param[in,out] r_files Pointer to RFiles with all file names and settings
 * @param[in]     i       Index of file name to change
 * @return        none
 */
static void
name_spaces_underscores (RFiles        *r_files,
                         const uint16_t i)
{
    // underscores to spaces
    if (r_files->spaces == 0)
        string_process_filename (string_replace_in,
                r_files->names.new[i], "_", " ", r_files->applyto);
    // spaces to underscores
    if (r_files->spaces == 1) string_process_filename (string_replace_in,
            r_files->names.new[i], " ", "_", r_files->applyto);
}
//------------------------------------------------------------------------------
/**
 * @brief  Replace string in specified file name to other.
 *         If "replace from" string is not empty, perform string replace.
 *
 * @param[in,out] r_files Pointer to RFiles with all file names and settings
 * @param[in]     i       Index of file name to change
 * @return        none
 */
static void
name_replace_strings (RFiles        *r_files,
                      const uint16_t i)
{
    if (strcmp (r_files->replace.from, "") != 0)
        string_process_filename (string_replace_in, r_files->names.new[i],
                r_files->replace.from, r_files->replace.to, r_files->applyto);
}
//------------------------------------------------------------------------------
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
    const char *en = NULL;

    en = gtk_entry_get_text (GTK_ENTRY (widget));

    if (strcmp (new_str, en) != 0)
        gtk_entry_set_text (GTK_ENTRY (widget), new_str);
}
//------------------------------------------------------------------------------
/**
 * @brief  Apply changes to all file names in buffer and update entries.
 *
 * @param[in,out] r_files Pointer to RFiles with all file names and settings
 * @return        none
 */
static void
file_names_update_changes (RFiles *r_files)
{
    for (uint16_t i = 0; i < r_files->names.cnt; ++i) {
        // clear file name
        memset (r_files->names.new[i], 0, FN_LEN+1);
        // set old name as tooltip
        gtk_widget_set_tooltip_text (r_files->entry[i], r_files->names.org[i]);
        // copy original name to new to process
        strcpy (r_files->names.new[i], r_files->names.org[i]);
        name_to_upcase_lowercase (r_files, i);
        name_spaces_underscores (r_files, i);
        name_delete_chars (r_files, i);
        name_replace_strings (r_files, i);
        name_insert_string (r_files, i);
        entry_check_and_update (r_files->entry[i], r_files->names.new[i]);
    }
}
//------------------------------------------------------------------------------
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
    while (tmp_list != NULL) { // iterating through RadioButton list
        ++i; // increment index of active RadioButton
        tmp_butt = tmp_list->data; // getting current list value (RadioButton)
        tmp_list = tmp_list->next; // going to next value
        // if current RadioButton is active break the loop
        if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (tmp_butt))) break;
    }
    tmp_butt = NULL; // nulling temp button
    return i;
}
//------------------------------------------------------------------------------
/**
 * @brief  Destroy program window event.
 *
 * @param[in] widget     The object which received the signal
 * @param[in] data       User data set when the signal handler was connected
 * @return    none
 */
static void
event_close (GtkWidget *widget,
             gpointer   data)
{
    GtkWidget *gw_toplevel = gtk_widget_get_toplevel (widget);
    gtk_widget_destroy (gw_toplevel);
}
//------------------------------------------------------------------------------
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
    uint8_t     ui_renamed   = 0;
    uint8_t     ui_ren_count = 0; // Count renamed files

    for (int i = 0; i < r_files->names.cnt; ++i) {
        a = gtk_entry_get_text (GTK_ENTRY (r_files->entry[i]));
        ui_renamed = file_check_and_rename (r_files->names.org[i], a);
        if (ui_renamed == 1) {       // Renamed
            printf ("File: %s renamed to: %s\n", r_files->names.org[i], a);
            // copy new name to original in buffer
            strcpy (r_files->names.org[i], a); 
            ++ui_ren_count;
        }
        if (ui_renamed > 1) {   // File not renamed
            if  (ui_renamed == 2)    // Coud not rename
                printf ("File: %s could not be renamed\n",
                        r_files->names.org[i]);
            if  (ui_renamed == 3)    // file already exists
                printf ("File: %s already exists\n", a);
            // Revert old file names to new
            strcpy (r_files->names.new[i], r_files->names.org[i]);
            // Update file name in entry
            entry_check_and_update (r_files->entry[i],
                    r_files->names.new[i]);
        }
    }
    printf ("Renamed %d files of %d\n", ui_ren_count, r_files->names.cnt);
    // exit application if "Exit after rename" checkbox was selected
    if (r_files->renexit)
        event_close (widget, NULL);
    else
        file_names_update_changes (r_files);
}
//------------------------------------------------------------------------------
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
    r_files->insert.pos = gtk_spin_button_get_value_as_int (sp_button);
    if (strcmp (r_files->insert.text, "") != 0)
        file_names_update_changes (r_files);
}
//------------------------------------------------------------------------------
/**
 * @brief  Insert string text in Entry changed.
 *
 * Copies string to insert into file name to RFiles and calls to update file
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
    memset (r_files->insert.text, 0, sizeof (r_files->insert.text));
    strncpy (r_files->insert.text, s_en, utf8_get_last_char_len (s_en, FN_LEN));
    file_names_update_changes (r_files);
}
//------------------------------------------------------------------------------
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
    r_files->delete.cnt = gtk_spin_button_get_value_as_int (sp_button);
    file_names_update_changes (r_files);
}
//------------------------------------------------------------------------------
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
    r_files->delete.pos = gtk_spin_button_get_value_as_int (sp_button);
    if (r_files->delete.cnt > 0)
        file_names_update_changes (r_files);
}
//------------------------------------------------------------------------------
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
        r_files->uplo = get_radio_active (radiob);
        file_names_update_changes (r_files);
    }
}
//------------------------------------------------------------------------------
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
        r_files->spaces = get_radio_active (radiob);
        file_names_update_changes (r_files);
    }
}
//------------------------------------------------------------------------------
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
    const char *s_en = NULL;

    s_en = gtk_entry_get_text (GTK_ENTRY(widget));
    memset (r_files->replace.from, 0, sizeof (r_files->replace.from));
    strncpy (r_files->replace.from,
            s_en, utf8_get_last_char_len (s_en, FN_LEN));
    file_names_update_changes (r_files);
}
//------------------------------------------------------------------------------
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
    const char *s_en = NULL;
    
    s_en = gtk_entry_get_text (GTK_ENTRY(widget));
    memset (r_files->replace.to, 0, sizeof (r_files->replace.to));
    strncpy (r_files->replace.to, s_en, utf8_get_last_char_len (s_en, FN_LEN));
    if (strcmp (r_files->replace.from, "") != 0)
        file_names_update_changes (r_files);
}
//------------------------------------------------------------------------------
/**
 * @brief  Changed state of RadioButton responsible for applying changes to file
 *         name, file extension or both.
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
        // read apply to names/ext active RadioButton
        r_files->applyto = get_radio_active (radiob);
        // perform updates in file names
        file_names_update_changes (r_files);
    }
}
//------------------------------------------------------------------------------
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
    // Set renexit value in r_files based on the value of ChackBox
    r_files->renexit = (uint8_t) gtk_toggle_button_get_active (toggleb);
}
//------------------------------------------------------------------------------
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
    // Catch Esc key in main window and exit
    if (event->keyval == GDK_KEY_Escape)
        event_close (widget, NULL);
    // Catch Enter and preform rename
    if (event->keyval == GDK_KEY_Return) {
        RFiles *r_files = user_data;
        event_click_rename (widget, r_files);
    }
    return FALSE;
}
//------------------------------------------------------------------------------
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
    // Uppercase / lowercase radios
    GtkWidget *gw_ncc;
    GtkWidget *gw_lcc;
    GtkWidget *gw_upc;

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
    // To uppercase lowercase box
    *gw_container = gtk_grid_new ();
    gtk_grid_set_column_spacing (GTK_GRID (*gw_container), 4);
    gtk_grid_attach (GTK_GRID (*gw_container), gw_ncc, 0, 0, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (*gw_container),
            gw_lcc, gw_ncc, GTK_POS_BOTTOM, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (*gw_container),
            gw_upc, gw_lcc, GTK_POS_BOTTOM, 1, 1);
}
//------------------------------------------------------------------------------
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
    // Space to underscores radios
    GtkWidget *gw_sptounc;
    GtkWidget *gw_sptou;
    GtkWidget *gw_utosp;

    gw_sptounc = gtk_radio_button_new_with_label (NULL, "No change");
    gw_sptou = gtk_radio_button_new_with_label_from_widget (
            GTK_RADIO_BUTTON (gw_sptounc), "Space to underscore");
    gw_utosp = gtk_radio_button_new_with_label_from_widget(
            GTK_RADIO_BUTTON (gw_sptounc), "Underscore to space");
    g_signal_connect (G_OBJECT (gw_sptounc),
            "toggled", G_CALLBACK (event_spaces_radio_active), r_files);
    g_signal_connect (G_OBJECT (gw_sptou),
            "toggled", G_CALLBACK (event_spaces_radio_active), r_files);
    g_signal_connect (G_OBJECT (gw_utosp),
            "toggled", G_CALLBACK (event_spaces_radio_active), r_files);
    // Space to underscores box
    *gw_container = gtk_grid_new ();
    gtk_grid_set_column_spacing (GTK_GRID (*gw_container), 4);
    gtk_grid_attach (GTK_GRID (*gw_container), gw_sptounc, 0, 0, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (*gw_container),
            gw_sptou, gw_sptounc, GTK_POS_BOTTOM, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (*gw_container),
            gw_utosp, gw_sptou, GTK_POS_BOTTOM, 1, 1);
}
//------------------------------------------------------------------------------
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
    // Apply to names and extensions
    GtkWidget *gw_appne;
    GtkWidget *gw_appn;
    GtkWidget *gw_appe;

    gw_appne = gtk_radio_button_new_with_label (NULL, "Apply to name and ext");
    gw_appn = gtk_radio_button_new_with_label_from_widget (
            GTK_RADIO_BUTTON (gw_appne), "Apply to name");
    gw_appe = gtk_radio_button_new_with_label_from_widget (
            GTK_RADIO_BUTTON (gw_appne), "Apply to ext");
    g_signal_connect (G_OBJECT (gw_appne),
            "toggled", G_CALLBACK (event_apply_radio_active), r_files);
    g_signal_connect (G_OBJECT (gw_appn),
            "toggled", G_CALLBACK (event_apply_radio_active), r_files);
    g_signal_connect (G_OBJECT (gw_appe),
            "toggled", G_CALLBACK (event_apply_radio_active), r_files);
    // Apply to names and extensions box
    *gw_container = gtk_grid_new ();
    gtk_grid_set_column_spacing (GTK_GRID (*gw_container), 4);
    gtk_grid_attach (GTK_GRID (*gw_container), gw_appne, 0, 0, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (*gw_container),
            gw_appn, gw_appne, GTK_POS_BOTTOM, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (*gw_container),
            gw_appe, gw_appn, GTK_POS_BOTTOM, 1, 1);
}
//------------------------------------------------------------------------------
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
    GtkWidget *gw_sfrom;
    GtkWidget *gw_sto;
    GtkWidget *gw_lab1;
    GtkWidget *gw_lab2;

    gw_sfrom = gtk_entry_new ();
    gw_sto = gtk_entry_new ();
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
//------------------------------------------------------------------------------
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
    GtkWidget     *gw_del_cnt;
    GtkWidget     *gw_del_pos;
    GtkWidget     *gw_lab;
    GtkAdjustment *gw_del_adj_cnt;
    GtkAdjustment *gw_del_adj_pos;

    gw_lab = gtk_label_new ("Delete text:");
    gw_del_adj_cnt = gtk_adjustment_new (0.0, 0.0, FN_LEN, 1.0, 5.0, 0.0);
    gw_del_cnt = gtk_spin_button_new (gw_del_adj_cnt, 1.0, 0);
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
//------------------------------------------------------------------------------
/**
 * @brief  Creates container with widgets to insert string into file name.
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
    GtkWidget     *gw_lab;
    GtkWidget     *gw_ins_entry;
    GtkWidget     *gw_ins_pos;
    GtkAdjustment *gw_ins_adj_pos;

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
//------------------------------------------------------------------------------
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
    // Rename close buttons
    GtkWidget *gw_but_ok;
    GtkWidget *gw_but_cc;
    // Exit after rename check button
    GtkWidget *gw_renexit;

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
//------------------------------------------------------------------------------
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
    GtkWidget     *gw_entry_box;
    GtkWidget     *gw_vp;
    GtkAdjustment *ga_h;
    GtkAdjustment *ga_v;
    uint16_t       ui_cn = 0;  // file names count

    // Allocate memory for pointers to entries and file name strings
    r_files->entry = g_malloc (n_files * sizeof (GtkWidget*));
    r_files->names.org = g_malloc (n_files * sizeof (char*));
    r_files->names.new = g_malloc (n_files * sizeof (char*));
    // Create box for file name entries
    gw_entry_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 4);
    for (int i = 0; i < n_files; ++i) {
        char *ch_fname = g_file_get_basename (files[i]);
        if (strcmp (ch_fname, "..") == 0) continue;
        if (access (ch_fname, F_OK) == 0) { // check if file exists
            // Allocate memory for original and new file name strings
            r_files->names.org[ui_cn] = g_slice_alloc0 (
                    (FN_LEN + 1) * sizeof (char));
            r_files->names.new[ui_cn] = g_slice_alloc0 (
                    (FN_LEN + 1) * sizeof (char));
            // Copy verified file names to original and new file name string
            strcpy (r_files->names.org[ui_cn], ch_fname);
            strcpy (r_files->names.new[ui_cn], ch_fname);
            // Create entry and set max length to defined file name length
            r_files->entry[ui_cn] = gtk_entry_new ();
            gtk_entry_set_max_length (GTK_ENTRY (r_files->entry[ui_cn]),
                                     FN_LEN);
            // Set entry file names
            gtk_entry_set_text (GTK_ENTRY (r_files->entry[ui_cn]),
                    r_files->names.org[ui_cn]);
            // Add entry to the container
            gtk_box_pack_start (GTK_BOX (gw_entry_box),
                               r_files->entry[ui_cn], FALSE, FALSE, 0);
            ++ui_cn;
        }
        g_free (ch_fname);
    }
    // Make scrollbars
    *gw_container = gtk_scrolled_window_new (NULL, NULL);
    ga_h = gtk_scrolled_window_get_hadjustment(
            GTK_SCROLLED_WINDOW (*gw_container));
    ga_v = gtk_scrolled_window_get_vadjustment(
            GTK_SCROLLED_WINDOW (*gw_container));
    gw_vp = gtk_viewport_new (ga_h, ga_v);
    gtk_container_add (GTK_CONTAINER (gw_vp), gw_entry_box);
    gtk_container_add (GTK_CONTAINER (*gw_container), gw_vp);
    // If file count is smaller than passed arguments, decrease number of
    // entries, original and new file name strings
    if (ui_cn < n_files) {
        r_files->entry = g_realloc (
                r_files->entry, ui_cn * sizeof (GtkWidget*));
        r_files->names.org = g_realloc (
                r_files->names.org, ui_cn * sizeof (char*));
        r_files->names.new = g_realloc (
                r_files->names.new, ui_cn * sizeof (char*));
    }
    r_files->names.cnt = ui_cn; // set file count value in r_files
    return 0;
}
//------------------------------------------------------------------------------
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
    // Create window widget
    *window = gtk_application_window_new (application);
    //*window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    // Set window properties
    gtk_window_set_title (GTK_WINDOW (*window), "Small File Renamer v1.1.1");
    gtk_container_set_border_width (GTK_CONTAINER (*window), 10);
    gtk_window_set_default_size (GTK_WINDOW (*window), WIN_WIDTH, WIN_HEIGHT);
    gtk_window_set_position (GTK_WINDOW (*window), GTK_WIN_POS_CENTER);
    // Connect window events
    g_signal_connect (G_OBJECT (*window),
            "key-press-event", G_CALLBACK (event_win_key_press), r_files);
}
//------------------------------------------------------------------------------
/**
 * @brief  Application startup signal.
 *
 * @param[in,out] application  Pointer to GtkApplication
 * @param[in,out] r_files      Pointer to RFiles with all file names and
 *                             settings
 * @return        none
 */
static void
startup (GtkApplication *application,
         RFiles         *r_files)
{
    rfiles_init (r_files);
}
//------------------------------------------------------------------------------
/**
 * @brief  Application shutdown signal.
 *
 * @param[in,out] application  Pointer to GtkApplication
 * @param[in,out] r_files      Pointer to RFiles with all file names and
 *                             settings
 * @return        none
 */
static void
shutdown (GtkApplication *application,
          RFiles         *r_files)
{
    free_stuff (r_files);
}
//------------------------------------------------------------------------------
/**
 * @brief  Application activate signal.
 *
 * @param[in,out] application  Pointer to GtkApplication
 * @return        none
 */
static void
activate (GtkApplication *application)
{
    printf ("No files to open\n");
}
//------------------------------------------------------------------------------
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
      const char      *hint,
      RFiles          *r_files)
{
    GtkWidget *window;
    GtkWidget *gw_entry_box;
    GtkWidget *gw_vbox;
    GtkWidget *gw_uplc_box;
    GtkWidget *gw_undersc_box;
    GtkWidget *gw_replace_str_box;
    GtkWidget *gw_del_str_box;
    GtkWidget *gw_ins_str_box;
    GtkWidget *gw_apply_box;
    GtkWidget *gw_udusc_box;
    GtkWidget *gw_okcl_box;
    GtkWidget *gw_sep;

    // Create file name entries and set r_files properties
    create_file_name_entries (files , n_files, r_files, &gw_entry_box);
    if (r_files->names.cnt < 1) {
        printf ("No files to open\n");
        return;
    }
    // Create main window and set properties
    create_window (&window, application, r_files);
    create_upcase_lowercase_box (&gw_uplc_box, r_files);
    create_spaces_to_underscores_box (&gw_undersc_box, r_files);
    create_apply_to_names_ext_box (&gw_apply_box, r_files);
    create_replace_str_with_str_box (&gw_replace_str_box, r_files);
    create_delete_chars_box (&gw_del_str_box, r_files);
    create_insert_string_box (&gw_ins_str_box, r_files);
    // OK, Close box
    create_rename_close_exit_box (&gw_okcl_box, r_files);

    // Box for up down, underscore and apply to boxes
    gw_udusc_box = gtk_grid_new ();
    gtk_grid_set_column_spacing (GTK_GRID (gw_udusc_box), 4);
    gtk_grid_set_row_spacing (GTK_GRID (gw_udusc_box), 4);
    gtk_grid_attach (GTK_GRID (gw_udusc_box), gw_apply_box, 0, 0, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (gw_udusc_box),
            gw_uplc_box, gw_apply_box, GTK_POS_RIGHT, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (gw_udusc_box),
            gw_undersc_box, gw_uplc_box, GTK_POS_RIGHT, 1, 1);

    gw_sep = gtk_separator_new (GTK_ORIENTATION_HORIZONTAL);
    gtk_grid_attach_next_to (GTK_GRID (gw_udusc_box),
            gw_sep, gw_apply_box, GTK_POS_BOTTOM, 3, 1);

    gtk_grid_attach_next_to (GTK_GRID (gw_udusc_box),
            gw_replace_str_box, gw_sep, GTK_POS_BOTTOM, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (gw_udusc_box),
            gw_ins_str_box, gw_replace_str_box, GTK_POS_RIGHT, 1, 1);
    gtk_grid_attach_next_to (GTK_GRID (gw_udusc_box),
            gw_del_str_box, gw_ins_str_box, GTK_POS_RIGHT, 1, 1);

    // Main application widgets box
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
//------------------------------------------------------------------------------
/**
 * @brief Main function.
 *
 * @param[in] argc Arguments passed to the program from the environment in which
 *                 the program is run
 * @param[in] argv Pointer to the first element of an array of pointers that
 *                 represent the arguments passed to the program
 * @return         Return value
 */  
//------------------------------------------------------------------------------
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
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);
    return status;
}
//------------------------------------------------------------------------------


