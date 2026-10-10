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
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#include <string.h>
#include "string_functions.h"
#include "namefn.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Delete chars in specified file name.
 */
void
name_delete_chars (RenData             *data,
                   const uint_fast32_t  i)
{
    auto rdel = rendata_get_rdelete (data);
    ProcessData process = {
        .text       =  data->names->rf_items[i]->s_new,
        .text_len   = &data->names->rf_items[i]->new_len,
        .text_u8len = &data->names->rf_items[i]->new_u8len,
        .pos        =  rdelete_get_pos (rdel),
        .count      =  rdelete_get_cnt (rdel),
    };

    /* exit if no chars to delete */
    if (rdelete_get_cnt (rdel)) {
        string_process_filename (string_delete_chars,
                                 &process,
                                 rendata_get_applyto (data));
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Insert string in specified file name at some position.
 */
void
name_insert_string (RenData             *data,
                    const uint_fast32_t  i)
{
    auto rins = rendata_get_rinsert (data);
    ProcessData process = {
        .text        =  data->names->rf_items[i]->s_new,
        .text_len    = &data->names->rf_items[i]->new_len,
        .text_u8len  = &data->names->rf_items[i]->new_u8len,
        .first       =  rinsovr_get_text  (rins),
        .first_len   =  rinsovr_get_len   (rins),
        .first_u8len =  rinsovr_get_u8len (rins),
        .pos         =  rinsovr_get_pos   (rins),
    };

    if (!rinsovr_empty (rins)) {
        string_process_filename (string_insert_string,
                                 &process,
                                 rendata_get_applyto (data));
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Overwrite string in specified file name at some position.
 */
void
name_overwrite_string (RenData             *data,
                       const uint_fast32_t  i)
{
    auto riovr = rendata_get_roverwr (data);
    ProcessData process = {
        .text        =  data->names->rf_items[i]->s_new,
        .text_len    = &data->names->rf_items[i]->new_len,
        .text_u8len  = &data->names->rf_items[i]->new_u8len,
        .first       =  rinsovr_get_text  (riovr),
        .first_len   =  rinsovr_get_len   (riovr),
        .first_u8len =  rinsovr_get_u8len (riovr),
        .pos         =  rinsovr_get_pos   (riovr),
    };

    if (!rinsovr_empty (riovr)) {
        string_process_filename (string_overwrite_string,
                                 &process,
                                 rendata_get_applyto (data));
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Number file names.
 */
void
name_number_string (RenData             *data,
                    const uint_fast32_t  i)
{
    auto rnum = rendata_get_rnumber (data);
    ProcessData process = {
        .text       =  data->names->rf_items[i]->s_new,
        .text_len   = &data->names->rf_items[i]->new_len,
        .text_u8len = &data->names->rf_items[i]->new_u8len,
        .pos        =  rnumber_get_pos   (rnum),
        .number     =  i,
        .start      =  rnumber_get_start (rnum),
        .max        =  rfnames_get_cnt (rendata_get_rfnames (data)) - 1,
    };

    if (rnumber_get_opt (rnum)) {
        string_process_filename (string_add_number,
                                 &process,
                                 rendata_get_applyto (data));
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Change specified file name to upcase or lowercase.
 */
void
name_to_upcase_lowercase (RenData             *data,
                          const uint_fast32_t  i)
{
    ProcessData process = {
        .text       =  data->names->rf_items[i]->s_new,
        .text_len   = &data->names->rf_items[i]->new_len,
        .text_u8len = &data->names->rf_items[i]->new_u8len,
    };

    /* to uppercase */
    if (rendata_get_uplo (data) == 0) {
        string_process_filename (string_to_upper,
                                 &process,
                                 rendata_get_applyto (data));
    }
    /* to lowercase */
    else if (rendata_get_uplo (data) == 1) {
        string_process_filename (string_to_lower,
                                 &process,
                                 rendata_get_applyto (data));
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Replace spaces to underscores or vice versa in specified file name to
 *         other.
 */
void
name_spaces_underscores (RenData             *data,
                         const uint_fast32_t  i)
{
    /* underscores to spaces */
    if (rendata_get_spaces (data) == 0) {
        ProcessData process = {
            .text       =  data->names->rf_items[i]->s_new,
            .text_len   = &data->names->rf_items[i]->new_len,
            .text_u8len = &data->names->rf_items[i]->new_u8len,
            .first = "_", .first_len = 1, .first_u8len = 1,
            .second = " ", .second_len = 1, .second_u8len = 1,
        };
        string_process_filename (string_replace_in,
                                 &process,
                                 rendata_get_applyto (data));
    }
    /* spaces to underscores */
    else if (rendata_get_spaces (data) == 1) {
        ProcessData process = {
            .text       =  data->names->rf_items[i]->s_new,
            .text_len   = &data->names->rf_items[i]->new_len,
            .text_u8len = &data->names->rf_items[i]->new_u8len,
            .first = " ", .first_len = 1, .first_u8len = 1,
            .second = "_", .second_len = 1, .second_u8len = 1,
        };
        string_process_filename (string_replace_in,
                                 &process,
                                 rendata_get_applyto (data));
    }
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Replace string in specified file name to other.
 *         If "replace from" string is not empty, perform string replace.
 */
void
name_replace_strings (RenData             *data,
                      const uint_fast32_t  i)
{
    auto rrep = rendata_get_rreplace (data);
    ProcessData process = {
        .text         =  data->names->rf_items[i]->s_new,
        .text_len     = &data->names->rf_items[i]->new_len,
        .text_u8len   = &data->names->rf_items[i]->new_u8len,
        .first        =  rreplace_get_from       (rrep),
        .first_len    =  rreplace_get_from_len   (rrep),
        .first_u8len  =  rreplace_get_from_u8len (rrep),
        .second       =  rreplace_get_to         (rrep),
        .second_len   =  rreplace_get_to_len     (rrep),
        .second_u8len =  rreplace_get_to_u8len   (rrep),
    };

    if (!rreplace_empty_from (rrep)) {
        string_process_filename (string_replace_in,
                                 &process,
                                 rendata_get_applyto (data));
    }
}
/*----------------------------------------------------------------------------*/
