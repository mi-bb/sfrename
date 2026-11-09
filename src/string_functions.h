/**
 * @file  string_functions.h
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
#ifndef STRING_FUNCTIONS_H
#define STRING_FUNCTIONS_H

#include <stddef.h>
#include <stdint.h>
#include "rendata.h"
/*----------------------------------------------------------------------------*/
/**
 * @struct ProcessData
 * @brief  Structure with data to string process functions
 *
 * @var   ProcessData::text
 * @brief String to process
 * @var   ProcessData::text_len
 * @brief String to process length
 * @var   ProcessData::text_u8len
 * @brief String to process unicode length
 * @var   ProcessData::first
 * @brief Input string 1
 * @var   ProcessData::first_len
 * @brief String 1 length
 * @var   ProcessData::first_u8len
 * @brief String 1 unicode length
 * @var   ProcessData::second
 * @brief Input string 2
 * @var   ProcessData::second_len
 * @brief String 2 length
 * @var   ProcessData::second_u8len
 * @brief String 2 unicode length
 * @var   ProcessData::pos
 * @brief Position to insert/overwrite/delete
 * @var   ProcessData::count
 * @brief Character count
 * @var   ProcessData::number
 * @brief Number
 * @var   ProcessData::start
 * @brief Start numbering value
 * @var   ProcessData::max
 * @brief Maximum numbering range
 */ 
typedef struct
ProcessData {
    char          *text;         /* String to process */
    size_t        *text_len;     /* String to process length */
    size_t        *text_u8len;   /* String to process uni length */
    const char    *first;        /* Input string 1 */
    size_t         first_len;    /* String 1 length */
    size_t         first_u8len;  /* String 1 uni length */
    const char    *second;       /* Input string 2 */
    size_t         second_len;   /* String 2 length */
    size_t         second_u8len; /* String 2 uni length */
    size_t         pos;          /* Position to insert/overwrite/delete */
    size_t         count;        /* Character count */
    uint_fast32_t  number;       /* Number */
    uint_fast32_t  start;        /* Start numbering value */
    uint_fast32_t  max;          /* Maximum numbering range */
} ProcessData;
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get length (not greater than max) in bytes for the input string.
 *
 *  Gets length of maximum correct chars in string. End of string should be
 *  a null character. Returned length can not be greater than max,
 *  input string may have any length.
 *  Utf8 chars can be multi byte and for desired fixed size string a char may
 *  be cut in half when simple split is used.
 *
 * @param[in] str Input string
 * @param[in] max Maximum possible bytes of length
 * @return    Length in bytes of correct chars up to max value
 */
[[nodiscard]] size_t string_get_valid_length (const char   *str,
                                              const size_t  max)
                     __attribute__ ((pure, nonnull (1)));
/*----------------------------------------------------------------------------*/
/** 
 * @brief  Replace one string with another in a given src_dest string.
 *
 * Function searches in text string for from and replaces it with to
 * string. The final result is copied into text string.
 *
 * @param[in]     process    ProcessData object with process values
 * @return        none
 */ 
void   string_replace_in       (const ProcessData  *process);
/*----------------------------------------------------------------------------*/
/** 
 * @brief  Detele chars in string.
 *
 * @param[in]     process    ProcessData object with process values
 * @return        none
 */ 
void   string_delete_chars     (const ProcessData  *process);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Insert string to existing string at specified position.
 *
 * @param[in]     process    ProcessData object with process values
 * @return        none
 */
void   string_insert_string    (const ProcessData  *process);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Overwrite string in existing string at specified position.
 *
 * @param[in]     process    ProcessData object with process values
 * @return        none
 */
void   string_overwrite_string (const ProcessData  *process);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Add number to string.
 *
 * @param[in]     process    ProcessData object with process values
 * @return        none
 */
void   string_add_number       (const ProcessData  *process);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Convert all chars in a given string to lower case.
 *
 * @param[in]     process    ProcessData object with process values
 * @return        none
 */
void   string_to_lower         (const ProcessData  *process);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Convert all chars in a given string to upper case.
 *
 * @param[in]     process    ProcessData object with process values
 * @return        none
 */
void   string_to_upper         (const ProcessData  *process);
/*----------------------------------------------------------------------------*/
/**
 * @fn void string_extract_name_ext (const char   *name_ext,
 *                                   char         *name,
 *                                   char         *ext)
 * @brief  Find file name and extension and store in appropriate strings.
 *
 * Function finds file extension by searching for a first from right comma sign
 * and copy what's left before it to name and right after comma to ext.
 *
 * @param[in]  name_ext Pointer to string containing file name with extension
 *                        to process
 * @param[out] name     Destination pointer to file name only string
 * @param[out] ext      Destination pointer to file extension string
 * @return     none
 *
 * @fn void string_combine_name_ext (char         *name_ext,
 *                                   const char   *name,
 *                                   const char   *ext)
 * @brief      Join file name with its extension.
 * @param[out] name_ext Pointer to string where file name and extension will
 *                        be written
 * @param[in]  name     Destination pointer to file name string
 * @param[in]  ext      Destination pointer to file extension string
 * @return     none
 */
/*----------------------------------------------------------------------------*/
void   string_extract_name_ext (const char   *name_ext,
                                char         *name,
                                char         *ext)
                                __attribute__ ((nonnull (1, 2, 3)));

void   string_combine_name_ext (char         *name_ext,
                                const char   *name,
                                const char   *ext)
                                __attribute__ ((nonnull (1, 2, 3)));
/*----------------------------------------------------------------------------*/
/**
 * @brief  Function to process file name str using a pointed function.
 *
 * Function checks whether it should perform change whole file name, its name
 * or ext and and calls on it passed as value "fun" function.
 *
 * @param[in]     fun       Pointer to a string change function 
 * @param[in]     process   ProcessData object with process values
 * @param[in]     ne        Value tells to change file name, ext or both
 * @return        none
 */
void string_process_filename (void      (*fun) (const ProcessData*),
                              const ProcessData *process,
                              const int8_t       ne);
/*----------------------------------------------------------------------------*/
#endif

