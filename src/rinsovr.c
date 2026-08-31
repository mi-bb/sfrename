/**
 * @file  rinsovr.c
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
 * @brief  RInsOvr struncture and functions.
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <stdlib.h>
#include <string.h>
#include <gmodule.h>
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
    *r_insovr = (RInsOvr){};
}
/*----------------------------------------------------------------------------*/
/**
 * @brief   Create new RInsOvr object.
 */
RInsOvr *
rinsovr_new (void)
{
    RInsOvr *r_ins = nullptr;

    if ((r_ins = malloc (sizeof (RInsOvr))) == nullptr)
        err (EXIT_FAILURE, nullptr);

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
 * @brief  Set text string.
 */
void 
rinsovr_set_text (RInsOvr    *rio_item,
                  const char *val)
{
    size_t ui_len = string_get_valid_length (val, FN_LEN);

    memcpy (rio_item->s_text, val, ui_len);
    rio_item->s_text[ui_len] = '\0';
    rio_item->len = ui_len;
    rio_item->u8len = (size_t) g_utf8_strlen (val, -1);
}
/*----------------------------------------------------------------------------*/

