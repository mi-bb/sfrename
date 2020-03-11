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
/*----------------------------------------------------------------------------*/
/**
 * @struct RDelete
 *
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
 * @fn         uint8_t rdelete_get_cnt (const RDelete *r_delete)
 * @brief      Get cnt value.
 * @param[in]  r_delete  RDelete object
 * @return     cnt value
 *
 * @fn         void rdelete_set_cnt (RDelete *r_delete,
 *                                   uint8_t  val)
 * @brief      Set cnt value.
 * @param[out] r_delete  RDelete object
 * @param[in]  val       New cnt value
 * @return     none
 *
 * @fn         uint8_t rdelete_get_pos (const RDelete *r_delete)
 * @brief      Get pos value.
 * @param[in]  r_delete  RDelete object
 * @return     pos value
 *
 * @fn         void rdelete_set_pos (RDelete *r_delete,
 *                                   uint8_t  val)
 * @brief      Set pos value.
 * @param[out] r_delete  RDelete object
 * @param[in]  val       New pos value
 * @return     none
 */
/*----------------------------------------------------------------------------*/
uint8_t rdelete_get_cnt (const RDelete *r_delete) __attribute__ ((pure));

void    rdelete_set_cnt (RDelete       *r_delete,
                         uint8_t        val);

uint8_t rdelete_get_pos (const RDelete *r_delete) __attribute__ ((pure));

void    rdelete_set_pos (RDelete       *r_delete,
                         uint8_t        val);
/*----------------------------------------------------------------------------*/
/**
 * @struct RInsOvr
 *
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
 * @fn         const char * rinsovr_get_text (const RInsOvr *rio_item)
 * @brief      Get text string.
 * @param[in]  rio_item  RInsOvr object
 * @return     text string
 *
 * @fn         void rinsovr_set_text (RInsOvr    *rio_item,
 *                                    const char *val)
 * @brief      Set text string.
 * @param[out] rio_item  RInsOvr object
 * @param[in]  val       New text string
 * @return     none
 *
 * @fn         uint8_t rinsovr_get_pos (const RInsOvr *rio_item)
 * @brief      Get pos value.
 * @param[in]  rio_item  RInsOvr object
 * @return     pos value
 *
 * @fn         void rinsovr_set_pos  (RInsOvr       *rio_item,
 *                                    const uint8_t  val)
 * @brief      Set pos value.
 * @param[out] rio_item  RInsOvr object
 * @param[in]  val       New pos value
 * @return     none
 */
/*----------------------------------------------------------------------------*/
const char * rinsovr_get_text (const RInsOvr *rio_item);

void         rinsovr_set_text (RInsOvr       *rio_item,
                               const char    *val);

uint8_t      rinsovr_get_pos  (const RInsOvr *rio_item) __attribute__ ((pure));

void         rinsovr_set_pos  (RInsOvr       *rio_item,
                               const uint8_t  val);
/*----------------------------------------------------------------------------*/
/**
 * @struct RReplace
 *
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
 * @fn         const char * rreplace_get_from (const RReplace *r_replace)
 * @brief      Get "from" string.
 * @param[in]  r_replace  RReplace object
 * @return     "from" string
 *
 * @fn         void rreplace_set_from (RReplace   *r_replace,
 *                                     const char *val)
 * @brief      Set "from" string.
 * @param[out] r_replace  RReplace object
 * @param[in]  val        New "from" string
 * @return     none
 *
 * @fn         const char * rreplace_get_to (const RReplace *r_replace)
 * @brief      Get "to" string.
 * @param[in]  r_replace  RReplace object
 * @return     "to" string
 *
 * @fn         void rreplace_set_to   (RReplace   *r_replace,
 *                                     const char *val)
 * @brief      Set "to" string.
 * @param[out] r_replace  RReplace object
 * @param[in]  val        New "to" string
 * @return     none
 */
/*----------------------------------------------------------------------------*/
const char * rreplace_get_from (const RReplace *r_replace);

void         rreplace_set_from (RReplace       *r_replace,
                                const char     *val);

const char * rreplace_get_to   (const RReplace *r_replace);

void         rreplace_set_to   (RReplace       *r_replace,
                                const char     *val);
/*----------------------------------------------------------------------------*/
/**
 * @struct RNumber
 *
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
    int8_t        opt;
    uint_fast32_t start;
    uint8_t       pos;
} RNumber;
/*----------------------------------------------------------------------------*/
/**
 * @fn         int8_t rnumber_get_opt (const RNumber *r_number)
 * @brief      Get opt value.
 * @param[in]  r_number  RNumber object
 * @return     opt value
 *
 * @fn         void rnumber_set_opt (RNumber      *r_number,
 *                                   const int8_t  val)
 * @brief      Set opt value.
 * @param[out] r_number  RNumber object
 * @param[in]  val       New opt value
 * @return     none
 *
 * @fn         uint_fast32_t rnumber_get_start (const RNumber *r_number)
 * @brief      Get start value.
 * @param[in]  r_number  RNumber object
 * @return     start value
 *
 * @fn         void rnumber_set_start (RNumber             *r_number,
 *                                     const uint_fast32_t  val)
 * @brief      Set start value.
 * @param[out] r_number  RNumber object
 * @param[in]  val       New start value
 * @return     none
 *
 * @fn         uint8_t rnumber_get_pos (const RNumber *r_number)
 * @brief      Get pos value.
 * @param[in]  r_number  RNumber object
 * @return     pos value
 *
 * @fn         void rnumber_set_pos (RNumber       *r_number,
 *                                   const uint8_t  val)
 * @brief      Set pos value.
 * @param[out] r_number  RNumber object
 * @param[in]  val       New pos value
 * @return     none
 */
/*----------------------------------------------------------------------------*/
int8_t        rnumber_get_opt   (const RNumber       *r_number)
              __attribute__ ((pure));

void          rnumber_set_opt   (RNumber             *r_number,
                                 const int8_t         val);

uint_fast32_t rnumber_get_start (const RNumber       *r_number)
              __attribute__ ((pure));

void          rnumber_set_start (RNumber             *r_number,
                                 const uint_fast32_t  val);

uint8_t       rnumber_get_pos   (const RNumber       *r_number)
              __attribute__ ((pure));

void          rnumber_set_pos   (RNumber             *r_number,
                                 const uint8_t        val);
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
RFnames * rendata_get_names (const RenData *rd_data) __attribute__ ((pure));
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

