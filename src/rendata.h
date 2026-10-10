/**
 * @file  rendata.h
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
 * @brief  RenData struncture with data to rename files.
 *
 * @author Michal Babik <michal.babik@protonmail.com>
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
 * @var   RenData::rememberopt
 * @brief Remember options on exit option
 */
typedef struct
RenData {
    RFnames   *names;       /* File names, old, new, entries */
    RDelete   *del;         /* Delete chars properties */
    RInsOvr   *ins;         /* Insert text properties */
    RInsOvr   *ovrw;        /* Overwrite text properties */
    RReplace  *replace;     /* Replace string with string settings */
    RNumber   *number;      /* Numbering names settings */
    int8_t     uplo;        /* Upper/lower case option */
    int8_t     spaces;      /* Spaces/underscores option */
    int8_t     applyto;     /* Apply to file names/ext or both option */
    bool       renexit;     /* Exit after rename option */
    int8_t     dirsel;      /* Select files from directory options */
    bool       rememberopt; /* Remember options on exit option */
} RenData;
/*----------------------------------------------------------------------------*/
/**
 * @fn  RenData *rendata_new (void)
 *
 * @brief  Create and return new RenData item.
 *
 * @return RenData item
 *
 * @fn  void rendata_free (RenData *data)
 *
 * @brief  Free allocated memory.
 *
 * @param[out] data RenData object
 * @return     none
 */
/*----------------------------------------------------------------------------*/
[[nodiscard]] RenData * rendata_new  (void)
              __attribute__ ((returns_nonnull));

void      rendata_free (RenData *data);
/*----------------------------------------------------------------------------*/
/**
 * @fn  RFnames * rendata_get_rfnames (const RenData *data)
 *
 * @brief  Get names object with file list.
 *
 * @param[in] data RenData object
 * @return    RFnames item
 *
 * @fn  RDelete * rendata_get_rdelete (const RenData *data)
 *
 * @brief  Get delete chars info structure.
 *
 * @param[in] data RenData object
 * @return    RDelete item
 *
 * @fn  RInsOvr * rendata_get_rinsert (const RenData *data)
 *
 * @brief  Get insert info structure.
 *
 * @param[in] data RenData object
 * @return    RInsOvr item
 *
 * @fn  RInsOvr * rendata_get_roverwr (const RenData *data)
 *
 * @brief  Get overwrite info structure.
 *
 * @param[in] data RenData object
 * @return    RInsOvr item
 *
 * @fn  RReplace * rendata_get_rreplace (const RenData *data)
 *
 * @brief  Get replace strings info structure.
 *
 * @param[in] data RenData object
 * @return    RReplace item
 *
 * @fn  RNumber * rendata_get_rnumber (const RenData *data)
 *
 * @brief  Get numbering strings info structure.
 *
 * @param[in] data RenData object
 * @return    RNumber item
 */
/*----------------------------------------------------------------------------*/
static inline const RFnames * rendata_get_rfnames   (const RenData *data) {
    return (const RFnames *) data->names;
}
static inline const RDelete * rendata_get_rdelete   (const RenData *data) {
    return (const RDelete *) data->del;
}
static inline const RInsOvr * rendata_get_rinsert   (const RenData *data) {
    return (const RInsOvr *) data->ins;
}
static inline const RInsOvr * rendata_get_roverwr   (const RenData *data) {
    return (const RInsOvr *) data->ovrw;
}
static inline const RReplace * rendata_get_rreplace (const RenData *data) {
    return (const RReplace *) data->replace;
}
static inline const RNumber * rendata_get_rnumber   (const RenData *data) {
    return (const RNumber *) data->number;
}
/*----------------------------------------------------------------------------*/
/**
 * @fn  int8_t rendata_get_uplo (const RenData *data)
 *
 * @brief  Get uplo value.
 *
 * @param[in]  data  RenData object
 * @return     uplo value
 *
 * @fn  void rendata_set_uplo (RenData *data,
 *                             int8_t   val)
 * @brief  Set uplo value.
 *
 * @param[out] data  RenData object
 * @param[in]  val      New uplo value
 * @return     none
 *
 * @fn  int8_t rendata_get_spaces (const RenData *data)
 *
 * @brief  Get spaces value.
 *
 * @param[in]  data  RenData object
 * @return     spaces value
 *
 * @fn  void rendata_set_spaces (RenData *data,
 *                               int8_t   val)
 * @brief  Set spaces value.
 *
 * @param[out] data  RenData object
 * @param[in]  val      New spaces value
 * @return     none
 *
 * @fn  int8_t rendata_get_applyto (const RenData *data)
 *
 * @brief  Get applyto value.
 *
 * @param[in]  data  RenData object
 * @return     applyto value
 *
 * @fn  void rendata_set_applyto (RenData *data,
 *                                int8_t   val)
 * @brief  Set applyto value.
 *
 * @param[out] data  RenData object
 * @param[in]  val      New applyto value
 * @return     none
 *
 * @fn  bool rendata_get_renexit (const RenData *data)
 *
 * @brief  Get renexit value.
 *
 * @param[in]  data  RenData object
 * @return     renexit value
 *
 * @fn  void rendata_set_renexit (RenData *data,
 *                                int8_t   val)
 * @brief  Set renexit value.
 *
 * @param[out] data  RenData object
 * @param[in]  val      New renexit value
 * @return     none
 *
 * @fn  int8_t rendata_get_dirsel (const RenData *data)
 *
 * @brief  Get dirsel value.
 *
 * @param[in]  data  RenData object
 * @return     dirsel value
 *
 * @fn  void rendata_set_dirsel  (RenData *data,
 *                                int8_t   val)
 * @brief  Set dirsel value.
 *
 * @param[out] data  RenData object
 * @param[in]  val      New dirsel value
 * @return     none
 *
 * @fn  bool rendata_get_rememberopt (const RenData *data)
 *
 * @brief  Get rememberopt value.
 *
 * @param[in]  data  RenData object
 * @return     rememberopt value
 *
 * @fn  void rendata_set_rememberopt (RenData *data,
 *                                    int8_t   val)
 * @brief  Set rememberopt value.
 *
 * @param[out] data  RenData object
 * @param[in]  val      New rememberopt value
 * @return     none
 */
/*----------------------------------------------------------------------------*/
static inline int8_t rendata_get_uplo    (const RenData *data) {
    return data->uplo;
}
static inline void   rendata_set_uplo    (RenData       *data,
                                          int8_t         val) {
    data->uplo = val;
}
static inline int8_t rendata_get_spaces  (const RenData *data) {
    return data->spaces;
}
static inline void   rendata_set_spaces  (RenData       *data,
                                          int8_t         val) {
    data->spaces = val;
}
static inline int8_t rendata_get_applyto (const RenData *data) {
    return data->applyto;
}
static inline void   rendata_set_applyto (RenData       *data,
                                          int8_t         val) {
    data->applyto = val;
}
static inline bool   rendata_get_renexit (const RenData *data) {
    return data->renexit;
}
static inline void   rendata_set_renexit (RenData       *data,
                                          bool           val) {
    data->renexit = val;
}
static inline int8_t rendata_get_dirsel  (const RenData *data) {
    return data->dirsel;
}
static inline void   rendata_set_dirsel  (RenData       *data,
                                          int8_t         val) {
    data->dirsel = val;
}
static inline bool   rendata_get_rememberopt (const RenData *data) {
    return data->rememberopt;
}
static inline void   rendata_set_rememberopt (RenData       *data,
                                              bool           val) {
    data->rememberopt = val;
}
/*----------------------------------------------------------------------------*/
#endif

