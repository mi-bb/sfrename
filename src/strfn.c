/**
 * @file  strfn.c
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
 * @brief  Functions to modify strings
 *
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#include <glib.h>
#include <inttypes.h>
#include <stdio.h>
#include "defs.h"
#include "strfn.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get length (not greater than ui_max) in bytes for the input string.
 */
size_t
get_valid_length (const char   *s_str,
                  const size_t  ui_max)
{
    const char *s_end  = NULL; /* Pointer to last valid char in string */
    size_t      ui_len = 0;    /* Valid length to return */

    ui_len = strlen (s_str);

    if (ui_len > ui_max)
        ui_len = ui_max;

    g_utf8_validate (s_str, (gssize) ui_len, &s_end);
    ui_len = (size_t) (s_end - s_str);

    return ui_len;
}
/*----------------------------------------------------------------------------*/
/** 
 * @brief  Replace one string with another in a given src_dest string.
 */ 
void
string_replace_in (char              *s_src_dst,
                   const ProcessData *pd_data)
{
    char          s_tmp [FN_LEN+1];           /* temp file name */
    size_t        ui_vlen  = 0;               /* valid name length */
    size_t        i        = 0;               /* the i */
    size_t        ui_frlen = 0;               /* rename from length */
    const size_t  ui_max   = FN_LEN;          /* max length of name */
    const char   *s_fr     = pd_data->s_str1; /* "replace from" string */
    const char   *s_to     = pd_data->s_str2; /* "replace to" string */
    char         *fp       = NULL;            /* find string pointer */

    if (g_utf8_validate (s_src_dst, -1, NULL) && 
        g_utf8_validate (s_fr, -1, NULL) &&
        g_utf8_validate (s_to, -1, NULL) ) {

        const char * __restrict sp = s_src_dst; /* copy src_dest pointer */
        char       * __restrict dp = s_tmp;     /* copy s_tmp pointer */

        fp = strstr (sp, s_fr);

        if (fp == NULL)
            return; 

        ui_frlen = strlen (s_fr);

        while (fp != NULL) {

            const char * __restrict tp = s_to;  /* pointer to s_to */

            while (sp != fp && i < FN_LEN) {
                dp[i++] = *sp++;
            }

            while (*tp && i < FN_LEN) {
                dp[i++] = *tp++;
            }

            /* change source pointer to "after found" */
            sp = fp + ui_frlen; 
            fp = strstr (sp, s_fr);
        }
        while (*sp && i < FN_LEN) {
            dp[i++] = *sp++;
        }
        dp[i] = '\0';
    }
    else
        return;

    ui_vlen = get_valid_length (s_tmp, ui_max);

    memcpy (s_src_dst, s_tmp, ui_vlen);
    s_src_dst[ui_vlen] = '\0';

    #ifdef DEBUG
        printf ("\n%ld %s\n", strlen (s_tmp), s_tmp);
        printf ("%ld %s\n", strlen (s_src_dst), s_src_dst);
    #endif
}
/*----------------------------------------------------------------------------*/
/** 
 * @brief  Detele chars in string.
 */ 
