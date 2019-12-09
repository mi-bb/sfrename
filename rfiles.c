/**
 * @file  rfiles.c
 * @copyright Copyright (C) 2019 Michal Babik
 *
 * This file is part of Small File Renamer.
 *
 * Wall Changer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Wall Changer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Wall Changer.  If not, see <https://www.gnu.org/licenses/>.
 *
 * @brief  Functions to modify file names
 * 
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#include "rfiles.h"

/** 
 * @brief Rename data structure
 */ 
typedef struct
RenData {
    RFnames     names;           /**< File names, old, new, entries */
    RDelete     delete;          /**< Delete chars properties */
    RInsOvr     insert;          /**< Insert text properties */
    RInsOvr     overwrite;       /**< Overwrite text properties */
    RReplace    replace;         /**< Replace string with string settings */
    RNumber     number;          /**< Numbering names settings */
    int8_t      uplo;            /**< Upper/lower case option */
    int8_t      spaces;          /**< Spaces/underscores option */
    int8_t      applyto;         /**< Apply to file names/ext or both option */
    uint8_t     renexit;         /**< Exit after rename option */
} RenData;

