/**
 * @file  rnumber.h
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
 * @brief  RNumber struncture and functions.
 *
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#ifndef RNUMBER_H
#define RNUMBER_H

#include <stdint.h>
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
 * @fn  RNumber * rnumber_new (void)
 *
 * @brief  Create new RNumber object.
 *
 * @return New RNumber object
 *
 * @fn  void rnumber_free (RNumber *r_number)
 *
 * @brief  Free allocated memory.
 *
 * @param[out] r_number Pointer to RNumber object
 * @return     none
 */
/*----------------------------------------------------------------------------*/
RNumber * rnumber_new  (void) __attribute__ ((malloc, returns_nonnull));

void      rnumber_free (RNumber *r_number);
/*----------------------------------------------------------------------------*/
/**
 * @fn  int8_t rnumber_get_opt (const RNumber *r_number)
 *
 * @brief  Get opt value.
 *
 * @param[in]  r_number  RNumber object
 * @return     opt value
 *
 * @fn  void rnumber_set_opt (RNumber      *r_number,
 *                            const int8_t  val)
 * @brief  Set opt value.
 *
 * @param[out] r_number  RNumber object
 * @param[in]  val       New opt value
 * @return     none
 *
 * @fn  uint_fast32_t rnumber_get_start (const RNumber *r_number)
 *
 * @brief  Get start value.
 *
 * @param[in]  r_number  RNumber object
 * @return     start value
 *
 * @fn  void rnumber_set_start (RNumber             *r_number,
 *                              const uint_fast32_t  val)
 * @brief  Set start value.
 *
 * @param[out] r_number  RNumber object
 * @param[in]  val       New start value
 * @return     none
 *
 * @fn  uint8_t rnumber_get_pos (const RNumber *r_number)
 *
 * @brief  Get pos value.
 *
 * @param[in]  r_number  RNumber object
 * @return     pos value
 *
 * @fn  void rnumber_set_pos (RNumber       *r_number,
 *                            const uint8_t  val)
 * @brief  Set pos value.
 *
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
#endif

