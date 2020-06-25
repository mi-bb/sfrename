/**
 * @file  rendata.c
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
 * @brief  RenData struncture with data to rename files.
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#include <err.h>
#include "strfn.h"
#include "rendata.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  RenData initialization.
 *
 * @param[out] rd_data Pointer to RenData object
 */
static void      rendata_init (RenData *rd_data);
/*----------------------------------------------------------------------------*/
/**
 * @brief  RenData initialization.
 */
static void
rendata_init (RenData *rd_data)
{
    rd_data->uplo    = 0;
    rd_data->spaces  = 0;
    rd_data->applyto = 0;
    rd_data->renexit = 0;
    rd_data->dirsel  = 0;
    rd_data->names   = NULL;
    rd_data->del     = NULL;
    rd_data->ins     = NULL;
    rd_data->ovrw    = NULL;
    rd_data->replace = NULL;
    rd_data->number  = NULL;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create and return new RenData item.
 */
RenData *
rendata_new (void)
{
    RenData *rd_data = NULL;

    if ((rd_data = malloc (sizeof (RenData))) == NULL)
        err (EXIT_FAILURE, NULL);

    rendata_init (rd_data);

    rd_data->uplo    = DEF_UPLO;
    rd_data->spaces  = DEF_SPACES;
    rd_data->applyto = DEF_APPLTO;
    rd_data->renexit = DEF_RENEXT;
    rd_data->dirsel  = DEF_DIRSEL;
    rd_data->names   = rfnames_new ();
    rd_data->del     = rdelete_new ();
    rd_data->ins     = rinsovr_new ();
    rd_data->ovrw    = rinsovr_new ();
    rd_data->replace = rreplace_new ();
    rd_data->number  = rnumber_new ();

    return rd_data;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free allocated memory.
 */
void
rendata_free (RenData *rd_data)
{
    rfnames_free (rd_data->names);
    rdelete_free (rd_data->del);
    rinsovr_free (rd_data->ins);
    rinsovr_free (rd_data->ovrw);
    rreplace_free (rd_data->replace);
    rnumber_free (rd_data->number);
    free (rd_data);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get names object with file list.
 */
const RFnames *
rendata_get_rfnames (const RenData *rd_data)
{
    return (const RFnames *) rd_data->names;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get delete chars info structure.
 */
const RDelete *
rendata_get_rdelete (const RenData *rd_data)
{
    return (const RDelete *) rd_data->del;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get insert info structure.
 */
const RInsOvr *
rendata_get_rinsert (const RenData *rd_data)
{
    return (const RInsOvr *) rd_data->ins;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get overwrite info structure.
 */
const RInsOvr *
rendata_get_roverwr (const RenData *rd_data)
{
    return (const RInsOvr *) rd_data->ovrw;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get replace strings info structure.
 */
const RReplace *
rendata_get_rreplace (const RenData *rd_data)
{
    return (const RReplace *) rd_data->replace;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get numbering strings info structure.
 */
const RNumber *
rendata_get_rnumber (const RenData *rd_data)
{
    return (const RNumber *) rd_data->number;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get uplo value.
 */
int8_t
rendata_get_uplo (const RenData *rd_data)
{
    return rd_data->uplo;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set uplo value.
 */
void
rendata_set_uplo (RenData *rd_data,
                  int8_t   val)
{
    rd_data->uplo = val;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get spaces value.
 */
int8_t
rendata_get_spaces (const RenData *rd_data)
{
    return rd_data->spaces;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set spaces value.
 */
void
rendata_set_spaces (RenData *rd_data,
                    int8_t   val)
{
    rd_data->spaces = val;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get applyto value.
 */
int8_t
rendata_get_applyto (const RenData *rd_data)
{
    return rd_data->applyto;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set applyto value.
 */
void
rendata_set_applyto (RenData *rd_data,
                     int8_t   val)
{
    rd_data->applyto = val;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get renexit value.
 */
int8_t
rendata_get_renexit (const RenData *rd_data)
{
    return rd_data->renexit;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set renexit value.
 */
void
rendata_set_renexit (RenData *rd_data,
                     int8_t   val)
{
    rd_data->renexit = val;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Get dirsel value.
 */
int8_t
rendata_get_dirsel (const RenData *rd_data)
{
    return rd_data->dirsel;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Set dirsel value.
 */
void
rendata_set_dirsel (RenData *rd_data,
                    int8_t   val)
{
    rd_data->dirsel = val;
}
/*----------------------------------------------------------------------------*/

