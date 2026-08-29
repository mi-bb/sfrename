/**
 * @file  strfn.c
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
 * @brief  Functions to modify strings
 *
 * @author Michal Babik <michal.babik@protonmail.com>
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
string_get_valid_length (const char   *s_str,
                         const size_t  ui_max)
{
    const char *s_end  = NULL; /* Pointer to last valid char in string */
    size_t      ui_len = 0;    /* Valid length to return */

    if ((ui_len = strlen (s_str)) > ui_max)
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
string_replace_in (const ProcessData *pd_data)
{
    char          s_tmp [FN_LEN+1];             /* temp file name */
    char         *s_srcdst = pd_data->s_srcdst; /* String to process */
    size_t        ui_len   = 0;                 /* valid name length */
    size_t        i        = 0;                 /* the i */
    const size_t  ui_frlen = pd_data->str1_len; /* rename from length */
    const size_t  ui_max   = FN_LEN;            /* max length of name */
    const char   *s_fr     = pd_data->s_str1;   /* "replace from" string */
    const char   *s_to     = pd_data->s_str2;   /* "replace to" string */
    const char   *fp       = NULL;              /* find string pointer */
    const char   *sp       = s_srcdst;          /* copy src_dest pointer */
    char         *dp       = s_tmp;             /* copy s_tmp pointer */
    const char   *tp       = NULL;              /* pointer to s_to */

    if ((fp = strstr (sp, s_fr)) == NULL)
        return; 

    while (fp != NULL) {

        tp = s_to;

        while (sp != fp && i++ < ui_max)
            *dp++ = *sp++;

        while (*tp && i++ < ui_max)
            *dp++ = *tp++;

        /* change source pointer to "after found" */
        sp = fp + ui_frlen; 
        fp = strstr (sp, s_fr);
    }
    while (*sp && i++ < ui_max)
        *dp++ = *sp++;
    *dp = '\0';
    ui_len = (size_t) (dp - s_tmp);

    ui_len = string_get_valid_length (s_tmp, ui_max);

    memcpy (s_srcdst, s_tmp, ui_len);
    s_srcdst[ui_len] = '\0';

    *pd_data->srcdst_u8len = (size_t) g_utf8_strlen (s_srcdst, -1);
    *pd_data->srcdst_len = ui_len;

    #ifdef DEBUG
        printf ("\n%ld %s\n", strlen (s_tmp), s_tmp);
        printf ("%ld %s\n", strlen (s_srcdst), s_srcdst);
    #endif
}
/*----------------------------------------------------------------------------*/
/** 
 * @brief  Detele chars in string.
 */ 
