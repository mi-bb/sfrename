/**
 * @file  strfn.c
 * @copyright Copyright (C) 2019 Michal Babik
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
#include <stdint.h>
#include <stdio.h>
#include "defs.h"
#include "strfn.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get length (not greater than i_max_chars) in bytes for the input
 *         string.
 */
size_t
get_valid_length (const char   *s_str,
                  const size_t  ul_max)
{
    size_t ul_len = 0;

    ul_len = strlen (s_str);

    if (ul_len > ul_max)
        ul_len = ul_max;

    while (!g_utf8_validate (s_str, (long int) ul_len, NULL))
        ul_len--;

    return ul_len;
}
/*----------------------------------------------------------------------------*/
/** 
 * @brief  Replace one string with another in a given src_dest string.
 */ 
void
string_replace_in (char              *s_src_dst,
                   const ProcessData *pd_data)
{
    char          s_tmp [FN_LEN+1];  // temp file name
    const size_t  ul_max   = FN_LEN; // max length of name
    const char   *s_fr     = NULL;   // "replace from" string
    const char   *s_to     = NULL;   // "replace to" string
    const char   *tp       = NULL;   // pointer to s_to
    char         *sp       = NULL;   // copy src_dest pointer
    char         *fp       = NULL;   // find string pointer
    size_t        ul_vlen  = 0;      // valid name length
    uint16_t      i        = 0;
    size_t        ul_frlen = 0;      // rename from length

    memset (s_tmp, 0, sizeof (s_tmp));
    
    s_fr = pd_data->s_str1;
    s_to = pd_data->s_str2;

    if (s_src_dst == NULL || s_fr == NULL || s_to == NULL)
        return;

    sp = s_src_dst;
    fp = strstr (sp, s_fr);

    if (fp == NULL)
        return; 

    ul_frlen = strlen (s_fr);

    while (fp != NULL) {

        tp = s_to;

        while (sp != fp && i < FN_LEN) {
            s_tmp[i++] = *sp++;
        }

        while (*tp && i < FN_LEN) {
            s_tmp[i++] = *tp++;
        }

        /* change source pointer to "after found" */
        sp = fp + ul_frlen; 
        fp = strstr (sp, s_fr);
    }
    while (*sp && i < FN_LEN) {
        s_tmp[i++] = *sp++;
    }

    ul_vlen = get_valid_length (s_tmp, ul_max);

    memcpy (s_src_dst, s_tmp, ul_vlen);
    s_src_dst[ul_vlen] = '\0';

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
    size_t  ul_cnt  = 0;    // Delete chars count
    size_t  ul_pos  = 0;    // Delete start position
    size_t  ul_len  = 0;    // Length of text to process
    size_t  ul_olen = 0;    // Length of text before processing
    char   *ch_po   = NULL; // Pointer to delete start position
    char   *ch_cn   = NULL; // Pointer to position after deleted chars

    ul_cnt = pd_data->ul_cnt;
    ul_pos = pd_data->ul_pos;
    ul_len = strlen (s_src_dst);

    if (ul_cnt == 0)
        return;

    if (g_utf8_validate (s_src_dst, -1, NULL)) {

        ul_olen = ul_len;
        ul_len = (size_t) g_utf8_strlen (s_src_dst, -1);

        if (ul_cnt > ul_len)
            ul_cnt = ul_len;
        if (ul_pos > ul_len)
            ul_pos = ul_len;

        if (ul_pos + ul_cnt > ul_len) {
            if (ul_pos == ul_len)
                ul_pos = ul_len - ul_cnt;
            else
                ul_cnt = ul_len - ul_pos;
        }
        ch_po = g_utf8_offset_to_pointer (s_src_dst, (glong) ul_pos);
        ch_cn = g_utf8_offset_to_pointer (s_src_dst, (glong) (ul_pos + ul_cnt));

        memmove (ch_po, ch_cn, ul_olen - ul_pos - (size_t) (ch_cn - ch_po) + 1);
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
    char          s_tmp [FN_LEN+1];     // Temp String
    size_t        ul_pos     = 0;       // Text insert position 
    size_t        ul_len     = 0;       // Length of text
    const size_t  ul_max     = FN_LEN;  // Max string length
    const char   *s_ins      = NULL;    // Text to insert
    const char   *x          = NULL;    // Insert text pointer
    char         *tp         = NULL;    // Temp string pointer
    const char   *sp         = NULL;    // Source string pointer
    const char   *ip         = NULL;    // Insert string pointer
    size_t        ul_slen_u8 = 0;       // Length of unicode text
    size_t        i          = 0;

    s_ins = pd_data->s_str1;

    if (s_src_dst == NULL || s_ins == NULL)
        return;

    ul_pos     = pd_data->ul_pos;
    ul_slen_u8 = (size_t) g_utf8_strlen (s_src_dst, -1);

    sp = s_src_dst;
    tp = s_tmp;
    ip = s_ins;

    memset (s_tmp, 0, sizeof (s_tmp));

    if (g_utf8_validate (s_src_dst, -1, NULL) && 
        g_utf8_validate (s_ins, -1, NULL)) {

        if (ul_pos > ul_slen_u8)
            ul_pos = ul_slen_u8;

        x = g_utf8_offset_to_pointer (s_src_dst, (glong) ul_pos);

        while (sp != x) {
            tp[i++] = *sp++;
        }

        while (*ip && i < ul_max) {
            tp[i++] = *ip++;
        }

        while (*sp && i < ul_max) {
            tp[i++] = *sp++;
        }

        ul_len = get_valid_length (s_tmp, ul_max);

        memcpy (s_src_dst, s_tmp, ul_len);
        s_src_dst[ul_len] = '\0';

        #ifdef DEBUG
            printf ("\n%ld %s\n", strlen (s_tmp), s_tmp);
            printf ("%ld %s\n", strlen (s_src_dst), s_src_dst);
        #endif
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Overwrite string in existing string at specified position.
 */
void
string_overwrite_string (char              *s_src_dst,
                         const ProcessData *pd_data)
{
    char          s_tmp [FN_LEN+1];     // Temp String
    size_t        ul_pos     = 0;       // Text insert position 
    size_t        ul_len     = 0;       // Length of text
    long          l_ilen     = 0;       // Length of overwrite text
    const size_t  ul_max     = FN_LEN;  // Max string length
    const char   *s_ins      = NULL;    // Overwrite text
    const char   *x          = NULL;    // Overwrite start pointer
    char         *tp         = NULL;    // Temp string pointer
    const char   *sp         = NULL;    // Source string pointer
    const char   *ip         = NULL;    // Insert string pointer
    size_t        ul_slen_u8 = 0;       // Length of unicode text
    size_t        i          = 0;

    s_ins = pd_data->s_str1;

    if (s_src_dst == NULL || s_ins == NULL)
        return;

    ul_pos     = pd_data->ul_pos;
    ul_slen_u8 = (size_t) g_utf8_strlen (s_src_dst, -1);
    l_ilen     = g_utf8_strlen (s_ins, -1);

    sp = s_src_dst;
    tp = s_tmp;
    ip = s_ins;

    memset (s_tmp, 0, sizeof (s_tmp));

    if (g_utf8_validate (s_src_dst, -1, NULL) && 
        g_utf8_validate (s_ins, -1, NULL)) {

        if (ul_pos + (size_t) l_ilen > ul_slen_u8) {
            if (l_ilen > (long) ul_slen_u8)
                ul_pos = 0;
            else
                ul_pos = ul_slen_u8 - (size_t) l_ilen;
        }

        x = g_utf8_offset_to_pointer (s_src_dst, (glong) ul_pos);

        while (sp != x) {
            tp[i++] = *sp++;
        }

        while (*ip && i < ul_max) {
            tp[i++] = *ip++;
        }

        if (g_utf8_strlen (sp, -1) > l_ilen) {

            x = g_utf8_offset_to_pointer (sp, l_ilen);

            while (*x && i < ul_max) {
                tp[i++] = *x++;
            }
        }

        ul_len = get_valid_length (s_tmp, ul_max);

        memcpy (s_src_dst, s_tmp, ul_len);
        s_src_dst[ul_len] = '\0';

        #ifdef DEBUG
            printf ("\n%ld %s\n", strlen (s_tmp), s_tmp);
            printf ("%ld %s\n", strlen (s_src_dst), s_src_dst);
        #endif
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Add number to string.
 */
void
string_add_number (char              *s_src_dst,
                   const ProcessData *pd_data)
{
    char         s_no     [20];  // Temp string for number
    char         s_tmp    [20];  // Temp string
    unsigned int ui_no    = 0;   // Number to insert
    unsigned int ui_st    = 0;   // Start numbering position
    unsigned int ui_mx    = 0;   // Max number range
    unsigned int ui_z     = 0;   // How many zeros should be added
    unsigned int ui_t     = 0;   // Temp value
    ProcessData  pd_data2 = { NULL, NULL, 0, 0, 0, 0, 0 };

    memset (s_no,  0, sizeof (s_no));
    memset (s_tmp, 0, sizeof (s_tmp));

    ui_no = pd_data->i_no;
    ui_st = pd_data->i_start;
    ui_mx = pd_data->i_max;

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

    for (unsigned int i = 0; i < ui_z; ++i) {
        s_no[i] = '0';
    }

    sprintf (s_tmp, "%d", ui_no);
    strcat (s_no, s_tmp);

    pd_data2.s_str1 = s_no;
    pd_data2.ul_pos = pd_data->ul_pos;

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
                 const ProcessData *pd_data)
{
    char         *s_tt   = NULL;   // temp string
    const size_t  ul_max = FN_LEN; // Max string length
    size_t        ul_len = 0;      // Length of name string

    if (s_src_dst == NULL)
        return;

    if (g_utf8_validate (s_src_dst, -1, NULL)) {

        s_tt = g_utf8_strdown (s_src_dst, -1);

        ul_len = get_valid_length (s_tt, ul_max);

        memcpy (s_src_dst, s_tt, ul_len);

        #ifdef DEBUG
            printf ("\n%ld %s\n", ul_len, s_tt);
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
                 const ProcessData *pd_data)
{
    char         *s_tt   = NULL;   // temp string
    const size_t  ul_max = FN_LEN; // Max string length
    size_t        ul_len = 0;      // Length of name string

    if (s_src_dst == NULL)
        return;

    if (g_utf8_validate (s_src_dst, -1, NULL)) {

        s_tt = g_utf8_strup(s_src_dst, -1);

        ul_len = get_valid_length (s_tt, ul_max);

        memcpy (s_src_dst, s_tt, ul_len);

        #ifdef DEBUG
            printf ("\n%ld %s\n", ul_len, s_tt);
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
    const char *pn = strrchr (s_name_ext, '.'); // find first dot from right

    /* if file is hidden or has no ext copy whole src name as f_name */
    if ((pn == NULL) || (pn == s_name_ext)) {
        strcpy (s_name, s_name_ext); 
    }
    else { // should be file name with ext
        strcpy (s_ext, pn); // copy extension to f_ext
        memcpy (s_name, s_name_ext, pn - s_name_ext); // copy name to f_name
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
    size_t       ul_len  = 0;
    const size_t ul_elen = strlen (s_ext);
    const size_t ul_max  = FN_LEN - ul_elen;

    memset (s_name_ext, 0, FN_LEN + 1);

    ul_len = get_valid_length (s_name, ul_max);

    if (s_ext != NULL && strcmp (s_ext, "") != 0) { // extension present

        memcpy (s_name_ext, s_name, ul_len);
        memcpy (s_name_ext + ul_len, s_ext, ul_elen);
    }
    else { // no extenstion
        memcpy (s_name_ext, s_name, ul_len);
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
string_process_filename (void          (*fun) (char*, const ProcessData*),
                         char           *s_src_dst,
                         ProcessData    *pd_data,
                         const int       ne)
{
    char f_name [FN_LEN + 1]; // temp name
    char f_ext  [FN_LEN + 1]; // temp extension

    if (ne == 2)
        fun (s_src_dst, pd_data); // change text in name and ext
    else { // change text in name or ext

        memset (f_name, 0, sizeof (f_name));
        memset (f_ext,  0, sizeof (f_ext));

        /* get name and ext to separate strings */
        string_extract_name_ext (s_src_dst, f_name, f_ext);

        if (ne == 1) fun (f_name, pd_data); // change name only
        if (ne == 0) fun (f_ext,  pd_data); // change ext only

        string_combine_name_ext (s_src_dst, f_name, f_ext); // join name and ext
    }
}
/*----------------------------------------------------------------------------*/
