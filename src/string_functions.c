/**
 * @file  string_functions.c
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
#include "string_functions.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get length (not greater than max) in bytes for the input string.
 */
size_t
string_get_valid_length (const char   *str,
                         const size_t  max)
{
    const char *end = nullptr; /* Pointer to last valid char in string */
    size_t      len = 0;       /* Valid length to return */

    if ((len = strlen (str)) > max)
        len = max;

    g_utf8_validate (str, (gssize) len, &end);
    len = (size_t) (end - str);

    return len;
}
/*----------------------------------------------------------------------------*/
/** 
 * @brief  Replace one string with another in a given src_dest string.
 */ 
void
string_replace_in (const ProcessData *process)
{
    char          tmp [FN_LEN+1];                /* temp file name */
    char         *text     = process->text;      /* String to process */
    size_t        len      = 0;                  /* valid name length */
    size_t        i        = 0;                  /* the i */
    const size_t  from_len = process->first_len; /* rename from length */
    const size_t  max      = FN_LEN;             /* max length of name */
    const char   *from     = process->first;     /* "replace from" string */
    const char   *to       = process->second;    /* "replace to" string */
    const char   *fp       = nullptr;            /* find string pointer */
    const char   *sp       = text;               /* copy src_dest pointer */
    char         *dp       = tmp;                /* copy tmp pointer */
    const char   *tp       = nullptr;            /* pointer to to */

    if ((fp = strstr (sp, from)) == nullptr)
        return; 

    while (fp != nullptr) {

        tp = to;

        while (sp != fp && i++ < max)
            *dp++ = *sp++;

        while (*tp && i++ < max)
            *dp++ = *tp++;

        /* change source pointer to "after found" */
        sp = fp + from_len; 
        fp = strstr (sp, from);
    }
    while (*sp && i++ < max)
        *dp++ = *sp++;
    *dp = '\0';
    len = string_get_valid_length (tmp, max);

    memcpy (text, tmp, len);
    text[len] = '\0';

    *process->text_u8len = (size_t) g_utf8_strlen (text, -1);
    *process->text_len = len;

    #ifdef DEBUG
        printf ("\n%ld %s\n", strlen (tmp), tmp);
        printf ("%ld %s\n", strlen (text), text);
    #endif
}
/*----------------------------------------------------------------------------*/
/** 
 * @brief  Detele chars in string.
 */ 
