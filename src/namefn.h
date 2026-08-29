/**
 * @file  namefn.h
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
 * @brief  Functions to modify file names
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#ifndef NAMEFN_H
#define NAMEFN_H

#include <stdint.h>
#include "rendata.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Delete chars in specified file name.
 *
 * Prepares data and calls delete chars in string function.
 *
 * @param[in,out] rd_data Pointer to RenData with rename process info
 * @param[in]     i       Index of file name to change
 * @return        none
 */
void name_delete_chars        (RenData             *rd_data,
                               const uint_fast32_t  i);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Insert string in specified file name at some position.
 *
 * Prepares data and calls insert string in string function.
 *
 * @param[in,out] rd_data Pointer to RenData with rename process info
 * @param[in]     i       Index of file name to change
 * @return        none
 */
void name_insert_string       (RenData             *rd_data,
                               const uint_fast32_t  i);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Overwrite string in specified file name at some position.
 *
 * Prepares data and calls overwrite string in string function.
 *
 * @param[in,out] rd_data Pointer to RenData with rename process info
 * @param[in]     i       Index of file name to change
 * @return        none
 */
void name_overwrite_string    (RenData             *rd_data,
                               const uint_fast32_t  i);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Number file names.
 *
 * Prepares data and calls numbering string function.
 *
 * @param[in,out] rd_data Pointer to RenData with rename process info
 * @param[in]     i       Index of file name to change
 * @return        none
 */
void name_number_string       (RenData             *rd_data,
                               const uint_fast32_t  i);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Change specified file name to upcase or lowercase.
 *
 * @param[in,out] rd_data Pointer to RenData with rename process info
 * @param[in]     i       Index of file name to change
 * @return        none
 */
void name_to_upcase_lowercase (RenData             *rd_data,
                               const uint_fast32_t  i);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Replace spaces to underscores or vice versa in specified file name to
 *         other.
 *
 * @param[in,out] rd_data Pointer to RenData with rename process info
 * @param[in]     i       Index of file name to change
 * @return        none
 */
void name_spaces_underscores  (RenData             *rd_data,
                               const uint_fast32_t  i);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Replace string in specified file name to other.
 *         If "replace from" string is not empty, perform string replace.
 *
 * @param[in,out] rd_data Pointer to RenData with rename process info
 * @param[in]     i       Index of file name to change
 * @return        none
 */
void name_replace_strings     (RenData             *rd_data,
                               const uint_fast32_t  i);
/*----------------------------------------------------------------------------*/
#endif

