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
 * @author Michal Babik <michal.babik@protonmail.com>
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
[[nodiscard]] char * rconfig_get_path (void)
              __attribute__ ((malloc, returns_nonnull));
/*----------------------------------------------------------------------------*/
/**
 * @fn  bool rconfig_write (const RenData *data,
 *                          const char    *path)
 *
 * @brief  Serialize data as JSON and write it to path.
 *
 * Creates the parent directory if it does not already exist.
 *
 * @param[in] data RenData object to serialize
 * @param[in] path    Destination file path
 * @return    true on success, false on failure
 *
 * @fn  bool rconfig_parse (RenData    *data,
 *                          const char *path)
 *
 * @brief  Read and parse a JSON config file, applying it to data.
 *
 * Uses a parse-then-apply strategy: if the file is missing, unreadable, or
 * malformed, data is left completely unmodified and false is returned.
 *
 * @param[out] data RenData object to update
 * @param[in]  path    Source file path
 * @return     true on success, false on failure
 */
/*----------------------------------------------------------------------------*/
bool rconfig_write (const RenData *data,
                    const char    *path);

bool rconfig_parse (RenData    *data,
                    const char *path);
/*----------------------------------------------------------------------------*/
/**
 * @fn  bool rconfig_save (const RenData *data)
 *
 * @brief  Save settings to the standard config path if "remember options on
 *         exit" is enabled, otherwise remove any existing config file.
 *
 * @param[in] data RenData object to save
 * @return    true on success, false on failure
 *
 * @fn  bool rconfig_load (RenData *data)
 *
 * @brief  Load settings from the standard config path into data, if it
 *         exists and parses successfully.
 *
 * @param[out] data RenData object to update
 * @return     true if settings were loaded, false otherwise
 */
/*----------------------------------------------------------------------------*/
bool rconfig_save (const RenData *data);

bool rconfig_load (RenData *data);
/*----------------------------------------------------------------------------*/
#endif