void
string_delete_chars (char              *s_src_dst,
                     const ProcessData *pd_data)
{
    size_t  ui_cnt  = pd_data->ui_cnt;    /* Delete chars count */
    size_t  ui_pos  = pd_data->ui_pos;    /* Delete start position */
    size_t  ui_len  = 0;    /* Length of text to process */
    size_t  ui_olen = 0;    /* Length of text before processing */
    char   *ch_po   = NULL; /* Pointer to delete start position */
    char   *ch_cn   = NULL; /* Pointer to position after deleted chars */

    if (ui_cnt == 0)
        return;

    if (g_utf8_validate (s_src_dst, -1, NULL)) {

        ui_len = strlen (s_src_dst);

        ui_olen = ui_len;
        ui_len = (size_t) g_utf8_strlen (s_src_dst, -1);

        if (ui_cnt > ui_len)
            ui_cnt = ui_len;
        if (ui_pos > ui_len)
            ui_pos = ui_len;

        if (ui_pos + ui_cnt > ui_len) {
            if (ui_pos == ui_len)
                ui_pos = ui_len - ui_cnt;
            else
                ui_cnt = ui_len - ui_pos;
        }
        ch_po = g_utf8_offset_to_pointer (s_src_dst, (glong) ui_pos);
        ch_cn = g_utf8_offset_to_pointer (s_src_dst, (glong) (ui_pos + ui_cnt));

        memmove (ch_po, ch_cn, ui_olen - ui_pos - (size_t) (ch_cn - ch_po) + 1);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Insert string to existing string at specified position.
 */
void
string_insert_string (char              *s_src_dst,
                      const ProcessData *pd_data)
{
    char          s_tmp [FN_LEN+1];             /* Temp String */
    size_t        ui_pos     = pd_data->ui_pos; /* Text insert position  */
    size_t        ui_len     = 0;               /* Length of text */
    size_t        ui_slen_u8 = 0;               /* Length of unicode text */
    size_t        i          = 0;
    const size_t  ui_max     = FN_LEN;          /* Max string length */
    const char   *s_ins      = pd_data->s_str1; /* Text to insert */
    const char   *x          = NULL;            /* Insert text pointer */

    if (g_utf8_validate (s_src_dst, -1, NULL) && 
        g_utf8_validate (s_ins, -1, NULL)) {

        char       * __restrict tp = s_tmp;     /* Temp string pointer */
        const char * __restrict sp = s_src_dst; /* Source string pointer */
        const char * __restrict ip = s_ins;     /* Insert string pointer */

        ui_slen_u8 = (size_t) g_utf8_strlen (sp, -1);

        if (ui_pos > ui_slen_u8)
            ui_pos = ui_slen_u8;

        x = g_utf8_offset_to_pointer (sp, (glong) ui_pos);

        while (sp != x) {
            tp[i++] = *sp++;
        }

        while (*ip && i < ui_max) {
            tp[i++] = *ip++;
        }

        while (*sp && i < ui_max) {
            tp[i++] = *sp++;
        }
        tp[i] = '\0';
    }
    else
        return;

    ui_len = get_valid_length (s_tmp, ui_max);

    memcpy (s_src_dst, s_tmp, ui_len);
    s_src_dst[ui_len] = '\0';

    #ifdef DEBUG
        printf ("\n%ld %s\n", strlen (s_tmp), s_tmp);
        printf ("%ld %s\n", strlen (s_src_dst), s_src_dst);
    #endif
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Overwrite string in existing string at specified position.
 */
void
string_overwrite_string (char              *s_src_dst,
                         const ProcessData *pd_data)
{
    char          s_tmp [FN_LEN+1];             /* Temp String */
    size_t        ui_pos     = pd_data->ui_pos; /* Text insert position  */
    size_t        ui_len     = 0;               /* Length of text */
    size_t        ui_ilen    = 0;               /* Length of overwrite text */
    size_t        ui_slen_u8 = 0;               /* Length of unicode text */
    size_t        i          = 0;
    const size_t  ui_max     = FN_LEN;          /* Max string length */
    const char   *s_ins      = pd_data->s_str1; /* Overwrite text */
    const char   *x          = NULL;            /* Overwrite start pointer */

    if (g_utf8_validate (s_src_dst, -1, NULL) && 
        g_utf8_validate (s_ins, -1, NULL)) {

        char       * __restrict tp = s_tmp;     /* Temp string pointer */
        const char * __restrict sp = s_src_dst; /* Source string pointer */
        const char * __restrict ip = s_ins;     /* Insert string pointer */

        ui_slen_u8 = (size_t) g_utf8_strlen (sp, -1);
        ui_ilen    = (size_t) g_utf8_strlen (ip, -1);

        if (ui_pos + (size_t) ui_ilen > ui_slen_u8) {
            if (ui_ilen > ui_slen_u8)
                ui_pos = 0;
            else
                ui_pos = ui_slen_u8 - (size_t) ui_ilen;
        }

        x = g_utf8_offset_to_pointer (sp, (glong) ui_pos);

        while (sp != x) {
            tp[i++] = *sp++;
        }

        while (*ip && i < ui_max) {
            tp[i++] = *ip++;
        }

        if ((size_t) g_utf8_strlen (sp, -1) > ui_ilen) {

            sp = g_utf8_offset_to_pointer (sp, (glong) ui_ilen);

            while (*sp && i < ui_max) {
                tp[i++] = *sp++;
            }
        }
        tp[i] = '\0';
    }
    else
        return;
    
    ui_len = get_valid_length (s_tmp, ui_max);

    memcpy (s_src_dst, s_tmp, ui_len);
    s_src_dst[ui_len] = '\0';

    #ifdef DEBUG
        printf ("\n%ld %s\n", strlen (s_tmp), s_tmp);
        printf ("%ld %s\n", strlen (s_src_dst), s_src_dst);
    #endif
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Add number to string.
 */
void
string_add_number (char              *s_src_dst,
                   const ProcessData *pd_data)
{
    char          s_no  [20];                  /* Temp string for number */
    char          s_tmp [20];                  /* Temp string */
    uint_fast32_t ui_no    = pd_data->i_no;    /* Number to insert */
    uint_fast32_t ui_st    = pd_data->i_start; /* Start numbering position */
    uint_fast32_t ui_mx    = pd_data->i_max;   /* Max number range */
    uint_fast32_t ui_z     = 0;                /* How many zeros to add */
    uint_fast32_t ui_t     = 0;                /* Temp value */
    ProcessData  pd_data2 = { NULL, NULL, 0, 0, 0, 0, 0 };

    memset (s_no,  '\0', sizeof (s_no));
    memset (s_tmp, '\0', sizeof (s_tmp));

    ui_no += ui_st;
    ui_mx += ui_st;

    ui_z = 0;
    ui_t = ui_mx;

    while (ui_t /= 10) {
        ui_z++;
    }

    ui_t = ui_no;

    while (ui_t /= 10) {
        ui_z--;
    }

    for (uint_fast32_t i = 0; i < ui_z; ++i) {
        s_no[i] = '0';
    }

    sprintf (s_tmp, "%" PRIdFAST32, ui_no);
    strcat (s_no, s_tmp);

    pd_data2.s_str1 = s_no;
    pd_data2.ui_pos = pd_data->ui_pos;

    string_insert_string (s_src_dst, &pd_data2);

    #ifdef DEBUG
        printf ("%ld %s\n", strlen (s_src_dst), s_src_dst);
    #endif
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Convert all chars in a given string to lower case.
 */
void
string_to_lower (char              *s_src_dst,
                 const ProcessData *pd_data __attribute__ ((unused)))
{
    char         *s_tt   = NULL;   /* temp string */
    const size_t  ui_max = FN_LEN; /* Max string length */
    size_t        ui_len = 0;      /* Length of name string */

    if (g_utf8_validate (s_src_dst, -1, NULL)) {

        s_tt   = g_utf8_strdown (s_src_dst, -1);
        ui_len = get_valid_length (s_tt, ui_max);
        memcpy (s_src_dst, s_tt, ui_len);
        s_src_dst[ui_len] = '\0';

        #ifdef DEBUG
            printf ("\n%ld %s\n", ui_len, s_tt);
            printf ("%ld %s\n", strlen (s_src_dst), s_src_dst);
        #endif

        g_free (s_tt);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Convert all chars in a given string to upper case.
 */
void
string_to_upper (char              *s_src_dst,
                 const ProcessData *pd_data __attribute__ ((unused)))
{
    char         *s_tt   = NULL;   /* temp string */
    const size_t  ui_max = FN_LEN; /* Max string length */
    size_t        ui_len = 0;      /* Length of name string */

    if (g_utf8_validate (s_src_dst, -1, NULL)) {

        s_tt   = g_utf8_strup(s_src_dst, -1);
        ui_len = get_valid_length (s_tt, ui_max);
        memcpy (s_src_dst, s_tt, ui_len);
        s_src_dst[ui_len] = '\0';

        #ifdef DEBUG
            printf ("\n%ld %s\n", ui_len, s_tt);
            printf ("%ld %s\n", strlen (s_src_dst), s_src_dst);
        #endif

        g_free (s_tt);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Find file name and extension and store in appropriate strings.
 */
void
string_extract_name_ext (const char *s_name_ext,
                         char       *s_name,
                         char       *s_ext)
{
    const char *pn = strrchr (s_name_ext, '.'); /* find first dot from right */

    /* if file is hidden or has no ext copy whole src name as f_name */
    if ((pn == NULL) || (pn == s_name_ext)) {
        strcpy (s_name, s_name_ext); 
    }
    else { /* should be file name with ext */
        size_t ui_len = (size_t) (pn - s_name_ext);
        /* copy extension to f_ext */
        strcpy (s_ext, pn); 
        /* copy name to f_name */
        memcpy (s_name, s_name_ext, ui_len); 
        s_name[ui_len] = '\0';
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Join file name with its extension.
 */
void
string_combine_name_ext (char       *s_name_ext,
                         const char *s_name,
                         const char *s_ext)
{
    size_t       ui_len  = 0;                /* Length of file name */
    const size_t ui_elen = strlen (s_ext);   /* Length of file extension */
    const size_t ui_max  = FN_LEN - ui_elen; /* Maximal length of file name */

    ui_len = get_valid_length (s_name, ui_max);

    if (strcmp (s_ext, "") != 0) { /* extension present */

        memcpy (s_name_ext, s_name, ui_len);
        memcpy (s_name_ext + ui_len, s_ext, ui_elen);
        s_name_ext[ui_len + ui_elen] = '\0';
    }
    else { /* no extenstion */
        memcpy (s_name_ext, s_name, ui_len);
        s_name_ext[ui_len] = '\0';
    }
    #ifdef DEBUG
        printf ("f %s e %s \n", s_name, s_ext);
    #endif
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Function to process file name str using a pointed function.
 */
void
string_process_filename (void             (*fun) (char*, const ProcessData*),
                         char              *s_src_dst,
                         const ProcessData *pd_data,
                         const int8_t       ne)
{
    char f_name [FN_LEN + 1]; /* temp name */
    char f_ext  [FN_LEN + 1]; /* temp extension */

    f_name [0] = '\0';
    f_ext  [0] = '\0';

    if (ne == 2)
        fun (s_src_dst, pd_data); /* change text in name and ext */
    else { /* change text in name or ext */

        /* get name and ext to separate strings */
        string_extract_name_ext (s_src_dst, f_name, f_ext);

        if (ne == 1) fun (f_name, pd_data); /* change name only */
        if (ne == 0) fun (f_ext,  pd_data); /* change ext only */
        /* join name and ext */
        string_combine_name_ext (s_src_dst, f_name, f_ext);
    }
}
/*----------------------------------------------------------------------------*/
