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
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#include <glib.h>
#include "cres.h"
#include "rendata.h"
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
 * @brief  RInsOvr initialization.
 *
 * @param[out] r_delete Pointer to RInsOvr object
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
 * @brief  RReplace initialization.
 *
 * @param[out] r_delete Pointer to RReplace object
 * @return     none
 */
static void
rreplace_init (RReplace *r_replace)
{
    memset (r_replace->s_from, '\0', sizeof (r_replace->s_from));
    memset (r_replace->s_to,   '\0', sizeof (r_replace->s_to));
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  RNumber initialization.
 *
 * @param[out] r_delete Pointer to RNumber object
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
 * @brief  RenData initialization.
 */
void
rendata_init (RenData *rd_data)
{
    rd_data->uplo    = 2;    /* upper/lower case setting */
    rd_data->spaces  = 2;    /* spaces/underscores setting */
    rd_data->applyto = 2;    /* apply to names/ext/both setting */
    rd_data->renexit = 1;    /* exit after rename setting */

    rfnames_init (&rd_data->names);
    rdelete_init (&rd_data->del);
    rinsovr_init (&rd_data->ins);
    rinsovr_init (&rd_data->overwrite);
    rreplace_init (&rd_data->replace);
    rnumber_init (&rd_data->number);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create and return new RenData item.
 */
RenData *
rendata_new (void)
{
    RenData *rd_new = calloc (1, sizeof (RenData));
    if (rd_new == NULL) {
        fputs ("Alloc error\n", stderr);
        exit (EXIT_FAILURE);
    }
    rendata_init (rd_new);
    return rd_new;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free allocated memory.
 */
void
rendata_free (RenData *rd_data)
{
    rfnames_free (&rd_data->names);
    free (rd_data);
}
/*----------------------------------------------------------------------------*/

