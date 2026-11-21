/**
 * @file  rrename.h
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
 * @brief  Renaming files on disk.
 *
 * @author Michal Babik <michal.babik@protonmail.com>
 */
#ifndef RRENAME_H
#define RRENAME_H

#include "defs.h"
#include "rfitem.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if file can be renamed and rename it.
 *
 * @param[in] old_name Old file name
 * @param[in] new_name New file name
 * @param[in] path     Path to directory where the file is located, with
 *                     trailing slash
 * @return    REN_OK, REN_NOT_REN, REN_EXISTS or REN_NC
 */
[[nodiscard]] RenResult rrename_file (const char *old_name,
                                      const char *new_name,
                                      const char *path)
                        __attribute__ ((nonnull (1, 2, 3)));
/*----------------------------------------------------------------------------*/
/**
 * @brief  Rename the file of a checked item to its pending name.
 *
 * An unchecked item is not touched and gives REN_SKIP. After REN_OK org_name
 * holds the name the file has now. After REN_NOT_REN or REN_EXISTS new_name
 * is set back to org_name. The entry widget is not updated.
 *
 * @param[in,out] item RFitem object
 * @return        Renaming result
 */
[[nodiscard]] RenResult rrename_item (RFitem *item)
                        __attribute__ ((nonnull (1)));
/*----------------------------------------------------------------------------*/
#endif

