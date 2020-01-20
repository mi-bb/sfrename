/**
 * @file  rendata.h
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
 * @brief  RenData struncture with data to rename files.
 * 
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#ifndef RENDATA_H
#define RENDATA_H

#include <stdint.h>
#include "rfnames.h"
#include "defs.h"
/*----------------------------------------------------------------------------*/
/**
 * @struct RDelete
 * @brief  Delete text in name string settings
 *
 * @var   RDelete::cnt
 * @brief Delete chars count
 *
 * @var   RDelete::pos
 * @brief Delete chars starting position
 */
typedef struct
RDelete {
    uint8_t   cnt;
    uint8_t   pos;
} RDelete;
/*----------------------------------------------------------------------------*/
/**
 * @struct RInsOvr
 * @brief  Insert / Overwrite text in name settings
 *
 * @var   RInsOvr::s_text
 * @brief String to insert / Overwrite name
 *
 * @var   RInsOvr::pos
 * @brief Position to put string
 */
typedef struct
RInsOvr {
    char      s_text [FN_LEN + 1];
    uint8_t   pos;
} RInsOvr;
/*----------------------------------------------------------------------------*/
/**
 * @struct RReplace
 * @brief  Replace strings in name settings
 *
 * @var   RReplace::s_from
 * @brief Replace from string
 *
 * @var   RReplace::s_to
 * @brief Replace to string
 */
typedef struct
RReplace {
    char      s_from [FN_LEN + 1];
    char      s_to   [FN_LEN + 1];
} RReplace;
/*----------------------------------------------------------------------------*/
/**
 * @struct RNumber
 * @brief  Number names settigns
 *
 * @var   RNumber::opt
 * @brief If opt is 0 numbering is disabled, if opt is not 0 it is enabled
 *
 * @var   RNumber::start
 * @brief Start numbering from value start
 *
 * @var   RNumber::pos
 * @brief Number position in string
 */
typedef struct
RNumber {
    uint8_t   opt;
    uint32_t  start;
    uint8_t   pos;
} RNumber;
/*----------------------------------------------------------------------------*/
/** 
 * @struct RenData
 *
 * @brief  Rename data structure
 *
 * Structure with information gathered from application widgets,
 * passed to file name process functions and later used to update
 * widgets data.
 *
 * @var   RenData::names
 * @brief File names, old, new, entries
 * @var   RenData::del
 * @brief Delete chars properties
 * @var   RenData::ins
 * @brief Insert text properties
 * @var   RenData::overwrite
 * @brief Overwrite text properties
 * @var   RenData::replace
 * @brief Replace string with string settings
 * @var   RenData::number
 * @brief Numbering names settings
 * @var   RenData::uplo
 * @brief Upper/lower case option
 * @var   RenData::spaces
 * @brief Spaces/underscores option
 * @var   RenData::applyto
 * @brief Apply to file names/ext or both option
 * @var   RenData::renexit
 * @brief Exit after rename option
 */ 
typedef struct
RenData {
    RFnames   names;           /* File names, old, new, entries */
    RDelete   del;             /* Delete chars properties */
    RInsOvr   ins;             /* Insert text properties */
    RInsOvr   overwrite;       /* Overwrite text properties */
    RReplace  replace;         /* Replace string with string settings */
    RNumber   number;          /* Numbering names settings */
    int8_t    uplo;            /* Upper/lower case option */
    int8_t    spaces;          /* Spaces/underscores option */
    int8_t    applyto;         /* Apply to file names/ext or both option */
    int8_t    renexit;         /* Exit after rename option */
} RenData;
/*----------------------------------------------------------------------------*/
/**
 * @fn         void rendata_init (RenData *rd_data)
 * @brief      RenData initialization.
 * @param[out] rd_data Pointer to RenData object
 * @return     none
 *
 * @fn     RenData *rendata_new (void)
 * @brief  Create and return new RenData item.
 * @return RenData item
 *
 * @fn         void rendata_free (RenData *rd_data)
 * @brief      Free allocated memory.
 * @param[out] rd_data Pointer to RenData object
 * @return     none
 */
/*----------------------------------------------------------------------------*/
void      rendata_init (RenData *rd_data);

RenData * rendata_new  (void) __attribute__ ((malloc, returns_nonnull));

void      rendata_free (RenData *rd_data);
/*----------------------------------------------------------------------------*/
#endif