void
string_delete_chars (const ProcessData *process)
{
    char   *text      = process->text;        /* String to process */
    size_t  count     = process->count;       /* Number of chars to delete */
    size_t  pos       = process->pos;         /* Delete start position */
    size_t  src_u8len = *process->text_u8len; /* Uni length of text */
    size_t  src_len   = *process->text_len;   /* Length of text */
    size_t  len       = 0;                    /* Final length of text */
    char   *del_start = nullptr;              /* Ptr to delete start pos */
    char   *del_end   = nullptr;              /* Ptr to pos after deleted */

    if (count == 0)
        return;

    if (count > src_u8len)
        count = src_u8len;
    if (pos > src_u8len)
        pos = src_u8len;

    if (pos + count >= src_u8len) {
        if (pos == src_u8len) {
            pos = src_u8len - count;
        }
        del_start = g_utf8_offset_to_pointer (text, (glong) pos);
        *del_start = '\0';
        *process->text_u8len = pos;
        *process->text_len = (size_t) (del_start - text);
    }
    else {
        del_start = g_utf8_offset_to_pointer (text, (glong) pos);
        del_end = g_utf8_offset_to_pointer (text, (glong) (pos + count));
        len = src_len - (size_t) (del_end - del_start);
        memmove (del_start, del_end, len - pos + 1);
        *process->text_u8len = src_u8len - count;
        *process->text_len = len;
    }
    #ifdef DEBUG
    printf ("%ld %s\n", strlen (text), text);
    #endif
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Insert string to existing string at specified position.
 */
void
string_insert_string (const ProcessData *process)
{
    char         *text      = process->text;        /* String to process */
    size_t        pos       = process->pos;         /* Text insert position */
    size_t        len       = 0;                    /* Final length of text */
    const size_t  max       = FN_LEN;               /* Max string length */
    const size_t  src_len   = *process->text_len;   /* Source text length */
    const size_t  src_u8len = *process->text_u8len; /* Src uni text length */
    const size_t  ins_len   = process->first_len;   /* Insert text length */
    const size_t  ins_u8len = process->first_u8len; /* Insert text length */
    size_t        tail_len  = 0;                    /* Afer insert length */
    const char   *ins       = process->first;       /* Text to insert */
    char         *x         = nullptr;              /* Insert text pointer */
    char         *tail_dst  = nullptr;              /* Ptr to pos + ilen */
    const char   *limit     = text + max;           /* Pointer to max char */
    size_t        copy_len  = 0;                    /* Copy char count */

    if (pos > src_u8len)
        pos = src_u8len;

    x       = g_utf8_offset_to_pointer (text, (glong) pos);
    tail_len = src_len - (size_t) (x - text);
    tail_dst    = x + ins_len;

    if (src_len + ins_len <= max) {
        if (pos < src_u8len) {
            memmove (tail_dst, x, tail_len + 1);
        }
        memcpy (x, ins, ins_len);
        len = src_len + ins_len;
        text[len] = '\0';
        *process->text_len = len;
        *process->text_u8len = src_u8len + ins_u8len;
    }
    else {
        if (pos < src_u8len && tail_dst < limit) {
            copy_len = (tail_dst + tail_len > limit) ? (size_t) (limit - tail_dst) : tail_len;
            memmove (tail_dst, x, copy_len);
        }
        copy_len = (tail_dst > limit) ? (size_t) (limit - x) : ins_len;
        memcpy (x, ins, copy_len);

        len = string_get_valid_length (text, max);
        text[len] = '\0';
        *process->text_len = len;
        *process->text_u8len = pos + (size_t) g_utf8_strlen (x, -1);
    }
    #ifdef DEBUG
    printf ("%ld %s\n", strlen (text), text);
    #endif
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Overwrite string in existing string at specified position.
 */
void
string_overwrite_string (const ProcessData *process)
{
    char          tmp [FN_LEN+1];                   /* Temp String */
    char         *text      = process->text;        /* String to process */
    size_t        pos       = process->pos;         /* Insert text pos */
    size_t        len       = 0;                    /* Length of text */
    const size_t  ins_u8len = process->first_u8len; /* Len of ovr str */
    const size_t  src_u8len = *process->text_u8len; /* Len of src uni str */
    size_t        i         = 0;                    /* i */
    const size_t  max       = FN_LEN;               /* Max string length */
    const char   *ins       = process->first;       /* Overwrite text */
    char         *x         = nullptr;              /* Overwrite start ptr */
    char         *tp        = tmp;                  /* Temp string ptr */
    const char   *sp        = text;                 /* Source string ptr */
    const char   *ip        = ins;                  /* Insert string ptr */

    if (pos > src_u8len)
        pos = ins_u8len > src_u8len ? 0 : src_u8len - ins_u8len;

    x = g_utf8_offset_to_pointer (sp, (glong) pos);

    while (sp != x && i++ < max)
        *tp++ = *sp++;

    while (*ip && i++ < max)
        *tp++ = *ip++;

    if ((size_t) g_utf8_strlen (sp, -1) > ins_u8len) {

        sp = g_utf8_offset_to_pointer (sp, (glong) ins_u8len);

        while (*sp && i++ < max)
            *tp++ = *sp++;
    }
    *tp = '\0';

    len = string_get_valid_length (tmp, max);

    memcpy (text, tmp, len);
    text[len] = '\0';

    *process->text_len = len;
    *process->text_u8len = (size_t) g_utf8_strlen (text, -1);

    #ifdef DEBUG
        printf ("\n%ld %s\n", strlen (tmp), tmp);
        printf ("%ld %s\n", strlen (text), text);
    #endif
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Add number to string.
 */
void
string_add_number (const ProcessData *process)
{
    char          num_str [20] = {};              /* Temp string for number */
    char          tmp [20];                       /* Temp string */
    uint_fast32_t number       = process->number; /* Number to insert */
    uint_fast32_t start        = process->start;  /* Start numbering position */
    uint_fast32_t max          = process->max;    /* Max number range */
    uint_fast32_t zeros        = 0;               /* How many zeros to add */
    uint_fast32_t tmp_num      = 0;               /* Temp value */
    ProcessData  insert = {
        .text       = process->text,
        .text_len   = process->text_len,
        .text_u8len = process->text_u8len,
    };

    number += start;
    max += start;

    zeros = 0;
    tmp_num = max;

    while (tmp_num /= 10)
        zeros++;

    insert.first_len   = zeros + 1;
    insert.first_u8len = zeros + 1;

    tmp_num = number;

    while (tmp_num /= 10)
        zeros--;

    for (uint_fast32_t i = 0; i < zeros; ++i)
        num_str[i] = '0';

    snprintf (tmp, sizeof (tmp), "%" PRIuFAST32, number);
    strcat (num_str, tmp);

    insert.first = num_str;
    insert.pos = process->pos;

    string_insert_string (&insert);

    #ifdef DEBUG
        printf ("%ld %s\n", strlen (process->text), process->text);
    #endif
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Convert all chars in a given string to lower case.
 */
void
string_to_lower (const ProcessData *process)
{
    const size_t  max       = FN_LEN;        /* Max string length */
    char         *converted = nullptr;       /* temp string */
    size_t        len       = 0;             /* Length of name string */
    char         *text      = process->text; /* String to process */

    converted   = g_utf8_strdown (text, -1);
    len = string_get_valid_length (converted, max);
    memcpy (text, converted, len);
    text[len] = '\0';

    if (len != *process->text_len) {
        *process->text_len = len;
        *process->text_u8len = (size_t) g_utf8_strlen (text, -1);
    }
    #ifdef DEBUG
    printf ("\n%ld %s\n", len, converted);
    printf ("%ld %s\n", strlen (text), text);
    #endif

    g_free (converted);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Convert all chars in a given string to upper case.
 */
void
string_to_upper (const ProcessData *process)
{
    char         *converted = nullptr;       /* temp string */
    size_t        len       = 0;             /* Length of name string */
    const size_t  max       = FN_LEN;        /* Max string length */
    char         *text      = process->text; /* String to process */

    converted   = g_utf8_strup(text, -1);
    len = string_get_valid_length (converted, max);
    memcpy (text, converted, len);
    text[len] = '\0';

    if (len != *process->text_len) {
        *process->text_len = len;
        *process->text_u8len = (size_t) g_utf8_strlen (text, -1);
    }
    #ifdef DEBUG
    printf ("\n%ld %s\n", len, converted);
    printf ("%ld %s\n", strlen (text), text);
    #endif

    g_free (converted);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Find file name and extension and store in appropriate strings.
 */
void
string_extract_name_ext (const char *name_ext,
                         char       *name,
                         char       *ext)
{
    const char *pn = strrchr (name_ext, '.'); /* find first dot from right */

    /* if file is hidden or has no ext copy whole src name as f_name */
    if ((pn == nullptr) || (pn == name_ext)) {
        strcpy (name, name_ext); 
    }
    else { /* should be file name with ext */
        size_t len = (size_t) (pn - name_ext);
        /* copy extension to f_ext */
        strcpy (ext, pn); 
        /* copy name to f_name */
        memcpy (name, name_ext, len); 
        name[len] = '\0';
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Join file name with its extension.
 */
void
string_combine_name_ext (char       *name_ext,
                         const char *name,
                         const char *ext)
{
    size_t       len     = 0;                /* Length of file name */
    const size_t ext_len = strlen (ext);     /* Length of file extension */
    const size_t max     = FN_LEN - ext_len; /* Maximal length of file name */

    len = string_get_valid_length (name, max);

    if (strcmp (ext, "") != 0) { /* extension present */

        memcpy (name_ext, name, len);
        memcpy (name_ext + len, ext, ext_len);
        name_ext[len + ext_len] = '\0';
    }
    else { /* no extenstion */
        memcpy (name_ext, name, len);
        name_ext[len] = '\0';
    }
    #ifdef DEBUG
        printf ("f %s e %s \n", name, ext);
    #endif
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Function to process file name str using a pointed function.
 */
void
string_process_filename (void      (*fun) (const ProcessData*),
                         const ProcessData *process,
                         const int8_t       ne)
{
    char  f_name [FN_LEN + 1];  /* temp name */
    char  f_ext [FN_LEN + 1];   /* temp extension */
    char *text = process->text;

    f_name [0] = '\0';
    f_ext  [0] = '\0';

    if (ne == 2) { /* change text in name and ext */
        fun (process); 
    }
    else { /* change text in name or ext */

        /* get name and ext to separate strings */
        string_extract_name_ext (text, f_name, f_ext);

        if (ne == 1) {
            strcpy (text, f_name);
            *process->text_len = strlen (f_name);
            *process->text_u8len = (size_t) g_utf8_strlen (f_name, -1);
            fun (process); /* change name only */
            strcpy (f_name, text);
        }
        if (ne == 0) {
            strcpy (text, f_ext);
            *process->text_len = strlen (f_ext);
            *process->text_u8len = (size_t) g_utf8_strlen (f_ext, -1);
            fun (process); /* change name only */
            strcpy (f_ext, text);
        }
        /* join name and ext */
        string_combine_name_ext (text, f_name, f_ext);
        *process->text_len = strlen (text);
        *process->text_u8len = (size_t) g_utf8_strlen (text, -1);
    }
}
/*----------------------------------------------------------------------------*/
