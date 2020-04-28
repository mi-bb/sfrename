/**
 * @file  rfitem.c
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
 * @brief  RFitem struncture and functions.
 * 
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#include <err.h>
#include "defs.h"
#include "rfitem.h"
/*----------------------------------------------------------------------------*/
/**
 * @fn  static const char * get_name_dir_len (const char   *s_pth,
 *                                            size_t       *ui_len)
 * @brief      Get file name pointer and length of directory string in given
 *             file path.
 * @param[in]  s_pth   Input file path
 * @param[out] ui_len  Output for directory string length
 * @return     Pointer to file name in path
 *
 * @fn         static int str_compare (const char *a, const char *b)
 * @brief      String compare function.
 * @param[in]  a  String a 
 * @param[in]  b  String b
 * @return     Compare result
 *
 * @fn         static void rfitem_label_set_markup (RFitem *rf_item)
 * @brief      Set label style and text based on type of file / folder.
 * @param[out] rf_item Pointer to RFitem object
 * @return     none
 *
 * @fn         static void rfitem_init (RFitem *rf_item)
 * @brief      RFitem initialization.
 * @param[out] rf_item Pointer to RFitem object
 * @return     none
 *
 * @fn         static void rfitem_free_data (RFitem *rf_item)
 * @brief      Free allocated data memory, without widgets.
 * @param[out] rf_item Pointer to RFitem object
 * @return     none
 *
 * @fn         static void event_click_rev  (GtkWidget  *widget,
                                             RFitem     *rf_item)
 * @brief      Restore original file name button clicked.
 * @param[in]  widget  The object which received the signal
 * @param[out] rf_item Pointer to RFitem object
 * @return     none
 */
/*----------------------------------------------------------------------------*/
static const char * get_name_dir_len        (const char *s_pth,
                                             size_t     *ui_len)
                                             __attribute__ ((nonnull (1)));

static int          str_compare             (const char *a,
                                             const char *b);

static void         rfitem_label_set_markup (RFitem     *rf_item);

static void         rfitem_init             (RFitem     *rf_item);

static void         rfitem_free_data        (RFitem     *rf_item);

static void         event_click_rev         (GtkWidget  *widget,
                                             RFitem     *rf_item);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get file name pointer and length of directory string in given
 *         file path.
 */
