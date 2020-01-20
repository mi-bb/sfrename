/**
 * @file  rfnames.h
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
 * @brief  RFnames struncture and functions.
 * 
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
#ifndef RFNAMES_H
#define RFNAMES_H
#include <gtk/gtk.h>
/*----------------------------------------------------------------------------*/
/**
 * @struct RFnames
 * @brief  File names buffer and entries
 *
 * @var   RFnames::cnt
 * @brief Count of file names
 *
 * @var   RFnames::s_new
 * @brief Strings with new file names
 *
 * @var   RFnames::s_org
 * @brief Strings with original file names
 */
typedef struct
RFnames {
    size_t      cnt;
    char      **s_new;
    char      **s_org;
    char      **s_pth;
    GtkWidget **entry;
} RFnames;
/*----------------------------------------------------------------------------*/
/**
 * @brief  RFnames initialization.
 *
 * @param[out] rf_names Pointer to RFnames object
 * @return     none
 */
void rfnames_init (RFnames    *rf_names);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free allocated memory.
 *
 * @param[in,out] rf_names Pointer to RFnames object
 * @return        none
 */
void rfnames_free (RFnames    *rf_names);
/*----------------------------------------------------------------------------*/
/**
 * @brief  Add file to RFnames.
 *
 * @param[out] rf_names Pointer to RFnames object
 * @param[in]  s_pth    File path
 * @return     none
 */
void rfnames_add  (RFnames    *rf_names,
                   const char *s_pth) __attribute__ ((nonnull (2)));
/*----------------------------------------------------------------------------*/
#endif

