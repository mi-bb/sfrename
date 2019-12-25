/**
 * @file  rendata.h
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
 * @brief  RenData struncture with data to rename files.
 * 
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#ifndef RENDATA_H
#define RENDATA_H

#include <stdint.h>
#include "defs.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Delete text in name settings
 */
typedef struct
RDelete {
    uint8_t   cnt;                 /**< Delete chars count */
    uint8_t   pos;                 /**< Delete chars starting position */
} RDelete;
/**
 * @brief  Insert / Overwrite text in name settings
 */
typedef struct
RInsOvr {
    char      s_text [FN_LEN + 1]; /**< String to insert / Overwrite name */
    uint8_t   pos;                 /**< Position to put string */
} RInsOvr;
/**
 * @brief  Replace strings in name settings
 */
typedef struct
RReplace {
    char      s_from [FN_LEN + 1]; /**< Replace from string */
    char      s_to   [FN_LEN + 1]; /**< Replace to string */
} RReplace;
/**
 * @brief  Number names settigns
 */
typedef struct
RNumber {
    uint8_t   opt;                 /**< Number names or not */
    uint32_t  start;               /**< Start numbering from */
    uint8_t   pos;                 /**< Number position in string */
} RNumber;
/**
 * @brief  File names buffer and entries
 */
typedef struct
RFnames {
    int       cnt;                 /**< Count of file names */
    char    **s_new;               /**< New file names buffer */
    char    **s_org;               /**< Original file names buffer */
} RFnames;
/** 
 * @brief Rename data structure
 */ 
typedef struct
RenData {
    RFnames   names;           /**< File names, old, new, entries */
    RDelete   del;             /**< Delete chars properties */
    RInsOvr   ins;             /**< Insert text properties */
    RInsOvr   overwrite;       /**< Overwrite text properties */
    RReplace  replace;         /**< Replace string with string settings */
    RNumber   number;          /**< Numbering names settings */
    int8_t    uplo;            /**< Upper/lower case option */
    int8_t    spaces;          /**< Spaces/underscores option */
    int8_t    applyto;         /**< Apply to file names/ext or both option */
    int8_t    renexit;         /**< Exit after rename option */
} RenData;
/*----------------------------------------------------------------------------*/
/**
 * @brief  RenData initialization.
 *
 * @param[out] rd_data Pointer to RenData object
 * @return     none
 */
void rendata_init (RenData *rd_data);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free allocated memory.
 *
 * @param[in,out] rd_data Pointer to RenData object
 * @return        none
 */
void rendata_free (RenData *rd_data);
/*----------------------------------------------------------------------------*/
#endif

