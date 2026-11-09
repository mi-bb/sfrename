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
#include "string_functions.h"
#include "rinsovr.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  RInsOvr initialization.
 *
 * @param[out] insovr Pointer to RInsOvr object
 * @return     none
 */
static void
rinsovr_init (RInsOvr *insovr)
{
    *insovr = (RInsOvr){};
}
/*----------------------------------------------------------------------------*/
/**
 * @brief   Create new RInsOvr object.
 */
RInsOvr *
rinsovr_new (void)
{
    RInsOvr *insovr = nullptr;

    if ((insovr = malloc (sizeof (RInsOvr))) == nullptr)
        err (EXIT_FAILURE, nullptr);

    rinsovr_init (insovr);
    return insovr;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free allocated memory.
 */
void
rinsovr_free (RInsOvr *insovr)
{
    free (insovr);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set text string.
 */
void 
rinsovr_set_text (RInsOvr    *insovr,
                  const char *val)
{
    size_t len = string_get_valid_length (val, FN_LEN);

    memcpy (insovr->text, val, len);
    insovr->text[len] = '\0';
    insovr->len = len;
    insovr->u8len = (size_t) g_utf8_strlen (val, -1);
}
/*----------------------------------------------------------------------------*/

