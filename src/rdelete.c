/**
 * @file  rdelete.c
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
 * @brief  RDelete struncture and functions.
 *
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#include <stdio.h>
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
 *
 * @return  New RDelete object
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
/**
 * @brief  Get cnt value.
 */
uint8_t
rdelete_get_cnt (const RDelete *r_delete)
{
    return r_delete->cnt;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set cnt value.
 */
void
rdelete_set_cnt (RDelete *r_delete,
                 uint8_t  val)
{
    r_delete->cnt = val;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get pos value.
 */
uint8_t
rdelete_get_pos (const RDelete *r_delete)
{
    return r_delete->pos;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set pos value.
 */
void
rdelete_set_pos (RDelete *r_delete,
                 uint8_t  val)
{
    r_delete->pos = val;
}
/*----------------------------------------------------------------------------*/

