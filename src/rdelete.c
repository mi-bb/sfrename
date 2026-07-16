/**
 * @file  rdelete.c
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
 * @author Michal Babik <michal.babik@pm.me>
 */
#include <stdlib.h>
#include <err.h>
#include "rdelete.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  RDelete initialization.
 *
 * @param[out] r_delete Pointer to RDelete object
 * @return     none
 */
static void
rdelete_init (RDelete *r_delete)
{
    r_delete->cnt = 0;
    r_delete->pos = 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief   Create new RDelete object.
 */
RDelete *
rdelete_new (void)
{
    RDelete *rd_new = NULL;

    if ((rd_new = malloc (sizeof (RDelete))) == NULL)
        err (EXIT_FAILURE, NULL);

    rdelete_init (rd_new);
    return rd_new;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free allocated memory.
 */
void
rdelete_free (RDelete *r_delete)
{
    free (r_delete);
}
/*----------------------------------------------------------------------------*/

