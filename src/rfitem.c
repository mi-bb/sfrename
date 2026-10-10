/**
 * @file  rfitem.c
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
 * @brief  RFitem struncture and functions.
 * 
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <err.h>
#include <stdio.h>
#include "defs.h"
#include "rfitem.h"
/*----------------------------------------------------------------------------*/
/**
 * @fn  static const char * get_name_dir_len (const char *path,
 *                                            size_t     *len)
 * @brief      Get file name pointer and length of directory string in given
 *             file path.
 * @param[in]  path   Input file path
 * @param[out] len  Output for directory string length
 * @return     Pointer to file name in path
 *
 * @fn         static int str_compare (const char *a, const char *b)
 * @brief      String compare function.
 * @param[in]  a  String a 
 * @param[in]  b  String b
 * @return     Compare result
 *
 * @fn         static void rfitem_label_set_markup (RFitem *item)
 * @brief      Set label style and text based on type of file / folder.
 * @param[out] item Pointer to RFitem object
 * @return     none
 *
 * @fn         static void rfitem_init (RFitem *item)
 * @brief      RFitem initialization.
 * @param[out] item Pointer to RFitem object
 * @return     none
 *
 * @fn         static void rfitem_free_data (RFitem *item)
 * @brief      Free allocated data memory, without widgets.
 * @param[out] item Pointer to RFitem object
 * @return     none
 *
 * @fn         static void event_click_rev  (GtkWidget  *widget,
                                             RFitem     *item)
 * @brief      Restore original file name button clicked.
 * @param[in]  widget  The object which received the signal
 * @param[out] item Pointer to RFitem object
 * @return     none
 */
/*----------------------------------------------------------------------------*/
static const char * get_name_dir_len        (const char *path,
                                             size_t     *len)
                                             __attribute__ ((nonnull (1)));

static int          str_compare             (const char *a,
                                             const char *b);

static void         rfitem_label_set_markup (RFitem     *item);

static void         rfitem_init             (RFitem     *item);

static void         rfitem_free_data        (RFitem     *item);

static void         event_click_rev         (GtkWidget  *widget,
                                             RFitem     *item);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get file name pointer and length of directory string in given
 *         file path.
 */
