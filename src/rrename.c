/**
 * @file  rrename.c
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "rrename.h"
/*----------------------------------------------------------------------------*/
/**
 * @brief  Check if file can be renamed and rename it.
 */
RenResult
rrename_file (const char *old_name,
              const char *new_name,
              const char *path)
{
    char      *new_path = nullptr;           /* Full path for new file */
    char      *old_path = nullptr;           /* Full path for old file */
    RenResult  result   = REN_OK;            /* Renaming result */
    size_t     dir_len  = strlen (path);     /* Length of file dir path */
    size_t     old_len  = strlen (old_name); /* Length of old file name */
    size_t     new_len  = strlen (new_name); /* Length of new file name */

    /* Check if new file name is different than old */
    if (old_len == new_len && strcmp (old_name, new_name) == 0)
        return REN_NC; /* No chgange */

    /* Alloc memory for full paths of old and new file name */
    new_path = malloc ((dir_len + new_len + 1) * sizeof (char));
    old_path = malloc ((dir_len + old_len + 1) * sizeof (char));

    if (new_path == nullptr || old_path == nullptr) {
        fputs ("Alloc error\n", stderr);
        exit (EXIT_FAILURE);
    }
    memcpy (new_path, path, dir_len);
    memcpy (old_path, path, dir_len);
    memcpy (new_path + dir_len, new_name, new_len);
    memcpy (old_path + dir_len, old_name, old_len);
    new_path [dir_len + new_len] = '\0';
    old_path [dir_len + old_len] = '\0';

    /* Check if file with new file name don't exist */
    if (access (new_path, F_OK) != 0) {
        result = rename (old_path, new_path) == 0 ?
                REN_OK :     /* OK */
                REN_NOT_REN; /* Could not rename */
    }
    else {
        result = REN_EXISTS;  /* File exists */
    }
    free (new_path);
    free (old_path);

    return result;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Rename the file of a checked item to its pending name.
 */
RenResult
rrename_item (RFitem *item)
{
    RenResult result;

    if (!rfitem_get_checked (item))
        return REN_SKIP;

    result = rrename_file (rfitem_get_org_name (item),
                           rfitem_get_new_name (item),
                           rfitem_get_path (item));

    if (result == REN_OK) {
        rfitem_set_org_name_from_new_name (item);
    }
    else if (result != REN_NC) {
        rfitem_set_new_name_from_org_name (item);
    }
    return result;
}
/*----------------------------------------------------------------------------*/

