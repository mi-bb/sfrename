/**
 * @file  namefn.c
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
 * @brief  Functions to modify file names
 * 
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#include <string.h>
#include "strfn.h"
#include "rendata.h"
#include "namefn.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Delete chars in specified file name.
 */
void
name_delete_chars (RenData        *rd_data,
                   const uint16_t  i)
{
    ProcessData pd_data = {
        NULL,
        NULL,
        rd_data->del.pos,
        rd_data->del.cnt,
        0,
        0,
        0
    };

    /* exit if no chars to delete */
    if (rd_data->del.cnt == 0)
        return;

    string_process_filename (string_delete_chars,
                             rd_data->names.s_new[i],
                             &pd_data,
                             rd_data->applyto);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Insert string in specified file name at some position.
 */
void
name_insert_string (RenData        *rd_data,
                    const uint16_t  i)
{
    ProcessData pd_data = {
        rd_data->ins.s_text,
        NULL,
        rd_data->ins.pos,
        0,
        0,
        0,
        0
    };

    if (strcmp (rd_data->ins.s_text, "") == 0)
        return;

    string_process_filename (string_insert_string,
                             rd_data->names.s_new[i],
                             &pd_data,
                             rd_data->applyto);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Overwrite string in specified file name at some position.
 */
void
name_overwrite_string (RenData        *rd_data,
                       const uint16_t  i)
{
    ProcessData pd_data = {
        rd_data->overwrite.s_text,
        NULL,
        rd_data->overwrite.pos,
        0,
        0,
        0,
        0
    };

    if (strcmp (rd_data->overwrite.s_text, "") == 0)
        return;

    string_process_filename (string_overwrite_string,
                             rd_data->names.s_new[i],
                             &pd_data,
                             rd_data->applyto);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Number file names.
 */
void
name_number_string (RenData        *rd_data,
                    const uint16_t  i)
{
    ProcessData pd_data = {
        NULL,
        NULL,
        rd_data->number.pos,
        0,
        i,
        rd_data->number.start,
        (unsigned int) rd_data->names.cnt - 1
    };

    if (rd_data->number.opt) {
        string_process_filename (string_add_number,
                                 rd_data->names.s_new[i],
                                 &pd_data,
                                 rd_data->applyto);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Change specified file name to upcase or lowercase.
 */
void
name_to_upcase_lowercase (RenData        *rd_data,
                          const uint16_t  i)
{
    ProcessData pd_data = { NULL, NULL, 0, 0, 0, 0, 0 };

    /* to uppercase */
    if (rd_data->uplo == 0) {
        string_process_filename (string_to_upper,
                                 rd_data->names.s_new[i],
                                 &pd_data,
                                 rd_data->applyto);
    }

    /* to lowercase */
    if (rd_data->uplo == 1) {
        string_process_filename (string_to_lower,
                                 rd_data->names.s_new[i],
                                 &pd_data,
                                 rd_data->applyto);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Replace spaces to underscores or vice versa in specified file name to
 *         other.
 */
void
name_spaces_underscores (RenData        *rd_data,
                         const uint16_t  i)
{
    /* underscores to spaces */
    if (rd_data->spaces == 0) {
        ProcessData pd_data = { "_", " ", 0, 0, 0, 0, 0 };
        string_process_filename (string_replace_in,
                                 rd_data->names.s_new[i],
                                 &pd_data,
                                 rd_data->applyto);
    }
    /* spaces to underscores */
    if (rd_data->spaces == 1) {
        ProcessData pd_data = { " ", "_", 0, 0, 0, 0, 0 };
        string_process_filename (string_replace_in,
                                 rd_data->names.s_new[i],
                                 &pd_data,
                                 rd_data->applyto);
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Replace string in specified file name to other.
 *         If "replace from" string is not empty, perform string replace.
 */
void
name_replace_strings (RenData        *rd_data,
                      const uint16_t  i)
{
    ProcessData pd_data = {
        rd_data->replace.s_from,
        rd_data->replace.s_to,
        0,
        0,
        0,
        0,
        0
    };

    if (strcmp (rd_data->replace.s_from, "") != 0) {
        string_process_filename (string_replace_in,
                                 rd_data->names.s_new[i],
                                 &pd_data,
                                 rd_data->applyto);
    }
}
/*----------------------------------------------------------------------------*/
