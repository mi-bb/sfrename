/**
 * @file  defs.h
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
 * @brief  Defines
 *
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#ifndef DEFS_H
#define DEFS_H
/*----------------------------------------------------------------------------*/
/**
 * @def   FN_LEN
 * @brief Maximum chars in file name string
 *
 * @def   WIN_WIDTH
 * @brief Application window width
 *
 * @def   WIN_HEIGHT
 * @brief Application window height
 *
 * @def   APP_NAME
 * @brief Application name string
 *
 * @def   APP_VER
 * @brief Application version string
 */
#define FN_LEN     255
#define WIN_WIDTH  600
#define WIN_HEIGHT 800
#define APP_NAME   "Small File Renamer"
#define APP_VER    "1.2.3"
#define APP_WEB    "http://www.nongnu.org/small-file-renamer/"
#define APP_DSC    "Program for renaming files and directories."
/*----------------------------------------------------------------------------*/
enum {
    REN_OK,                      /**< File renamed */
    REN_NOT_REN,                 /**< Could not rename */
    REN_EXISTS,                  /**< File already exists */
    REN_NC                       /**< No change */
};
/*----------------------------------------------------------------------------*/
#endif