static const char *
get_name_dir_len (const char *s_pth,
                  size_t     *ui_len)
{
    const char *s_fn = strrchr (s_pth, '/');
    if (s_fn == NULL) {
        *ui_len = 0;
        s_fn = s_pth;
    }
    else {
        ++s_fn;
        *ui_len = (size_t) (s_fn - s_pth);
    }
    return s_fn;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  String compare function.
 */
static int
str_compare (const char *a,
             const char *b)
{
    int i_res = 0; /* Function result */

    if (a == NULL || b == NULL) {
        if (a == NULL && b == NULL) {
            i_res = 0;
        }
        else {
            if (a == NULL) {
                i_res = -1;
            }
            else {
                i_res = 1;
            }
        }
    }
    else {
        //i_res = strcmp (a, b);
        i_res = g_utf8_collate (a, b);
    }
    return i_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set label style and text based on type of file / folder.
 */
static void
rfitem_label_set_markup (RFitem *rf_item)
{
    char s_mark [100];
    s_mark[0] = '\0';

    strcpy (s_mark, "<span font_family=\"monospace\"");

    if (rf_item->b_slink) {
        strcat (s_mark, " foreground=\"blue\"");
    }
    else if (rf_item->b_hidden) {
        strcat (s_mark, " foreground=\"green\"");
    }
    if (rf_item->f_type == G_FILE_TYPE_REGULAR) {
        strcat (s_mark, ">F</span>");
    }
    else if (rf_item->f_type == G_FILE_TYPE_DIRECTORY) {
        strcat (s_mark, ">D</span>");
    }
    else if (rf_item->f_type == G_FILE_TYPE_SYMBOLIC_LINK) {
        strcat (s_mark, ">S</span>");
    }
    else if (rf_item->f_type == G_FILE_TYPE_SPECIAL) {
        strcat (s_mark, ">D</span>");
    }
    else {
        strcat (s_mark, ">?</span>");
    }
    gtk_label_set_markup (GTK_LABEL (rf_item->label), s_mark);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  RFitem initialization.
 */
static void
rfitem_init (RFitem *rf_item)
{
    rf_item->s_org     = NULL;  /* original name */
    rf_item->s_new     = NULL;  /* new name */
    rf_item->s_pth     = NULL;  /* file path */
    rf_item->label     = NULL;  /* type label */
    rf_item->entry     = NULL;  /* entry */
    rf_item->check     = NULL;  /* checkbutton */
    rf_item->rbut      = NULL;  /* restore button */
    rf_item->dbut      = NULL;  /* delete button */
    rf_item->box       = NULL;  /* box for widgets */
    rf_item->f_type    = 0;     /* file type */
    rf_item->new_len   = 0;     /* file type */
    rf_item->new_u8len = 0;     /* file type */
    rf_item->b_slink   = FALSE; /* symlink */
    rf_item->b_hidden  = FALSE; /* hidden */
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create and return new RFnames object from GFile.
 */
RFitem *
rfitem_new_from_gfile (GFile *g_file)
{
    size_t      ui_len  = 0;    /* Number of dir characters in file path */
    RFitem     *rf_item = NULL; /* RFitem object to return */
    GError     *g_err   = NULL; /* For error output */
    GFileInfo  *f_info  = NULL; /* For file info */
    const char *s_path  = NULL; /* Full file path dir + file name */
    const char *s_fn    = NULL; /* File name only */

    if ((rf_item = malloc (sizeof (RFitem))) == NULL)
        err (EXIT_FAILURE, NULL);

    rfitem_init (rf_item);

    rf_item->s_org = malloc ((FN_LEN + 1) * sizeof (char));
    rf_item->s_new = malloc ((FN_LEN + 1) * sizeof (char));
    rf_item->s_pth = malloc ((FN_LEN + 1) * sizeof (char));
    if (rf_item->s_org == NULL || rf_item->s_new == NULL ||
        rf_item->s_pth == NULL) {
        free (rf_item);
        err (EXIT_FAILURE, NULL);
    }
    rf_item->s_org[0] = '\0';
    rf_item->s_new[0] = '\0';
    rf_item->s_pth[0] = '\0';
    s_path  = g_file_peek_path (g_file);
    s_fn    = get_name_dir_len (s_path, &ui_len);

    f_info = g_file_query_info (g_file,
                                "standard::*",
                                G_FILE_QUERY_INFO_NONE,
                                NULL,
                                &g_err);
    if (f_info != NULL) {
        rf_item->f_type   = g_file_info_get_file_type (f_info);
        rf_item->b_hidden = g_file_info_get_is_hidden (f_info);
        rf_item->b_slink  = g_file_info_get_is_symlink (f_info);
        g_object_unref (f_info);
    }
    else {
        #ifdef DEBUG
        printf ("Could not get ifno of %s\n", s_fn);
        #endif
        rf_item->f_type = G_FILE_TYPE_UNKNOWN;
    }
    rf_item->label = gtk_label_new (NULL);
    rf_item->entry = gtk_entry_new ();
    rf_item->check = gtk_check_button_new ();
    rf_item->rbut  = gtk_button_new_with_label ("R");
    rf_item->dbut  = gtk_button_new_with_label ("D");
    rf_item->box   = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 4);
    /* Set type label markup (file or dir, type color) */
    rfitem_label_set_markup (rf_item);
    /* Create entry and set max length to defined file name length */
    gtk_entry_set_max_length (GTK_ENTRY (rf_item->entry), FN_LEN);
    /* Set entry file names */
    gtk_entry_set_text (GTK_ENTRY (rf_item->entry), s_fn);
    /* Set full path as a tooltip */
    gtk_widget_set_tooltip_text (rf_item->entry, s_path);
    /* Set item active to apply changes */
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (rf_item->check), TRUE);

    gtk_widget_set_tooltip_text (rf_item->rbut, "Restore original");
    gtk_widget_set_tooltip_text (rf_item->dbut, "Remove from list");
    gtk_widget_set_margin_bottom (rf_item->box, 4);

    /* Copy verified file names to original and new file name string */
    memcpy (rf_item->s_pth, s_path, ui_len);
    rf_item->s_pth[ui_len] = '\0';
    strncpy (rf_item->s_org, s_fn, FN_LEN);
    rf_item->s_org[FN_LEN] = '\0';
    rf_item->org_len = strlen (s_fn);
    rf_item->org_u8len = (size_t) g_utf8_strlen (s_fn, -1);
    strncpy (rf_item->s_new, s_fn, FN_LEN);
    rf_item->s_new[FN_LEN] = '\0';
    rf_item->new_len = rf_item->org_len;
    rf_item->new_u8len = rf_item->org_u8len;
    /* rfitem_set_snew_from_sorg (rf_item); */

    gtk_box_pack_start (GTK_BOX (rf_item->box), rf_item->check, FALSE, FALSE,0);
    gtk_box_pack_start (GTK_BOX (rf_item->box), rf_item->label, FALSE, FALSE,0);
    gtk_box_pack_start (GTK_BOX (rf_item->box), rf_item->entry, TRUE,  TRUE, 0);
    gtk_box_pack_start (GTK_BOX (rf_item->box), rf_item->rbut,  FALSE, FALSE,0);
    gtk_box_pack_start (GTK_BOX (rf_item->box), rf_item->dbut,  FALSE, FALSE,0);

    #ifdef DEBUG
    printf ("loaded : %s\n", rf_item->s_org);
    printf ("   dir : %s\n", rf_item->s_pth);
    printf ("  full : %s\n", s_path);
    printf ("  type : %d\n", rf_item->f_type);
    #endif

    g_signal_connect (G_OBJECT (rf_item->rbut), "clicked",
                      G_CALLBACK (event_click_rev), rf_item);

    return rf_item;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create and return new RFnames object from file path.
 */
RFitem *
rfitem_new_from_sfile (const char *s_pth)
{
    GFile  *g_file  = g_file_new_for_path (s_pth);
    RFitem *rf_item = rfitem_new_from_gfile (g_file);
    g_object_unref (g_file);
    return rf_item;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free allocated data memory, without widgets.
 */
static void
rfitem_free_data (RFitem *rf_item)
{
    free (rf_item->s_new);
    free (rf_item->s_org);
    free (rf_item->s_pth);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free allocated memory.
 */
void
rfitem_free (RFitem *rf_item)
{
    rfitem_free_data (rf_item);
    free (rf_item);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free allocated memory and delete widgets.
 */
void
rfitem_delete (RFitem *rf_item)
{
    rfitem_free_data (rf_item);
    gtk_widget_destroy (rf_item->box);
    free (rf_item);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Copy s_org string and length values to s_new.
 */
void
rfitem_set_snew_from_sorg (RFitem *rf_item)
{
    rf_item->new_len = rf_item->org_len;
    rf_item->new_u8len = rf_item->org_u8len;
    memcpy (rf_item->s_new, rf_item->s_org, rf_item->org_len + 1);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Copy s_new string and length values to s_org.
 */
void
rfitem_set_sorg_from_snew (RFitem *rf_item)
{
    rf_item->org_len = rf_item->new_len;
    rf_item->org_u8len = rf_item->new_u8len;
    memcpy (rf_item->s_org, rf_item->s_new, rf_item->new_len + 1);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief      Get s_new value.
 */
const char *
rfitem_get_snew (const RFitem *rf_item)
{
    return (const char *) rf_item->s_new;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set s_new value.
 */
void
rfitem_set_snew (RFitem     *rf_item,
                 const char *val)
{
    rf_item->new_len = strlen (val);
    memcpy (rf_item->s_new, val, rf_item->new_len+1);
    rf_item->new_u8len = (size_t) g_utf8_strlen (val, -1);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get s_org value.
 */
const char *
rfitem_get_sorg (const RFitem *rf_item)
{
    return (const char *) rf_item->s_org;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set s_org value.
 */
void
rfitem_set_sorg (RFitem     *rf_item,
                 const char *val)
{
    strcpy (rf_item->s_org, val);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get s_pth value.
 */
const char *
rfitem_get_spth (const RFitem *rf_item)
{
    return (const char *) rf_item->s_pth;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set s_pth value.
 */
void
rfitem_set_spth (RFitem     *rf_item,
                 const char *val)
{
    strcpy (rf_item->s_pth, val);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Restore original file name button clicked.
 */
static void
event_click_rev (GtkWidget *widget __attribute__ ((unused)),
                 RFitem    *rf_item)
{
    rfitem_entry_restore (rf_item);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get check button checked value.
 */
gboolean
rfitem_get_checked (const RFitem *rf_item)
{
    return gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (rf_item->check));
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set check button checked value.
 */
void
rfitem_set_checked (RFitem       *rf_item,
                    const gboolean  b_val)
{
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (rf_item->check), b_val);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Invert check item check value
 */
void
rfitem_invert_checked (RFitem *rf_item)
{
    rfitem_set_checked (rf_item,
            !gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (rf_item->check)));
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get text from entry.
 */
const char *
rfitem_entry_get_text (const RFitem *rf_item)
{
    return gtk_entry_get_text (GTK_ENTRY (rf_item->entry));
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Checks if the new entry text is equal to the previous value
 *         and changes it to a new value if it is not.
 */
void
rfitem_entry_check_and_update (RFitem     *rf_item,
                               const char *new_str)
{
    if (strcmp (new_str, rfitem_entry_get_text (rf_item)) != 0)
        gtk_entry_set_text (GTK_ENTRY (rf_item->entry), new_str);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Restore original file name to entry
 */
void
rfitem_entry_restore (RFitem *rf_item)
{
    gtk_entry_set_text (GTK_ENTRY (rf_item->entry), rf_item->s_org);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Compare 2 RFitem objects.
 */
int
rfitem_compare (const RFitem *rf_item1,
                const RFitem *rf_item2)
{
    int   i_res  = 0;
    char *s_str1 = g_utf8_casefold (rf_item1->s_org, -1);
    char *s_str2 = g_utf8_casefold (rf_item2->s_org, -1);

    i_res = str_compare (s_str1, s_str2);

    g_free (s_str1);
    g_free (s_str2);

    return i_res;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if RFitem type is a file.
 */
gboolean
rfitem_is_file (const RFitem *rf_item)
{
    return (rf_item->f_type == G_FILE_TYPE_REGULAR);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if RFitem type is a directory.
 */
gboolean
rfitem_is_folder (const RFitem *rf_item)
{
    return (rf_item->f_type == G_FILE_TYPE_DIRECTORY);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if RFitem file/dir type is a symlink.
 */
gboolean
rfitem_is_symlink (const RFitem *rf_item)
{
    return rf_item->b_slink;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if RFitem file/dir is hidden.
 */
gboolean
rfitem_is_hidden (const RFitem *rf_item)
{
    return rf_item->b_hidden;
}
/*----------------------------------------------------------------------------*/


