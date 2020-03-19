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
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#ifndef RINSOVR_H
#define RINSOVR_H

#include <stdint.h>
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
 */
typedef struct
RInsOvr {
    char      s_text [FN_LEN + 1];
    uint8_t   pos;
} RInsOvr;
/*----------------------------------------------------------------------------*/
/**
 * @fn         RInsOvr * rinsovr_new  (void)
 * @brief      Create new RInsOvr object.
 * @return     New RInsOvr object
 *
 * @fn         void rinsovr_free (RInsOvr *r_insovr)
 * @brief      Free allocated memory.
 * @param[out] r_insovr Pointer to RInsOvr object
 * @return     none
 */
/*----------------------------------------------------------------------------*/
RInsOvr * rinsovr_new  (void) __attribute__ ((malloc, returns_nonnull));

void      rinsovr_free (RInsOvr *r_insovr);
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
*
* @fn         int rinsovr_empty (const RInsOvr *rio_item)
* @brief      Check if insert/overwrite text is an empty string.
* @param[in]  rio_item  RInsOvr object
* @return     Check result
 */
/*----------------------------------------------------------------------------*/
const char * rinsovr_get_text (const RInsOvr *rio_item);

void         rinsovr_set_text (RInsOvr       *rio_item,
                               const char    *val);

uint8_t      rinsovr_get_pos  (const RInsOvr *rio_item) __attribute__ ((pure));

void         rinsovr_set_pos  (RInsOvr       *rio_item,
                               const uint8_t  val);
/*----------------------------------------------------------------------------*/
int          rinsovr_empty    (const RInsOvr *rio_item) __attribute__ ((pure));
/*----------------------------------------------------------------------------*/
#endif

