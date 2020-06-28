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
 * @author Michal Babik <michal.babik@pm.me>
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
