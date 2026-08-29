/**
 * @file  rdelete.h
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
 * @brief  RDelete struncture and functions.
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#ifndef RDELETE_H
#define RDELETE_H

#include <stdint.h>
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
 * @fn  RDelete * rdelete_new (void)
 *
 * @brief  Create new RDelete object.
 *
 * @return  New RDelete object
 *
 * @fn  void rdelete_free (RDelete *r_delete)
 *
 * @brief  Free allocated memory.
 *
 * @param[out] r_delete Pointer to RDelete object
 * @return     none
 */
/*----------------------------------------------------------------------------*/
RDelete * rdelete_new  (void) __attribute__ ((malloc, returns_nonnull));

void      rdelete_free (RDelete *r_delete);
/*----------------------------------------------------------------------------*/
/**
 * @fn  uint8_t rdelete_get_cnt (const RDelete *r_delete)
 *
 * @brief  Get cnt value.
 *
 * @param[in]  r_delete  RDelete object
 * @return     cnt value
 *
 * @fn  void rdelete_set_cnt (RDelete *r_delete,
 *                            uint8_t  val)
 * @brief  Set cnt value.
 *
 * @param[out] r_delete  RDelete object
 * @param[in]  val       New cnt value
 * @return     none
 *
 * @fn  uint8_t rdelete_get_pos (const RDelete *r_delete)
 *
 * @brief  Get pos value.
 *
 * @param[in]  r_delete  RDelete object
 * @return     pos value
 *
 * @fn  void rdelete_set_pos (RDelete *r_delete,
 *                            uint8_t  val)
 * @brief  Set pos value.
 *
 * @param[out] r_delete  RDelete object
 * @param[in]  val       New pos value
 * @return     none
 */
/*----------------------------------------------------------------------------*/
static inline uint8_t rdelete_get_cnt (const RDelete *r_delete) {
    return r_delete->cnt;
}
static inline void    rdelete_set_cnt (RDelete       *r_delete,
                                       uint8_t        val) {
    r_delete->cnt = val;
}
static inline uint8_t rdelete_get_pos (const RDelete *r_delete) {
    return r_delete->pos;
}
static inline void    rdelete_set_pos (RDelete       *r_delete,
                                       uint8_t        val) {
    r_delete->pos = val;
}
/*----------------------------------------------------------------------------*/
#endif

