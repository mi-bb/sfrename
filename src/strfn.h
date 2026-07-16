/**
 * @file  strfn.h
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
 * @author Michal Babik <michal.babik@pm.me>
 */
#ifndef STRFN_H
#define STRFN_H

#include <stddef.h>
#include <stdint.h>
#include "rendata.h"
/*----------------------------------------------------------------------------*/
/**
 * @struct ProcessData
 * @brief  Structure with data to string process functions
 *
 * @var   ProcessData::s_srcdst
 * @brief String to process
 * @var   ProcessData::srcdst_len
 * @brief String to process length
 * @var   ProcessData::srcdst_u8len
 * @brief String to process unicode length
 * @var   ProcessData::s_str1
 * @brief Input string 1
 * @var   ProcessData::str1_len
 * @brief String 1 length
 * @var   ProcessData::str1_u8len
 * @brief String 1 unicode length
 * @var   ProcessData::s_str2
 * @brief Input string 2
 * @var   ProcessData::str2_len
 * @brief String 2 length
 * @var   ProcessData::str2_u8len
 * @brief String 2 unicode length
 * @var   ProcessData::ui_pos
 * @brief Position to insert/overwrite/delete
 * @var   ProcessData::ui_cnt
 * @brief Character count
 * @var   ProcessData::ui_no
 * @brief Number
 * @var   ProcessData::ui_start
 * @brief Start numbering value
 * @var   ProcessData::ui_max
 * @brief Maximum numbering range
 */ 
typedef struct
ProcessData {
    char          *s_srcdst;      /* String to process */
    size_t        *srcdst_len;    /* String to process length */
    size_t        *srcdst_u8len;  /* String to process uni length */
    const char    *s_str1;        /* Input string 1 */
    size_t         str1_len;      /* String 1 length */
    size_t         str1_u8len;    /* String 1 uni length */
    const char    *s_str2;        /* Input string 2 */
    size_t         str2_len;      /* String 2 length */
    size_t         str2_u8len;    /* String 2 uni length */
    size_t         ui_pos;        /* Position to insert/overwrite/delete */
    size_t         ui_cnt;        /* Character count */
    uint_fast32_t  ui_no;         /* Number */
    uint_fast32_t  ui_start;      /* Start numbering value */
    uint_fast32_t  ui_max;        /* Maximum numbering range */
} ProcessData;
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get length (not greater than i_max) in bytes for the input string.
 *
 *  Gets length of maximum correct chars in string. End of string should be
 *  a null character. Returned length can not be greater than i_max,
 *  input string may have any length.
 *  Utf8 chars can be multi byte and for desired fixed size string a char may
 *  be cut in half when simple split is used.
 *
 * @param[in] s_str   Input string
 * @param[in] ui_max  Maximum possible bytes of length
 * @return    Length in bytes of correct chars up to ui_max value
 */
size_t string_get_valid_length (const char         *s_str,
                                const size_t        ui_max)
                                __attribute__ ((pure, nonnull (1)));
/*----------------------------------------------------------------------------*/
/** 
 * @brief  Replace one string with another in a given src_dest string.
 *
 * Function searches in s_src_dst string for s_fr and replaces it with s_to
 * string. The final result is copied into s_src_dst string.
 *
 * @param[in]     pd_data    ProcessData object with process values
 * @return        none
 */ 
void   string_replace_in       (const ProcessData  *pd_data);
/*----------------------------------------------------------------------------*/
/** 
 * @brief  Detele chars in string.
 *
 * @param[in]     pd_data    ProcessData object with process values
 * @return        none
 */ 
void   string_delete_chars     (const ProcessData  *pd_data);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Insert string to existing string at specified position.
 *
 * @param[in]     pd_data    ProcessData object with process values
 * @return        none
 */
void   string_insert_string    (const ProcessData  *pd_data);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Overwrite string in existing string at specified position.
 *
 * @param[in]     pd_data    ProcessData object with process values
 * @return        none
 */
void   string_overwrite_string (const ProcessData  *pd_data);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Add number to string.
 *
 * @param[in]     pd_data    ProcessData object with process values
 * @return        none
 */
void   string_add_number       (const ProcessData  *pd_data);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Convert all chars in a given string to lower case.
 *
 * @param[in]     pd_data    ProcessData object with process values
 * @return        none
 */
void   string_to_lower         (const ProcessData  *pd_data);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Convert all chars in a given string to upper case.
 *
 * @param[in]     pd_data    ProcessData object with process values
 * @return        none
 */
void   string_to_upper         (const ProcessData  *pd_data);
/*----------------------------------------------------------------------------*/
/**
 * @fn void string_extract_name_ext (const char   *s_name_ext,
 *                                   char         *s_name,
 *                                   char         *s_ext)
 * @brief  Find file name and extension and store in appropriate strings.
 *
 * Function finds file extension by searching for a first from right comma sign
 * and copy what's left before it to name and right after comma to ext.
 *
 * @param[in]  s_name_ext Pointer to string containing file name with extension
 *                        to process
 * @param[out] s_name     Destination pointer to file name only string
 * @param[out] s_ext      Destination pointer to file extension string
 * @return     none
 *
 * @fn void string_combine_name_ext (char         *s_name_ext,
 *                                   const char   *s_name,
 *                                   const char   *s_ext)
 * @brief      Join file name with its extension.
 * @param[out] s_name_ext Pointer to string where file name and extension will
 *                        be written
 * @param[in]  s_name     Destination pointer to file name string
 * @param[in]  s_ext      Destination pointer to file extension string
 * @return     none
 */
/*----------------------------------------------------------------------------*/
void   string_extract_name_ext (const char   *s_name_ext,
                                char         *s_name,
                                char         *s_ext)
                                __attribute__ ((nonnull (1, 2, 3)));

void   string_combine_name_ext (char         *s_name_ext,
                                const char   *s_name,
                                const char   *s_ext)
                                __attribute__ ((nonnull (1, 2, 3)));
/*----------------------------------------------------------------------------*/
/**
 * @brief  Function to process file name str using a pointed function.
 *
 * Function checks whether it should perform change whole file name, its name
 * or ext and and calls on it passed as value "fun" function.
 *
 * @param[in]     fun       Pointer to a string change function 
 * @param[in]     pd_data   ProcessData object with process values
 * @param[in]     ne        Value tells to change file name, ext or both
 * @return        none
 */
void string_process_filename (void      (*fun) (const ProcessData*),
                              const ProcessData *pd_data,
                              const int8_t       ne);
/*----------------------------------------------------------------------------*/
#endif

