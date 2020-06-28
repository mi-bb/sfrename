/**
 * @file  rinsovr.h
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
 * @brief  RInsOvr struncture and functions.
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#ifndef RINSOVR_H
#define RINSOVR_H

#include <stdint.h>
#include <stddef.h>
#include "defs.h"
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
 *
 * @var   RInsOvr::len
 * @brief Number of chars (bytes) in s_text
 *
 * @var   RInsOvr::u8len
 * @brief Number of unicode chars in s_text
 */
typedef struct
RInsOvr {
    char      s_text [FN_LEN + 1];
    uint8_t   pos;
    size_t    len;
    size_t    u8len;
} RInsOvr;
/*----------------------------------------------------------------------------*/
/**
 * @fn  RInsOvr * rinsovr_new  (void)
 *
 * @brief  Create new RInsOvr object.
 *
 * @return     New RInsOvr object
 *
 * @fn  void rinsovr_free (RInsOvr *r_insovr)
 *
 * @brief  Free allocated memory.
 *
 * @param[out] r_insovr Pointer to RInsOvr object
 * @return     none
 */
/*----------------------------------------------------------------------------*/
RInsOvr * rinsovr_new  (void) __attribute__ ((malloc, returns_nonnull));

void      rinsovr_free (RInsOvr *r_insovr);
/*----------------------------------------------------------------------------*/
/**
 * @fn  const char * rinsovr_get_text (const RInsOvr *rio_item)
 *
 * @brief  Get text string.
 *
 * @param[in]  rio_item  RInsOvr object
 * @return     text string
 *
 * @fn  void rinsovr_set_text (RInsOvr    *rio_item,
 *                             const char *val)
 * @brief  Set text string.
 *
 * @param[out] rio_item  RInsOvr object
 * @param[in]  val       New text string
 * @return     none
 *
 * @fn  uint8_t rinsovr_get_pos (const RInsOvr *rio_item)
 *
 * @brief  Get pos value.
 *
 * @param[in]  rio_item  RInsOvr object
 * @return     pos value
 *
 * @fn  void rinsovr_set_pos  (RInsOvr       *rio_item,
 *                             const uint8_t  val)
 * @brief  Set pos value.
 *
 * @param[out] rio_item  RInsOvr object
 * @param[in]  val       New pos value
 * @return     none
 *
 * @fn  size_t rinsovr_get_len (const RInsOvr *rio_item)
 *
 * @brief  Get string length value.
 *
 * @param[in]  rio_item  RInsOvr object
 * @return     String length in chars (bytes) value
 *
 * @fn  size_t rinsovr_get_u8len (const RInsOvr *rio_item)
 *
 * @brief  Get string unicode length value.
 *
 * @param[in]  rio_item  RInsOvr object
 * @return     String length in unicode chars value
 */
/*----------------------------------------------------------------------------*/
static inline const char * rinsovr_get_text  (const RInsOvr *rio_item) {
    return (const char *) rio_item->s_text;
}

void                       rinsovr_set_text  (RInsOvr    *rio_item,
                                              const char *val);

static inline uint8_t      rinsovr_get_pos   (const RInsOvr *rio_item) {
    return rio_item->pos;
}
static inline void         rinsovr_set_pos   (RInsOvr       *rio_item,
                                              const uint8_t  val) {
    rio_item->pos = val;
}
static inline size_t       rinsovr_get_len   (const RInsOvr *rio_item) {
    return rio_item->len;
}
static inline size_t       rinsovr_get_u8len (const RInsOvr *rio_item) {
    return rio_item->u8len;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if insert/overwrite text is an empty string.
 *
 * @param[in]  rio_item  RInsOvr object
 * @return     Check result
 */
static inline int          rinsovr_empty    (const RInsOvr *rio_item) {
    return (rio_item->s_text[0] == '\0');
}
/*----------------------------------------------------------------------------*/
#endif

