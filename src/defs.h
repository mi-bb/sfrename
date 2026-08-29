/**
 * @file  defs.h
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
 * @brief  Defines
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#ifndef DEFS_H
#define DEFS_H

#include <stdint.h>
/*----------------------------------------------------------------------------*/
/**
 * @def   APP_NAME
 * @brief Application name string
 *
 * Kept as a macro: it is pasted with adjacent string literals
 * (@c APP_NAME " v" @c APP_VER), which a @c constexpr array cannot do.
 *
 * @def   APP_VER
 * @brief Application version string
 *
 * Kept as a macro for the same reason as @c APP_NAME.
 */
#define APP_NAME   "Small File Renamer"
#define APP_VER    "1.2.11"
/*----------------------------------------------------------------------------*/
/**
 * @var   FN_LEN
 * @brief Maximum chars in file name string
 *
 * Deliberately @c int rather than @c size_t: it is compared against signed
 * values (see the range checks in rconfig.c), and an unsigned type would
 * silently turn those into unsigned comparisons.
 *
 * @var   WIN_WIDTH
 * @brief Application window width
 * @var   WIN_HEIGHT
 * @brief Application window height
 * @var   APP_WEB
 * @brief Application website
 * @var   APP_DSC
 * @brief Application description
 * @var   DEF_UPLO
 * @brief Default upper/lower case option
 * @var   DEF_SPACES
 * @brief Default spaces/underscores option
 * @var   DEF_APPLTO
 * @brief Default apply to file names/ext or both option
 * @var   DEF_RENEXT
 * @brief Default exit after rename option
 * @var   DEF_DIRSEL
 * @brief Default select files from directory options
 * @var   DEF_REMEMBEROPT
 * @brief Default remember options on exit option
 * @var   RCONFIG_DIR_NAME
 * @brief Name of the config directory created under the user's XDG config dir
 * @var   RCONFIG_FILE_NAME
 * @brief Name of the JSON config file holding remembered options
 */
constexpr int    FN_LEN          = 255;
constexpr int    WIN_WIDTH       = 600;
constexpr int    WIN_HEIGHT      = 800;
constexpr int8_t DEF_UPLO        = 2;
constexpr int8_t DEF_SPACES      = 2;
constexpr int8_t DEF_APPLTO      = 2;
constexpr bool   DEF_RENEXT      = true;
constexpr int8_t DEF_DIRSEL      = 1;
constexpr bool   DEF_REMEMBEROPT = false;

/**
 * @var   FOLDER_SELECT_FILES
 * @brief Option for select files in folder
 * @var   FOLDER_SELECT_FOLDERS
 * @brief Option for select folders in folder
 * @var   FOLDER_SELECT_SYMLINKS
 * @brief Option for select file/dir symlinks
 * @var   FOLDER_SELECT_HIDDEN
 * @brief Option for select hidden files/dirs
 * @var   FOLDER_SCAN_RECURSIVELY
 * @brief Option for scanning directory recursively
 * @var   FOLDER_SELECT_ALL
 * @brief Every valid folder-selection bit; the range a stored dirsel may hold
 */
constexpr int8_t FOLDER_SELECT_FILES     = 0x01;
constexpr int8_t FOLDER_SELECT_FOLDERS   = 0x02;
constexpr int8_t FOLDER_SELECT_SYMLINKS  = 0x04;
constexpr int8_t FOLDER_SELECT_HIDDEN    = 0x08;
constexpr int8_t FOLDER_SCAN_RECURSIVELY = 0x10;
constexpr int8_t FOLDER_SELECT_ALL       = FOLDER_SELECT_FILES
                                         | FOLDER_SELECT_FOLDERS
                                         | FOLDER_SELECT_SYMLINKS
                                         | FOLDER_SELECT_HIDDEN
                                         | FOLDER_SCAN_RECURSIVELY;

constexpr char APP_WEB[] = "http://www.nongnu.org/small-file-renamer/";
constexpr char APP_DSC[] = "Program for renaming files and directories.";
constexpr char RCONFIG_DIR_NAME[]  = "sfrename";
constexpr char RCONFIG_FILE_NAME[] = "config.json";
/*----------------------------------------------------------------------------*/
/**
 * @enum  RenResult
 * @brief Result of renaming a single file
 */
typedef enum RenResult : int_fast8_t {
    REN_OK,       /**< File renamed */
    REN_NOT_REN,  /**< Could not rename */
    REN_EXISTS,   /**< File already exists */
    REN_NC        /**< No change */
} RenResult;
/*----------------------------------------------------------------------------*/
#endif

