/**
 * @file  rendata.c
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
 * @brief  RenData struncture with data to rename files.
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <err.h>
#include "string_functions.h"
#include "rendata.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  RenData initialization.
 *
 * @param[out] data Pointer to RenData object
 */
static void      rendata_init (RenData *data);
/*----------------------------------------------------------------------------*/
/**
 * @brief  RenData initialization.
 */
static void
rendata_init (RenData *data)
{
    *data = (RenData){};
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create and return new RenData item.
 */
RenData *
rendata_new (void)
{
    RenData *data = nullptr;

    if ((data = malloc (sizeof (RenData))) == nullptr)
        err (EXIT_FAILURE, nullptr);

    rendata_init (data);

    data->uplo        = DEF_UPLO;
    data->spaces      = DEF_SPACES;
    data->applyto     = DEF_APPLTO;
    data->renexit     = DEF_RENEXT;
    data->dirsel      = DEF_DIRSEL;
    data->rememberopt = DEF_REMEMBEROPT;
    data->names       = rfnames_new ();
    data->del         = rdelete_new ();
    data->ins         = rinsovr_new ();
    data->ovrw        = rinsovr_new ();
    data->replace     = rreplace_new ();
    data->number      = rnumber_new ();

    return data;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free allocated memory.
 */
void
rendata_free (RenData *data)
{
    rfnames_free  (data->names);
    rdelete_free  (data->del);
    rinsovr_free  (data->ins);
    rinsovr_free  (data->ovrw);
    rreplace_free (data->replace);
    rnumber_free  (data->number);
    free (data);
}
/*----------------------------------------------------------------------------*/