void
string_delete_chars (const ProcessData *pd_data)
{
    char   *s_srcdst   = pd_data->s_srcdst;      /* String to process */
    size_t  ui_cnt     = pd_data->ui_cnt;        /* Number of chars to delete */
    size_t  ui_pos     = pd_data->ui_pos;        /* Delete start position */
    size_t  ui_slen_u8 = *pd_data->srcdst_u8len; /* Uni length of text */
    size_t  ui_slen    = *pd_data->srcdst_len;   /* Length of text */
    size_t  ui_len     = 0;                      /* Final length of text */
    char   *ch_po      = NULL; /* Pointer to delete start position */
    char   *ch_cn      = NULL; /* Pointer to position after deleted chars */

    if (ui_cnt == 0)
        return;

    if (ui_cnt > ui_slen_u8)
        ui_cnt = ui_slen_u8;
    if (ui_pos > ui_slen_u8)
        ui_pos = ui_slen_u8;

    if (ui_pos + ui_cnt >= ui_slen_u8) {
        if (ui_pos == ui_slen_u8) {
            ui_pos = ui_slen_u8 - ui_cnt;
        }
        ch_po = g_utf8_offset_to_pointer (s_srcdst, (glong) ui_pos);
        *ch_po = '\0';
        *pd_data->srcdst_u8len = ui_pos;
        *pd_data->srcdst_len = (size_t) (ch_po - s_srcdst);
    }
    else {
        ch_po = g_utf8_offset_to_pointer (s_srcdst, (glong) ui_pos);
        ch_cn = g_utf8_offset_to_pointer (s_srcdst, (glong) (ui_pos + ui_cnt));
        ui_len = ui_slen - (size_t) (ch_cn - ch_po);
        memmove (ch_po, ch_cn, ui_len - ui_pos + 1);
        *pd_data->srcdst_u8len = ui_slen_u8 - ui_cnt;
        *pd_data->srcdst_len = ui_len;
    }
    #ifdef DEBUG
    printf ("%ld %s\n", strlen (s_srcdst), s_srcdst);
    #endif
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Insert string to existing string at specified position.
 */
void
string_insert_string (const ProcessData *pd_data)
{
    char         *s_srcdst   = pd_data->s_srcdst;     /* String to process */
    size_t        ui_pos     = pd_data->ui_pos;       /* Text insert position */
    size_t        ui_len     = 0;                     /* Final length of text */
    const size_t  ui_max     = FN_LEN;                /* Max string length */
    const size_t  ui_slen    = *pd_data->srcdst_len;  /* Source text length */
    const size_t  ui_slen_u8 = *pd_data->srcdst_u8len;/* Src uni text length */
    const size_t  ui_ilen    = pd_data->str1_len;     /* Insert text length */
    const size_t  ui_ilen_u8 = pd_data->str1_u8len;   /* Insert text length */
    size_t        ui_xlen    = 0;                     /* Afer insert length */
    const char   *s_ins      = pd_data->s_str1;       /* Text to insert */
    char         *x          = NULL;                  /* Insert text pointer */
    char         *s_xi       = NULL;                  /* Ptr to pos + ilen */
    const char   *s_mx       = s_srcdst + ui_max;     /* Pointer to max char */
    size_t        ui_cnt     = 0;                     /* Copy char count */

    if (ui_pos > ui_slen_u8)
        ui_pos = ui_slen_u8;

    x       = g_utf8_offset_to_pointer (s_srcdst, (glong) ui_pos);
    ui_xlen = ui_slen - (size_t) (x - s_srcdst);
    s_xi    = x + ui_ilen;

    if (ui_slen + ui_ilen <= ui_max) {
        if (ui_pos < ui_slen_u8) {
            memmove (s_xi, x, ui_xlen + 1);
        }
        memcpy (x, s_ins, ui_ilen);
        ui_len = ui_slen + ui_ilen;
        s_srcdst[ui_len] = '\0';
        *pd_data->srcdst_len = ui_len;
        *pd_data->srcdst_u8len = ui_slen_u8 + ui_ilen_u8;
    }
    else {
        if (ui_pos < ui_slen_u8 && s_xi < s_mx) {
            ui_cnt = (s_xi + ui_xlen > s_mx) ? (size_t) (s_mx - s_xi) : ui_xlen;
            memmove (s_xi, x, ui_cnt);
        }
        ui_cnt = (s_xi > s_mx) ? (size_t) (s_mx - x) : ui_ilen;
        memcpy (x, s_ins, ui_cnt);

        ui_len = string_get_valid_length (s_srcdst, ui_max);
        s_srcdst[ui_len] = '\0';
        *pd_data->srcdst_len = ui_len;
        *pd_data->srcdst_u8len = ui_pos + (size_t) g_utf8_strlen (x, -1);
    }
    #ifdef DEBUG
    printf ("%ld %s\n", strlen (s_srcdst), s_srcdst);
    #endif
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Overwrite string in existing string at specified position.
 */
void
string_overwrite_string (const ProcessData *pd_data)
{
    char          s_tmp [FN_LEN+1];                   /* Temp String */
    char         *s_srcdst   = pd_data->s_srcdst;     /* String to process */
    size_t        ui_pos     = pd_data->ui_pos;       /* Insert text pos */
    size_t        ui_len     = 0;                     /* Length of text */
    const size_t  ui_ilen_u8 = pd_data->str1_u8len;   /* Len of ovr str */
    const size_t  ui_slen_u8 = *pd_data->srcdst_u8len;/* Len of src uni str */
    size_t        i          = 0;                     /* i */
    const size_t  ui_max     = FN_LEN;                /* Max string length */
    const char   *s_ins      = pd_data->s_str1;       /* Overwrite text */
    char         *x          = NULL;                  /* Overwrite start ptr */
    char         *tp         = s_tmp;                 /* Temp string ptr */
    const char   *sp         = s_srcdst;              /* Source string ptr */
    const char   *ip         = s_ins;                 /* Insert string ptr */

    if (ui_pos > ui_slen_u8)
        ui_pos = ui_ilen_u8 > ui_slen_u8 ? 0 : ui_slen_u8 - ui_ilen_u8;

    x = g_utf8_offset_to_pointer (sp, (glong) ui_pos);

    while (sp != x && i++ < ui_max)
        *tp++ = *sp++;

    while (*ip && i++ < ui_max)
        *tp++ = *ip++;

    if ((size_t) g_utf8_strlen (sp, -1) > ui_ilen_u8) {

        sp = g_utf8_offset_to_pointer (sp, (glong) ui_ilen_u8);

        while (*sp && i++ < ui_max)
            *tp++ = *sp++;
    }
    *tp = '\0';

    ui_len = string_get_valid_length (s_tmp, ui_max);

    memcpy (s_srcdst, s_tmp, ui_len);
    s_srcdst[ui_len] = '\0';

    *pd_data->srcdst_len = ui_len;
    *pd_data->srcdst_u8len = (size_t) g_utf8_strlen (s_srcdst, -1);

    #ifdef DEBUG
        printf ("\n%ld %s\n", strlen (s_tmp), s_tmp);
        printf ("%ld %s\n", strlen (s_srcdst), s_srcdst);
    #endif
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Add number to string.
 */
void
string_add_number (const ProcessData *pd_data)
{
    char          s_no  [20];                   /* Temp string for number */
    char          s_tmp [20];                   /* Temp string */
    uint_fast32_t ui_no    = pd_data->ui_no;    /* Number to insert */
    uint_fast32_t ui_st    = pd_data->ui_start; /* Start numbering position */
    uint_fast32_t ui_mx    = pd_data->ui_max;   /* Max number range */
    uint_fast32_t ui_z     = 0;                 /* How many zeros to add */
    uint_fast32_t ui_t     = 0;                 /* Temp value */
    ProcessData  pd_data2 = {
        pd_data->s_srcdst,
        pd_data->srcdst_len,
        pd_data->srcdst_u8len,
        NULL, 0, 0, NULL, 0, 0, 0, 0, 0, 0, 0 };

    memset (s_no, '\0', sizeof (s_no));

    ui_no += ui_st;
    ui_mx += ui_st;

    ui_z = 0;
    ui_t = ui_mx;

    while (ui_t /= 10)
        ui_z++;

    pd_data2.str1_len   = ui_z + 1;
    pd_data2.str1_u8len = ui_z + 1;

    ui_t = ui_no;

    while (ui_t /= 10)
        ui_z--;

    for (uint_fast32_t i = 0; i < ui_z; ++i)
        s_no[i] = '0';

    sprintf (s_tmp, "%" PRIuFAST32, ui_no);
    strcat (s_no, s_tmp);

    pd_data2.s_str1 = s_no;
    pd_data2.ui_pos = pd_data->ui_pos;

    string_insert_string (&pd_data2);

    #ifdef DEBUG
        printf ("%ld %s\n", strlen (pd_data->s_srcdst), pd_data->s_srcdst);
    #endif
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Convert all chars in a given string to lower case.
 */
void
string_to_lower (const ProcessData *pd_data)
{
    const size_t  ui_max   = FN_LEN;            /* Max string length */
    char         *s_tt     = NULL;              /* temp string */
    size_t        ui_len   = 0;                 /* Length of name string */
    char         *s_srcdst = pd_data->s_srcdst; /* String to process */

    s_tt   = g_utf8_strdown (s_srcdst, -1);
    ui_len = string_get_valid_length (s_tt, ui_max);
    memcpy (s_srcdst, s_tt, ui_len);
    s_srcdst[ui_len] = '\0';

    if (ui_len != *pd_data->srcdst_len) {
        *pd_data->srcdst_len = ui_len;
        *pd_data->srcdst_u8len = (size_t) g_utf8_strlen (s_srcdst, -1);
    }
    #ifdef DEBUG
    printf ("\n%ld %s\n", ui_len, s_tt);
    printf ("%ld %s\n", strlen (s_srcdst), s_srcdst);
    #endif

    g_free (s_tt);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Convert all chars in a given string to upper case.
 */
void
string_to_upper (const ProcessData *pd_data)
{
    char         *s_tt     = NULL;              /* temp string */
    size_t        ui_len   = 0;                 /* Length of name string */
    const size_t  ui_max   = FN_LEN;            /* Max string length */
    char         *s_srcdst = pd_data->s_srcdst; /* String to process */

    s_tt   = g_utf8_strup(s_srcdst, -1);
    ui_len = string_get_valid_length (s_tt, ui_max);
    memcpy (s_srcdst, s_tt, ui_len);
    s_srcdst[ui_len] = '\0';

    if (ui_len != *pd_data->srcdst_len) {
        *pd_data->srcdst_len = ui_len;
        *pd_data->srcdst_u8len = (size_t) g_utf8_strlen (s_srcdst, -1);
    }
    #ifdef DEBUG
    printf ("\n%ld %s\n", ui_len, s_tt);
    printf ("%ld %s\n", strlen (s_srcdst), s_srcdst);
    #endif

    g_free (s_tt);
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

    ui_len = string_get_valid_length (s_name, ui_max);

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
string_process_filename (void      (*fun) (const ProcessData*),
                         const ProcessData *pd_data,
                         const int8_t       ne)
{
    char  f_name [FN_LEN + 1]; /* temp name */
    char  f_ext  [FN_LEN + 1]; /* temp extension */
    char *s_srcdst = pd_data->s_srcdst;

    f_name [0] = '\0';
    f_ext  [0] = '\0';

    if (ne == 2) { /* change text in name and ext */
        fun (pd_data); 
    }
    else { /* change text in name or ext */

        /* get name and ext to separate strings */
        string_extract_name_ext (s_srcdst, f_name, f_ext);

        if (ne == 1) {
            strcpy (s_srcdst, f_name);
            *pd_data->srcdst_len = strlen (f_name);
            *pd_data->srcdst_u8len = (size_t) g_utf8_strlen (f_name, -1);
            fun (pd_data); /* change name only */
            strcpy (f_name, s_srcdst);
        }
        if (ne == 0) {
            strcpy (s_srcdst, f_ext);
            *pd_data->srcdst_len = strlen (f_ext);
            *pd_data->srcdst_u8len = (size_t) g_utf8_strlen (f_ext, -1);
            fun (pd_data); /* change name only */
            strcpy (f_ext, s_srcdst);
        }
        /* join name and ext */
        string_combine_name_ext (s_srcdst, f_name, f_ext);
        *pd_data->srcdst_len = strlen (s_srcdst);
        *pd_data->srcdst_u8len = (size_t) g_utf8_strlen (s_srcdst, -1);
    }
}
/*----------------------------------------------------------------------------*/
