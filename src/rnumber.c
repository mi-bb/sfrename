/**
 * @file  rnumber.c
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
#include <stdlib.h>
#include <err.h>
#include "rnumber.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  RNumber initialization.
 *
 * @param[out] r_number Pointer to RNumber object
 * @return     none
 */
static void
rnumber_init (RNumber *r_number)
{
    r_number->opt   = 0;
    r_number->start = 0;
    r_number->pos   = 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief   Create new RNumber object.
 */
RNumber *
rnumber_new (void)
{
    RNumber *r_num = NULL;

    if ((r_num = malloc (sizeof (RNumber))) == NULL)
        err (EXIT_FAILURE, NULL);

    rnumber_init (r_num);
    return r_num;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free allocated memory.
 */
void
rnumber_free (RNumber *r_number)
{
    free (r_number);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get opt value.
 */
int8_t
rnumber_get_opt (const RNumber *r_number)
{
    return r_number->opt;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set opt value.
 */
void
rnumber_set_opt (RNumber      *r_number,
                 const int8_t  val)
{
    r_number->opt = val;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get start value.
 */
uint_fast32_t
rnumber_get_start (const RNumber *r_number)
{
    return r_number->start;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set start value.
 */
void
rnumber_set_start (RNumber             *r_number,
                   const uint_fast32_t  val)
{
    r_number->start = val;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get pos value.
 */
uint8_t
rnumber_get_pos (const RNumber *r_number)
{
    return r_number->pos;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set pos value.
 */
void
rnumber_set_pos (RNumber       *r_number,
                 const uint8_t  val)
{
    r_number->pos = val;
}
/*----------------------------------------------------------------------------*/
