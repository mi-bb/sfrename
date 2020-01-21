/**
 * @file  rfnames.c
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
 * @brief  RFnames struncture and functions.
 * 
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#include "defs.h"
#include "cres.h"
#include "rfnames.h"
/*----------------------------------------------------------------------------*/
/**
 * @fn  static const char * get_name_dir_len (const char   *s_pth,
                                              size_t       *ul_len)
 * @brief      Get file name pointer and length of directory string in given
 *             file path.
 * @param[in]  s_pth   Input file path
 * @param[out] ul_len  Output for directory string length
 * @return     Pointer to file name in path
 *
 * @fn  static void rfnames_reserve  (RFnames      *rf_names,
                                      const size_t  ul_size)
 * @brief      Reserve space for file names, entries in RFnames object.
 * @param[out] rf_names  RFnames item to operate
 * @param[in]  ul_size   Size to reserve
 * @return     none
 */
/*----------------------------------------------------------------------------*/
static const char * get_name_dir_len (const char   *s_pth,
                                      size_t       *ul_len);

static void         rfnames_reserve  (RFnames      *rf_names,
                                      const size_t  ul_size);
/*----------------------------------------------------------------------------*/
static const char *
get_name_dir_len (const char *s_pth,
                  size_t     *ul_len)
{
    const char *s_fn = strrchr (s_pth, '/');
    if (s_fn == NULL) {
        *ul_len = 0;
        s_fn = s_pth;
    }
    else {
        s_fn++;
        *ul_len = (size_t) (s_fn - s_pth);
    }
    return s_fn;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  RFnames initialization.
 */
void
rfnames_init (RFnames *rf_names)
{
    rf_names->cnt   = 0;    /* names count */
    rf_names->s_org = NULL; /* org names list */
    rf_names->s_new = NULL; /* new names list */
    rf_names->s_pth = NULL; /* file paths */
    rf_names->entry = NULL; /* entry objects */
}
/*----------------------------------------------------------------------------*/
static void
rfnames_reserve (RFnames      *rf_names,
                 const size_t  ul_size)
{
    /* No need to resize */
    if (rf_names->cnt == ul_size)
        return;

    /* if larger free rest */
    while (ul_size < rf_names->cnt) {
        g_slice_free1 ( (FN_LEN+1) * sizeof (char),
                        rf_names->s_new[--rf_names->cnt]);
        g_slice_free1 ( (FN_LEN+1) * sizeof (char),
                        rf_names->s_org[rf_names->cnt]);
        g_slice_free1 ( 
                (strlen (rf_names->s_pth[rf_names->cnt]) +1) * sizeof (char),
                rf_names->s_pth[rf_names->cnt]);
        /*
        if (GTK_IS_WIDGET (rf_names->entry[rf_names->cnt]))
            gtk_widget_destroy (rf_names->entry[rf_names->cnt]);
            */
    }

    cres ((void ***) &rf_names->s_org, ul_size, sizeof (char*));
    cres ((void ***) &rf_names->s_new, ul_size, sizeof (char*));
    cres ((void ***) &rf_names->s_pth, ul_size, sizeof (char*));
    cres ((void ***) &rf_names->entry, ul_size, sizeof (GtkWidget*));

    if (ul_size == 0) {
        rfnames_init (rf_names);
    }

    /* Update file list count */
    rf_names->cnt = ul_size;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free allocated memory.
 */
void
rfnames_free (RFnames *rf_names)
{
    rfnames_reserve (rf_names, 0);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Add file to RFnames.
 */
void
rfnames_add (RFnames    *rf_names,
             const char *s_pth)
{
    size_t      ul_len = 0;
    size_t      i      = rf_names->cnt;
    const char *s_fn   = get_name_dir_len (s_pth, &ul_len);

    /* Resize file list by one */
    rfnames_reserve (rf_names, rf_names->cnt + 1);

    /* Allocate memory for original and new file name strings */
    rf_names->s_org[i] = g_slice_alloc0 ( (FN_LEN + 1) * sizeof (char));
    rf_names->s_new[i] = g_slice_alloc0 ( (FN_LEN + 1) * sizeof (char));
    rf_names->s_pth[i] = g_slice_alloc0 ( (ul_len + 1) * sizeof (char));

    /* Create entry and set max length to defined file name length */
    rf_names->entry[i] = gtk_entry_new ();

    gtk_entry_set_max_length (GTK_ENTRY (rf_names->entry[i]), FN_LEN);
    /* Set entry file names */
    gtk_entry_set_text (GTK_ENTRY (rf_names->entry[i]), s_fn);

    /* Set full path as a tooltip */
    gtk_widget_set_tooltip_text (rf_names->entry[i], s_pth);

    /* Copy verified file names to original and new file name string */
    strncpy (rf_names->s_org[i], s_fn, FN_LEN);
    strncpy (rf_names->s_new[i], s_fn, FN_LEN);
    rf_names->s_org[i][FN_LEN] = '\0';
    rf_names->s_new[i][FN_LEN] = '\0';
    memcpy (rf_names->s_pth[i], s_pth, ul_len);
    #ifdef DEBUG
    printf ("loaded : %s\n", rf_names->s_org[i]);
    printf ("   dir : %s\n", rf_names->s_pth[i]);
    printf ("  full : %s\n", s_pth);
    #endif
}
/*----------------------------------------------------------------------------*/
void
rfnames_free2 (RFnames *rf_names)
{
    rfnames_reserve (rf_names, 5);
}
/*----------------------------------------------------------------------------*/

