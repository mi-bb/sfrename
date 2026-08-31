/**
 * @file  rreplace.c
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
#include <stdlib.h>
#include <string.h>
#include <gmodule.h>
#include <err.h>
#include "strfn.h"
#include "rreplace.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  RReplace initialization.
 *
 * @param[out] r_replace Pointer to RReplace object
 * @return     none
 */
static void
rreplace_init (RReplace *r_replace)
{
    *r_replace = (RReplace){};
}
/*----------------------------------------------------------------------------*/
/**
 * @brief   Create new RReplace object.
 */
RReplace *
rreplace_new (void)
{
    RReplace *r_rep = nullptr;

    if ((r_rep = malloc (sizeof (RReplace))) == nullptr)
        err (EXIT_FAILURE, nullptr);

    rreplace_init (r_rep);
    return r_rep;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free allocated memory.
 */
void
rreplace_free (RReplace *r_replace)
{
    free (r_replace);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set "from" string.
 */
void
rreplace_set_from (RReplace   *r_replace,
                   const char *val)
{
    size_t ui_len = string_get_valid_length (val, FN_LEN);

    memcpy (r_replace->s_from, val, ui_len);
    r_replace->s_from[ui_len] = '\0';
    r_replace->from_len   = ui_len;
    r_replace->from_u8len = (size_t) g_utf8_strlen (val, -1);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set "to" string.
 */
void
rreplace_set_to (RReplace   *r_replace,
                 const char *val)
{
    size_t ui_len = string_get_valid_length (val, FN_LEN);

    memcpy (r_replace->s_to, val, ui_len);
    r_replace->s_to[ui_len] = '\0';
    r_replace->to_len   = ui_len;
    r_replace->to_u8len = (size_t) g_utf8_strlen (val, -1);
}
/*----------------------------------------------------------------------------*/

