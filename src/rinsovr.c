/**
 * @file  rinsovr.c
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>
#include "strfn.h"
#include "rinsovr.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  RInsOvr initialization.
 *
 * @param[out] r_insovr Pointer to RInsOvr object
 * @return     none
 */
static void
rinsovr_init (RInsOvr *r_insovr)
{
    r_insovr->pos = 0;
    memset (r_insovr->s_text, '\0', sizeof (r_insovr->s_text));
}
/*----------------------------------------------------------------------------*/
/**
 * @brief   Create new RInsOvr object.
 */
RInsOvr *
rinsovr_new (void)
{
    RInsOvr *r_ins = NULL;

    if ((r_ins = malloc (sizeof (RInsOvr))) == NULL)
        err (EXIT_FAILURE, NULL);

    rinsovr_init (r_ins);
    return r_ins;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free allocated memory.
 */
void
rinsovr_free (RInsOvr *r_insovr)
{
    free (r_insovr);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get text string.
 */
const char *
rinsovr_get_text (const RInsOvr *rio_item)
{
    return (const char *) rio_item->s_text;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set text string.
 */
void 
rinsovr_set_text (RInsOvr    *rio_item,
                  const char *val)
{
    size_t ui_len = string_get_valid_length (val, FN_LEN);

    memcpy (rio_item->s_text, val, ui_len);
    rio_item->s_text[ui_len] = '\0';
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get pos value.
 */
uint8_t
rinsovr_get_pos (const RInsOvr *rio_item)
{
    return rio_item->pos;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set pos value.
 */
void
rinsovr_set_pos (RInsOvr       *rio_item,
                 const uint8_t  val)
{
    rio_item->pos = val;
}
/*----------------------------------------------------------------------------*/
int
rinsovr_empty (const RInsOvr *rio_item)
{
    return (rio_item->s_text[0] == '\0');
}
/*----------------------------------------------------------------------------*/

