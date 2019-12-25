/**
 * @file  rendata.c
 * @copyright Copyright (C) 2019 Michal Babik
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
#include "rendata.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  RenData initialization.
 */
void
rendata_init (RenData *rd_data)
{
    rd_data->uplo          = 2;    // upper/lower case setting
    rd_data->spaces        = 2;    // spaces/underscores setting
    rd_data->applyto       = 2;    // apply to names/ext/both setting
    rd_data->renexit       = 1;    // exit after rename setting
    rd_data->del.cnt       = 0;    // chars to delete
    rd_data->del.pos       = 0;    // delete starting at pos
    rd_data->ins.pos       = 0;    // insert string at pos
    rd_data->overwrite.pos = 0;    // overwrite string at pos
    rd_data->names.cnt     = 0;    // names count
    rd_data->number.opt    = 0;    // numbering names opt
    rd_data->number.start  = 0;    // numbering starts from
    rd_data->number.pos    = 0;    // number position in string
    rd_data->names.s_org   = NULL; // org names list
    rd_data->names.s_new   = NULL; // new names list
    /* clear replace from, to, insert overwrite strings */
    memset (rd_data->replace.s_from,   0, sizeof (rd_data->replace.s_from));
    memset (rd_data->replace.s_to,     0, sizeof (rd_data->replace.s_to));
    memset (rd_data->ins.s_text,       0, sizeof (rd_data->ins.s_text));
    memset (rd_data->overwrite.s_text, 0, sizeof (rd_data->overwrite.s_text));
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free allocated memory.
 */
void
rendata_free (RenData *rd_data)
{
    for (uint16_t j=0; j < rd_data->names.cnt; ++j) {
        g_slice_free1 ( (FN_LEN+1) * sizeof (char), rd_data->names.s_org[j]);
        g_slice_free1 ( (FN_LEN+1) * sizeof (char), rd_data->names.s_new[j]);
    }
    if (rd_data->names.s_new != NULL)
        g_free (rd_data->names.s_new);

    if (rd_data->names.s_org != NULL)
        g_free (rd_data->names.s_org);
}
/*----------------------------------------------------------------------------*/

