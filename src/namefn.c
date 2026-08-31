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
    auto        rdel    = rendata_get_rdelete (rd_data);
    ProcessData pd_data = {
        .s_srcdst     =  rd_data->names->rf_items[i]->s_new,
        .srcdst_len   = &rd_data->names->rf_items[i]->new_len,
        .srcdst_u8len = &rd_data->names->rf_items[i]->new_u8len,
        .ui_pos       = rdelete_get_pos (rdel),
        .ui_cnt       = rdelete_get_cnt (rdel),
    };

    /* exit if no chars to delete */
    if (rdelete_get_cnt (rdel)) {
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
    auto        rins    = rendata_get_rinsert (rd_data);
    ProcessData pd_data = {
        .s_srcdst     =  rd_data->names->rf_items[i]->s_new,
        .srcdst_len   = &rd_data->names->rf_items[i]->new_len,
        .srcdst_u8len = &rd_data->names->rf_items[i]->new_u8len,
        .s_str1       = rinsovr_get_text  (rins),
        .str1_len     = rinsovr_get_len   (rins),
        .str1_u8len   = rinsovr_get_u8len (rins),
        .ui_pos       = rinsovr_get_pos   (rins),
    };

    if (!rinsovr_empty (rins)) {
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
    auto        rovr    = rendata_get_roverwr (rd_data);
    ProcessData pd_data = {
        .s_srcdst     =  rd_data->names->rf_items[i]->s_new,
        .srcdst_len   = &rd_data->names->rf_items[i]->new_len,
        .srcdst_u8len = &rd_data->names->rf_items[i]->new_u8len,
        .s_str1       = rinsovr_get_text  (rovr),
        .str1_len     = rinsovr_get_len   (rovr),
        .str1_u8len   = rinsovr_get_u8len (rovr),
        .ui_pos       = rinsovr_get_pos   (rovr),
    };

    if (!rinsovr_empty (rovr)) {
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
    auto        rnum    = rendata_get_rnumber (rd_data);
    ProcessData pd_data = {
        .s_srcdst     =  rd_data->names->rf_items[i]->s_new,
        .srcdst_len   = &rd_data->names->rf_items[i]->new_len,
        .srcdst_u8len = &rd_data->names->rf_items[i]->new_u8len,
        .ui_pos       = rnumber_get_pos   (rnum),
        .ui_no        = i,
        .ui_start     = rnumber_get_start (rnum),
        .ui_max       = rfnames_get_cnt (rendata_get_rfnames (rd_data)) - 1,
    };

    if (rnumber_get_opt (rnum)) {
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
        .s_srcdst     =  rd_data->names->rf_items[i]->s_new,
        .srcdst_len   = &rd_data->names->rf_items[i]->new_len,
        .srcdst_u8len = &rd_data->names->rf_items[i]->new_u8len,
    };

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
            .s_srcdst     =  rd_data->names->rf_items[i]->s_new,
            .srcdst_len   = &rd_data->names->rf_items[i]->new_len,
            .srcdst_u8len = &rd_data->names->rf_items[i]->new_u8len,
            .s_str1 = "_", .str1_len = 1, .str1_u8len = 1,
            .s_str2 = " ", .str2_len = 1, .str2_u8len = 1,
        };
        string_process_filename (string_replace_in,
                                 &pd_data,
                                 rendata_get_applyto (rd_data));
    }
    /* spaces to underscores */
    else if (rendata_get_spaces (rd_data) == 1) {
        ProcessData pd_data = {
            .s_srcdst     =  rd_data->names->rf_items[i]->s_new,
            .srcdst_len   = &rd_data->names->rf_items[i]->new_len,
            .srcdst_u8len = &rd_data->names->rf_items[i]->new_u8len,
            .s_str1 = " ", .str1_len = 1, .str1_u8len = 1,
            .s_str2 = "_", .str2_len = 1, .str2_u8len = 1,
        };
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
    auto        rrep    = rendata_get_rreplace (rd_data);
    ProcessData pd_data = {
        .s_srcdst     =  rd_data->names->rf_items[i]->s_new,
        .srcdst_len   = &rd_data->names->rf_items[i]->new_len,
        .srcdst_u8len = &rd_data->names->rf_items[i]->new_u8len,
        .s_str1       = rreplace_get_from       (rrep),
        .str1_len     = rreplace_get_from_len   (rrep),
        .str1_u8len   = rreplace_get_from_u8len (rrep),
        .s_str2       = rreplace_get_to         (rrep),
        .str2_len     = rreplace_get_to_len     (rrep),
        .str2_u8len   = rreplace_get_to_u8len   (rrep),
    };

    if (!rreplace_empty_from (rrep)) {
        string_process_filename (string_replace_in,
                                 &pd_data,
                                 rendata_get_applyto (rd_data));
    }
}
/*----------------------------------------------------------------------------*/
