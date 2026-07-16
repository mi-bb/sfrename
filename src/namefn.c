/**
 * @file  namefn.c
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
 * @brief  Functions to modify file names
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#include <string.h>
#include "strfn.h"
#include "namefn.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Delete chars in specified file name.
 */
void
name_delete_chars (RenData             *rd_data,
                   const uint_fast32_t  i)
{
    ProcessData pd_data = {
        rd_data->names->rf_items[i]->s_new,
        &rd_data->names->rf_items[i]->new_len,
        &rd_data->names->rf_items[i]->new_u8len,
        NULL,
        0,0,
        NULL,
        0,0,
        rdelete_get_pos (rendata_get_rdelete (rd_data)),
        rdelete_get_cnt (rendata_get_rdelete (rd_data)),
        0,
        0,
        0
    };

    /* exit if no chars to delete */
    if (rdelete_get_cnt (rendata_get_rdelete (rd_data))) {
        string_process_filename (string_delete_chars,
                                 &pd_data,
                                 rendata_get_applyto (rd_data));
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Insert string in specified file name at some position.
 */
void
name_insert_string (RenData             *rd_data,
                    const uint_fast32_t  i)
{
    ProcessData pd_data = {
        rd_data->names->rf_items[i]->s_new,
        &rd_data->names->rf_items[i]->new_len,
        &rd_data->names->rf_items[i]->new_u8len,
        rinsovr_get_text (rendata_get_rinsert (rd_data)),
        rinsovr_get_len (rendata_get_rinsert (rd_data)),
        rinsovr_get_u8len (rendata_get_rinsert (rd_data)),
        NULL,
        0,
        0,
        rinsovr_get_pos (rendata_get_rinsert (rd_data)),
        0,
        0,
        0,
        0
    };

    if (!rinsovr_empty (rendata_get_rinsert (rd_data))) {
        string_process_filename (string_insert_string,
                                 &pd_data,
                                 rendata_get_applyto (rd_data));
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Overwrite string in specified file name at some position.
 */
void
name_overwrite_string (RenData             *rd_data,
                       const uint_fast32_t  i)
{
    ProcessData pd_data = {
        rd_data->names->rf_items[i]->s_new,
        &rd_data->names->rf_items[i]->new_len,
        &rd_data->names->rf_items[i]->new_u8len,
        rinsovr_get_text (rendata_get_roverwr (rd_data)),
        rinsovr_get_len (rendata_get_roverwr (rd_data)),
        rinsovr_get_u8len (rendata_get_roverwr (rd_data)),
        NULL,
        0,
        0,
        rinsovr_get_pos (rendata_get_roverwr (rd_data)),
        0,
        0,
        0,
        0
    };

    if (!rinsovr_empty (rendata_get_roverwr (rd_data))) {
        string_process_filename (string_overwrite_string,
                                 &pd_data,
                                 rendata_get_applyto (rd_data));
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Number file names.
 */
void
name_number_string (RenData             *rd_data,
                    const uint_fast32_t  i)
{
    ProcessData pd_data = {
        rd_data->names->rf_items[i]->s_new,
        &rd_data->names->rf_items[i]->new_len,
        &rd_data->names->rf_items[i]->new_u8len,
        NULL,
        0,0,
        NULL,
        0,0,
        rnumber_get_pos (rendata_get_rnumber (rd_data)),
        0,
        i,
        rnumber_get_start (rendata_get_rnumber (rd_data)),
        rfnames_get_cnt (rendata_get_rfnames (rd_data)) - 1
    };

    if (rnumber_get_opt (rendata_get_rnumber (rd_data))) {
        string_process_filename (string_add_number,
                                 &pd_data,
                                 rendata_get_applyto (rd_data));
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Change specified file name to upcase or lowercase.
 */
void
name_to_upcase_lowercase (RenData             *rd_data,
                          const uint_fast32_t  i)
{
    ProcessData pd_data = {
        rd_data->names->rf_items[i]->s_new,
        &rd_data->names->rf_items[i]->new_len,
        &rd_data->names->rf_items[i]->new_u8len,
        NULL, 0,0,
        NULL, 0, 0,
        0, 0, 0, 0, 0 };

    /* to uppercase */
    if (rendata_get_uplo (rd_data) == 0) {
        string_process_filename (string_to_upper,
                                 &pd_data,
                                 rendata_get_applyto (rd_data));
    }
    /* to lowercase */
    else if (rendata_get_uplo (rd_data) == 1) {
        string_process_filename (string_to_lower,
                                 &pd_data,
                                 rendata_get_applyto (rd_data));
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Replace spaces to underscores or vice versa in specified file name to
 *         other.
 */
void
name_spaces_underscores (RenData             *rd_data,
                         const uint_fast32_t  i)
{
    /* underscores to spaces */
    if (rendata_get_spaces (rd_data) == 0) {
        ProcessData pd_data = {
        rd_data->names->rf_items[i]->s_new,
        &rd_data->names->rf_items[i]->new_len,
        &rd_data->names->rf_items[i]->new_u8len,
            "_", 1, 1,
            " ", 1, 1,
            0, 0, 0, 0, 0 };
        string_process_filename (string_replace_in,
                                 &pd_data,
                                 rendata_get_applyto (rd_data));
    }
    /* spaces to underscores */
    else if (rendata_get_spaces (rd_data) == 1) {
        ProcessData pd_data = {
        rd_data->names->rf_items[i]->s_new,
        &rd_data->names->rf_items[i]->new_len,
        &rd_data->names->rf_items[i]->new_u8len,
            " ", 1, 1,
            "_", 1, 1,
            0, 0, 0, 0, 0 };
        string_process_filename (string_replace_in,
                                 &pd_data,
                                 rendata_get_applyto (rd_data));
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Replace string in specified file name to other.
 *         If "replace from" string is not empty, perform string replace.
 */
void
name_replace_strings (RenData             *rd_data,
                      const uint_fast32_t  i)
{
    ProcessData pd_data = {
        rd_data->names->rf_items[i]->s_new,
        &rd_data->names->rf_items[i]->new_len,
        &rd_data->names->rf_items[i]->new_u8len,
        rreplace_get_from (rendata_get_rreplace (rd_data)),
        rreplace_get_from_len (rendata_get_rreplace (rd_data)),
        rreplace_get_from_u8len (rendata_get_rreplace (rd_data)),
        rreplace_get_to (rendata_get_rreplace (rd_data)),
        rreplace_get_to_len (rendata_get_rreplace (rd_data)),
        rreplace_get_to_u8len (rendata_get_rreplace (rd_data)),
        0,
        0,
        0,
        0,
        0
    };

    if (!rreplace_empty_from (rendata_get_rreplace (rd_data))) {
        string_process_filename (string_replace_in,
                                 &pd_data,
                                 rendata_get_applyto (rd_data));
    }
}
/*----------------------------------------------------------------------------*/
