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

#include <gtk/gtk.h>
#include <stdint.h>
#include "rfnames.h"
#include "defs.h"
#include "rdelete.h"
#include "rinsovr.h"
#include "rreplace.h"
#include "rnumber.h"
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
 * @var   RenData::ovrw
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
 * @var   RenData::dirsel
 * @brief Select files from directory options
 */ 
typedef struct
RenData {
    RFnames   *names;    /* File names, old, new, entries */
    RDelete   *del;      /* Delete chars properties */
    RInsOvr   *ins;      /* Insert text properties */
    RInsOvr   *ovrw;     /* Overwrite text properties */
    RReplace  *replace;  /* Replace string with string settings */
    RNumber   *number;   /* Numbering names settings */
    int8_t     uplo;     /* Upper/lower case option */
    int8_t     spaces;   /* Spaces/underscores option */
    int8_t     applyto;  /* Apply to file names/ext or both option */
    int8_t     renexit;  /* Exit after rename option */
    int8_t     dirsel;   /* Select files from directory options */
} RenData;
/*----------------------------------------------------------------------------*/
/**
 * @fn     RenData *rendata_new (void)
 * @brief  Create and return new RenData item.
 * @return RenData item
 *
 * @fn         void rendata_free (RenData *rd_data)
 * @brief      Free allocated memory.
 * @param[out] rd_data RenData object
 * @return     none
 */
/*----------------------------------------------------------------------------*/
RenData * rendata_new  (void) __attribute__ ((returns_nonnull));

void      rendata_free (RenData *rd_data);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get names object with file list.
 *
 * @param[in] rd_data RenData object with RFnames list
 * @return    RFnames item
 */
const RFnames * rendata_get_rfnames (const RenData *rd_data)
                __attribute__ ((pure));
const RDelete * rendata_get_rdelete (const RenData *rd_data)
                __attribute__ ((pure));
const RInsOvr * rendata_get_rinsert (const RenData *rd_data)
                __attribute__ ((pure));
const RInsOvr * rendata_get_roverwr (const RenData *rd_data)
                __attribute__ ((pure));
const RReplace * rendata_get_rreplace (const RenData *rd_data)
                __attribute__ ((pure));
const RNumber * rendata_get_rnumber (const RenData *rd_data)
                __attribute__ ((pure));
/*----------------------------------------------------------------------------*/
/**
 * @fn         int8_t rendata_get_uplo (const RenData *rd_data)
 * @brief      Get uplo value.
 * @param[in]  rd_data  RenData object
 * @return     uplo value
 *
 * @fn         void rendata_set_uplo (RenData *rd_data,
 *                                    int8_t   val)
 * @brief      Set uplo value.
 * @param[out] rd_data  RenData object
 * @param[in]  val      New uplo value
 * @return     none
 *
 * @fn         int8_t rendata_get_spaces (const RenData *rd_data)
 * @brief      Get spaces value.
 * @param[in]  rd_data  RenData object
 * @return     spaces value
 *
 * @fn         void rendata_set_spaces (RenData *rd_data,
 *                                      int8_t   val)
 * @brief      Set spaces value.
 * @param[out] rd_data  RenData object
 * @param[in]  val      New spaces value
 * @return     none
 *
 * @fn         int8_t rendata_get_applyto (const RenData *rd_data)
 * @brief      Get applyto value.
 * @param[in]  rd_data  RenData object
 * @return     applyto value
 *
 * @fn         void rendata_set_applyto (RenData *rd_data,
 *                                       int8_t   val)
 * @brief      Set applyto value.
 * @param[out] rd_data  RenData object
 * @param[in]  val      New applyto value
 * @return     none
 *
 * @fn         int8_t rendata_get_renexit (const RenData *rd_data)
 * @brief      Get renexit value.
 * @param[in]  rd_data  RenData object
 * @return     renexit value
 *
 * @fn         void rendata_set_renexit (RenData *rd_data,
 *                                       int8_t   val)
 * @brief      Set renexit value.
 * @param[out] rd_data  RenData object
 * @param[in]  val      New renexit value
 * @return     none
 *
 * @fn         int8_t rendata_get_dirsel (const RenData *rd_data)
 * @brief      Get dirsel value.
 * @param[in]  rd_data  RenData object
 * @return     dirsel value
 *
 * @fn         void rendata_set_dirsel  (RenData *rd_data,
 *                                       int8_t   val)
 * @brief      Set dirsel value.
 * @param[out] rd_data  RenData object
 * @param[in]  val      New dirsel value
 * @return     none
 */
/*----------------------------------------------------------------------------*/
int8_t    rendata_get_uplo    (const RenData *rd_data) __attribute__ ((pure));

void      rendata_set_uplo    (RenData       *rd_data,
                               int8_t         val);

int8_t    rendata_get_spaces  (const RenData *rd_data) __attribute__ ((pure));

void      rendata_set_spaces  (RenData       *rd_data,
                               int8_t         val);

int8_t    rendata_get_applyto (const RenData *rd_data) __attribute__ ((pure));

void      rendata_set_applyto (RenData       *rd_data,
                               int8_t         val);

int8_t    rendata_get_renexit (const RenData *rd_data) __attribute__ ((pure));

void      rendata_set_renexit (RenData       *rd_data,
                               int8_t         val);

int8_t    rendata_get_dirsel  (const RenData *rd_data) __attribute__ ((pure));

void      rendata_set_dirsel  (RenData       *rd_data,
                               int8_t         val);
/*----------------------------------------------------------------------------*/
#endif

