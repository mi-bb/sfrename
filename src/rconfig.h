/**
 * @file  rconfig.h
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
 * @brief  Save/restore RenData settings to/from a local JSON config file.
 *
 * @author Michal Babik <michal.babik@pm.me>
 */
#ifndef RCONFIG_H
#define RCONFIG_H

#include <glib.h>
#include "rendata.h"
/*----------------------------------------------------------------------------*/
/**
 * @fn  char * rconfig_get_path (void)
 *
 * @brief  Get the path to the JSON config file.
 *
 * @return     Newly allocated path (caller must g_free() it)
 */
/*----------------------------------------------------------------------------*/
char * rconfig_get_path (void) __attribute__ ((malloc, returns_nonnull));
/*----------------------------------------------------------------------------*/
/**
 * @fn  gboolean rconfig_write (const RenData *rd_data,
 *                               const char    *path)
 *
 * @brief  Serialize rd_data as JSON and write it to path.
 *
 * Creates the parent directory if it does not already exist.
 *
 * @param[in] rd_data RenData object to serialize
 * @param[in] path    Destination file path
 * @return    TRUE on success, FALSE on failure
 *
 * @fn  gboolean rconfig_parse (RenData    *rd_data,
 *                               const char *path)
 *
 * @brief  Read and parse a JSON config file, applying it to rd_data.
 *
 * Uses a parse-then-apply strategy: if the file is missing, unreadable, or
 * malformed, rd_data is left completely unmodified and FALSE is returned.
 *
 * @param[out] rd_data RenData object to update
 * @param[in]  path    Source file path
 * @return     TRUE on success, FALSE on failure
 */
/*----------------------------------------------------------------------------*/
gboolean rconfig_write (const RenData *rd_data,
                        const char    *path);

gboolean rconfig_parse (RenData    *rd_data,
                        const char *path);
/*----------------------------------------------------------------------------*/
/**
 * @fn  gboolean rconfig_save (const RenData *rd_data)
 *
 * @brief  Save settings to the standard config path if "remember options on
 *         exit" is enabled, otherwise remove any existing config file.
 *
 * @param[in] rd_data RenData object to save
 * @return    TRUE on success, FALSE on failure
 *
 * @fn  gboolean rconfig_load (RenData *rd_data)
 *
 * @brief  Load settings from the standard config path into rd_data, if it
 *         exists and parses successfully.
 *
 * @param[out] rd_data RenData object to update
 * @return     TRUE if settings were loaded, FALSE otherwise
 */
/*----------------------------------------------------------------------------*/
gboolean rconfig_save (const RenData *rd_data);

gboolean rconfig_load (RenData *rd_data);
/*----------------------------------------------------------------------------*/
#endif