static const char *
get_name_dir_len (const char *path,
                  size_t     *len)
{
    const char *file_name = strrchr (path, '/');
    if (file_name == nullptr) {
        *len = 0;
        file_name = path;
    }
    else {
        ++file_name;
        *len = (size_t) (file_name - path);
    }
    return file_name;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  String compare function.
 */
static int
str_compare (const char *a,
             const char *b)
{
    int result = 0; /* Function result */

    if (a == nullptr || b == nullptr) {
        if (a == nullptr && b == nullptr) {
            result = 0;
        }
        else {
            if (a == nullptr) {
                result = -1;
            }
            else {
                result = 1;
            }
        }
    }
    else {
        //i_res = strcmp (a, b);
        result = g_utf8_collate (a, b);
    }
    return result;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set label style and text based on type of file / folder.
 */
static void
rfitem_label_set_markup (RFitem *item)
{
    char        markup [100]; /* Pango markup for the type label */
    const char *colour = "";  /* Optional colour attribute */
    char        c_type;       /* Single letter naming the type */

    if (item->b_slink) {
        colour = " foreground=\"blue\"";
    }
    else if (item->b_hidden) {
        colour = " foreground=\"green\"";
    }

    switch (item->f_type) {
        case G_FILE_TYPE_REGULAR:       c_type = 'F'; break;
        case G_FILE_TYPE_DIRECTORY:     c_type = 'D'; break;
        case G_FILE_TYPE_SYMBOLIC_LINK: c_type = 'S'; break;
        case G_FILE_TYPE_SPECIAL:       c_type = 'D'; break;
        default:                        c_type = '?'; break;
    }

    snprintf (markup, sizeof (markup),
              "<span font_family=\"monospace\"%s>%c</span>", colour, c_type);

    gtk_label_set_markup (GTK_LABEL (item->label), markup);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  RFitem initialization.
 *
 * Zeroes every field: the name/path strings, the widget pointers, the file
 * type, both length pairs and the symlink/hidden flags.
 */
static void
rfitem_init (RFitem *item)
{
    *item = (RFitem){};
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create and return new RFnames object from GFile.
 */
RFitem *
rfitem_new_from_gfile (GFile *g_file)
{
    size_t      len    = 0;       /* Number of dir characters in file path */
    RFitem     *item   = nullptr; /* RFitem object to return */
    GError     *g_err  = nullptr; /* For error output */
    GFileInfo  *f_info = nullptr; /* For file info */
    const char *f_path = nullptr; /* Full file path dir + file name */
    const char *f_name = nullptr; /* File name only */

    if ((item = malloc (sizeof (RFitem))) == nullptr)
        err (EXIT_FAILURE, nullptr);

    rfitem_init (item);

    item->s_org = malloc ((FN_LEN + 1) * sizeof (char));
    item->s_new = malloc ((FN_LEN + 1) * sizeof (char));
    item->s_pth = malloc ((FN_LEN + 1) * sizeof (char));
    if (item->s_org == nullptr || item->s_new == nullptr ||
        item->s_pth == nullptr) {
        free (item);
        err (EXIT_FAILURE, nullptr);
    }
    f_path = g_file_peek_path (g_file);
    f_name = get_name_dir_len (f_path, &len);

    f_info = g_file_query_info (g_file,
                                "standard::*",
                                G_FILE_QUERY_INFO_NONE,
                                nullptr,
                                &g_err);
    if (f_info != nullptr) {
        item->f_type   = g_file_info_get_file_type (f_info);
        item->b_hidden = g_file_info_get_is_hidden (f_info);
        item->b_slink  = g_file_info_get_is_symlink (f_info);
        g_object_unref (f_info);
    }
    else {
        #ifdef DEBUG
        printf ("Could not get ifno of %s\n", s_fn);
        #endif
        item->f_type = G_FILE_TYPE_UNKNOWN;
    }
    item->label = gtk_label_new (nullptr);
    item->entry = gtk_entry_new ();
    item->check = gtk_check_button_new ();
    item->rbut  = gtk_button_new_with_label ("R");
    item->dbut  = gtk_button_new_with_label ("D");
    item->box   = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 4);
    /* Set type label markup (file or dir, type color) */
    rfitem_label_set_markup (item);
    /* Create entry and set max length to defined file name length */
    gtk_entry_set_max_length (GTK_ENTRY (item->entry), FN_LEN);
    /* Set entry file names */
    gtk_entry_set_text (GTK_ENTRY (item->entry), f_name);
    /* Set full path as a tooltip */
    gtk_widget_set_tooltip_text (item->entry, f_path);
    /* Set item active to apply changes */
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (item->check), TRUE);

    gtk_widget_set_tooltip_text (item->rbut, "Restore original");
    gtk_widget_set_tooltip_text (item->dbut, "Remove from list");
    gtk_widget_set_margin_bottom (item->box, 4);

    /* Copy verified file names to original and new file name string */
    item->org_len = strlen (f_name);
    item->org_u8len = (size_t) g_utf8_strlen (f_name, -1);
    item->new_len = item->org_len;
    item->new_u8len = item->org_u8len;
    memcpy (item->s_pth, f_path, len);
    item->s_pth[len] = '\0';
    memcpy (item->s_org, f_name, item->org_len + 1);
    memcpy (item->s_new, f_name, item->org_len + 1);
    /*item->s_org[FN_LEN] = '\0';*/
    /*item->s_new[FN_LEN] = '\0';*/
    /* rfitem_set_snew_from_sorg (item); */

    gtk_box_pack_start (GTK_BOX (item->box), item->check, FALSE, FALSE,0);
    gtk_box_pack_start (GTK_BOX (item->box), item->label, FALSE, FALSE,0);
    gtk_box_pack_start (GTK_BOX (item->box), item->entry, TRUE,  TRUE, 0);
    gtk_box_pack_start (GTK_BOX (item->box), item->rbut,  FALSE, FALSE,0);
    gtk_box_pack_start (GTK_BOX (item->box), item->dbut,  FALSE, FALSE,0);

    #ifdef DEBUG
    printf ("loaded : %s\n", item->s_org);
    printf ("   dir : %s\n", item->s_pth);
    printf ("  full : %s\n", f_path);
    printf ("  type : %d\n", item->f_type);
    #endif

    g_signal_connect (G_OBJECT (item->rbut), "clicked",
                      G_CALLBACK (event_click_rev), item);

    return item;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create and return new RFnames object from file path.
 */
RFitem *
rfitem_new_from_file_path (const char *path)
{
    auto g_file  = g_file_new_for_path (path);
    auto item = rfitem_new_from_gfile (g_file);
    g_object_unref (g_file);
    return item;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free allocated data memory, without widgets.
 */
static void
rfitem_free_data (RFitem *item)
{
    free (item->s_new);
    free (item->s_org);
    free (item->s_pth);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free allocated memory.
 */
void
rfitem_free (RFitem *item)
{
    rfitem_free_data (item);
    free (item);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free allocated memory and delete widgets.
 */
void
rfitem_delete (RFitem *item)
{
    rfitem_free_data (item);
    gtk_widget_destroy (item->box);
    free (item);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Copy s_org string and length values to s_new.
 */
void
rfitem_set_snew_from_sorg (RFitem *item)
{
    item->new_len = item->org_len;
    item->new_u8len = item->org_u8len;
    memcpy (item->s_new, item->s_org, item->org_len + 1);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Copy s_new string and length values to s_org.
 */
void
rfitem_set_sorg_from_snew (RFitem *item)
{
    item->org_len = item->new_len;
    item->org_u8len = item->new_u8len;
    memcpy (item->s_org, item->s_new, item->new_len + 1);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set s_new value.
static void
rfitem_set_snew (RFitem     *item,
                 const char *val)
{
    item->new_len = strlen (val);
    memcpy (item->s_new, val, item->new_len+1);
    item->new_u8len = (size_t) g_utf8_strlen (val, -1);
}
 */
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set s_org value.
static void
rfitem_set_sorg (RFitem     *item,
                 const char *val)
{
    strcpy (item->s_org, val);
}
 */
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set s_pth value.
static void
rfitem_set_spth (RFitem     *item,
                 const char *val)
{
    strcpy (item->s_pth, val);
}
 */
/*----------------------------------------------------------------------------*/
/**
 * @brief  Restore original file name button clicked.
 */
static void
event_click_rev ([[maybe_unused]] GtkWidget *widget,
                 RFitem    *item)
{
    rfitem_entry_restore (item);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Invert check item check value
 */
void
rfitem_invert_checked (RFitem *item)
{
    rfitem_set_checked (item,
            !gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (item->check)));
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Checks if the new entry text is equal to the previous value
 *         and changes it to a new value if it is not.
 */
void
rfitem_entry_check_and_update (RFitem     *item,
                               const char *new_str)
{
    if (strcmp (new_str, rfitem_entry_get_text (item)) != 0)
        gtk_entry_set_text (GTK_ENTRY (item->entry), new_str);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Compare 2 RFitem objects.
 */
int
rfitem_compare (const RFitem *item1,
                const RFitem *item2)
{
    int  result  = 0;
    auto str1 = g_utf8_casefold (item1->s_org, -1);
    auto str2 = g_utf8_casefold (item2->s_org, -1);

    result = str_compare (str1, str2);

    g_free (str1);
    g_free (str2);

    return result;
}
/*----------------------------------------------------------------------------*/

