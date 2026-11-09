/**
 * @file  rreplace.h
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
 * @brief  RReplace struncture and functions.
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#ifndef RREPLACE_H
#define RREPLACE_H

#include <stddef.h>
#include "defs.h"
/*----------------------------------------------------------------------------*/
/**
 * @struct RReplace
 *
 * @brief  Replace strings in name settings
 *
 * @var   RReplace::from
 * @brief Replace from string
 *
 * @var   RReplace::to
 * @brief Replace to string
 *
 * @var   RReplace::from_len
 * @brief Number of chars (bytes) in from
 *
 * @var   RReplace::from_u8len
 * @brief Number of unicode chars in from
 *
 * @var   RReplace::to_len
 * @brief Number of chars (bytes) in to
 *
 * @var   RReplace::to_u8len
 * @brief Number of unicode chars in to
 */
typedef struct
RReplace {
    char   from [FN_LEN + 1];
    char   to [FN_LEN + 1];
    size_t from_len;
    size_t from_u8len;
    size_t to_len;
    size_t to_u8len;
} RReplace;
/*----------------------------------------------------------------------------*/
/**
 * @fn  RReplace * rreplace_new (void)
 *
 * @brief  Create new RReplace object.
 *
 * @return  New RReplace object
 *
 * @fn  void rreplace_free (RReplace *replace)
 *
 * @brief  Free allocated memory.
 *
 * @param[out] replace Pointer to RReplace object
 * @return     none
 */
/*----------------------------------------------------------------------------*/
[[nodiscard]] RReplace * rreplace_new  (void)
              __attribute__ ((malloc, returns_nonnull));

void       rreplace_free (RReplace *replace);
/*----------------------------------------------------------------------------*/
/**
 * @fn  const char * rreplace_get_from (const RReplace *replace)
 *
 * @brief  Get "from" string.
 *
 * @param[in]  replace  RReplace object
 * @return     "from" string
 *
 * @fn  void rreplace_set_from (RReplace   *replace,
 *                              const char *val)
 * @brief  Set "from" string.
 *
 * @param[out] replace  RReplace object
 * @param[in]  val      New "from" string
 * @return     none
 *
 * @fn  const char * rreplace_get_to (const RReplace *replace)
 *
 * @brief  Get "to" string.
 *
 * @param[in]  replace  RReplace object
 * @return     "to" string
 *
 * @fn  void rreplace_set_to   (RReplace   *replace,
 *                              const char *val)
 * @brief  Set "to" string.
 *
 * @param[out] replace  RReplace object
 * @param[in]  val      New "to" string
 * @return     none
 */
/*----------------------------------------------------------------------------*/
static inline const char * rreplace_get_from (const RReplace *replace) {
    return replace->from;
}
static inline const char * rreplace_get_to   (const RReplace *replace) {
    return replace->to;
}

void                       rreplace_set_from (RReplace       *replace,
                                              const char     *val);

void                       rreplace_set_to   (RReplace       *replace,
                                              const char     *val);
/*----------------------------------------------------------------------------*/
/**
 * @fn  size_t rreplace_get_from_len (const RReplace *replace)
 *
 * @brief  Get from string length value.
 *
 * @param[in]  replace  RReplace object
 * @return     String length in chars (bytes) value
 *
 * @fn  size_t rreplace_get_from_u8len (const RReplace *replace)
 *
 * @brief  Get from string unicode length value.
 *
 * @param[in]  replace  RReplace object
 * @return     String length in unicode chars value
 *
 * @fn  size_t rreplace_get_to_len (const RReplace *replace)
 *
 * @brief  Get to string length value.
 *
 * @param[in]  replace  RReplace object
 * @return     String length in chars (bytes) value
 *
 * @fn  size_t rreplace_get_to_u8len (const RReplace *replace)
 *
 * @brief  Get to string unicode length value.
 *
 * @param[in]  replace  RReplace object
 * @return     String length in unicode chars value
 */
/*----------------------------------------------------------------------------*/
static inline size_t rreplace_get_from_len   (const RReplace *replace) {
    return replace->from_len;
}
static inline size_t rreplace_get_from_u8len (const RReplace *replace) {
    return replace->from_u8len;
}
static inline size_t rreplace_get_to_len     (const RReplace *replace) {
    return replace->to_len;
}
static inline size_t rreplace_get_to_u8len   (const RReplace *replace) {
    return replace->to_u8len;
}
/*----------------------------------------------------------------------------*/
/**
 * @fn  bool rreplace_empty_from (const RReplace *replace)
 *
 * @brief  Check if "from" string is an empty string.
 *
 * @param[in]  replace  RReplace object
 * @return     Check result
 *
 * @fn  bool rreplace_empty_to (const RReplace *replace)
 *
 * @brief  Check if "to" string is an empty string.
 *
 * @param[in]  replace  RReplace object
 * @return     Check result
 */
/*----------------------------------------------------------------------------*/
static inline bool rreplace_empty_from (const RReplace *replace) {
    return (replace->from[0] == '\0');
}
static inline bool rreplace_empty_to   (const RReplace *replace) {
    return (replace->to[0] == '\0');
}
/*----------------------------------------------------------------------------*/
#endif

