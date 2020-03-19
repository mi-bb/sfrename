/**
 * @file  rreplace.h
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
 * @brief  RReplace struncture and functions.
 *
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#ifndef RREPLACE_H
#define RREPLACE_H

#include <stdint.h>
#include "defs.h"
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
 * @fn         RReplace * rreplace_new (void)
 * @brief      Create new RReplace object.
 * @return     New RReplace object
 *
 * @fn         void rreplace_free (RReplace *r_replace)
 * @brief      Free allocated memory.
 * @param[out] r_replace Pointer to RReplace object
 * @return     none
 */
/*----------------------------------------------------------------------------*/
RReplace * rreplace_new  (void) __attribute__ ((malloc, returns_nonnull));

void       rreplace_free (RReplace *r_replace);
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
 *
 * @fn         int rreplace_empty_from (const RReplace *r_replace)
 * @brief      Check if "from" string is an empty string.
 * @param[in]  r_replace  RReplace object
 * @return     Check result
 *
 * @fn         int rreplace_empty_to (const RReplace *r_replace)
 * @brief      Check if "to" string is an empty string.
 * @param[in]  r_replace  RReplace object
 * @return     Check result
 */
/*----------------------------------------------------------------------------*/
const char * rreplace_get_from   (const RReplace *r_replace);

void         rreplace_set_from   (RReplace       *r_replace,
                                  const char     *val);

const char * rreplace_get_to     (const RReplace *r_replace);

void         rreplace_set_to     (RReplace       *r_replace,
                                  const char     *val);
/*----------------------------------------------------------------------------*/
int          rreplace_empty_from (const RReplace *r_replace)
             __attribute__ ((pure));

int          rreplace_empty_to   (const RReplace *r_replace)
             __attribute__ ((pure));
/*----------------------------------------------------------------------------*/
#endif

