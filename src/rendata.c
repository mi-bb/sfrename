/**
 * @file  rendata.c
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
 * @brief  RenData struncture with data to rename files.
 * 
 * @author Michał Bąbik <michalb1981@o2.pl>
 */
//#include <glib.h>
#include "rendata.h"
/*----------------------------------------------------------------------------*/
/**
 * @fn         static void rdelete_init (RDelete *r_delete)
 * @brief      RDelete initialization.
 * @param[out] r_delete Pointer to RDelete object
 * @return     none
 *
 * @fn         static RDelete * rdelete_new (void)
 * @brief      Create new RDelete object.
 * @return     New RDelete object
 *
 * @fn         static void rdelete_free (RDelete *r_delete)
 * @brief      Free allocated memory.
 * @param[out] r_delete Pointer to RDelete object
 * @return     none
 */
/*----------------------------------------------------------------------------*/
static void      rdelete_init (RDelete *r_delete);

static RDelete * rdelete_new  (void) __attribute__ ((returns_nonnull));

static void      rdelete_free (RDelete *r_delete);
/*----------------------------------------------------------------------------*/
/**
 * @fn         static void rinsovr_init (RInsOvr *r_insovr)
 * @brief      RInsOvr initialization.
 * @param[out] r_insovr Pointer to RInsOvr object
 * @return     none
 *
 * @fn         static RInsOvr * rinsovr_new  (void)
 * @brief      Create new RInsOvr object.
 * @return     New RInsOvr object
 *
 * @fn         static void rinsovr_free (RInsOvr *r_insovr)
 * @brief      Free allocated memory.
 * @param[out] r_insovr Pointer to RInsOvr object
 * @return     none
 */
/*----------------------------------------------------------------------------*/
static void      rinsovr_init (RInsOvr *r_insovr);

static RInsOvr * rinsovr_new  (void) __attribute__ ((returns_nonnull));

static void      rinsovr_free (RInsOvr *r_insovr);
/*----------------------------------------------------------------------------*/
/**
 * @fn         static void rreplace_init (RReplace *r_replace)
 * @brief      RReplace initialization.
 * @param[out] r_replace Pointer to RReplace object
 * @return     none
 *
 * @fn         static RReplace * rreplace_new (void)
 * @brief      Create new RReplace object.
 * @return     New RReplace object
 *
 * @fn         static void rreplace_free (RReplace *r_replace)
 * @brief      Free allocated memory.
 * @param[out] r_replace Pointer to RReplace object
 * @return     none
 */
/*----------------------------------------------------------------------------*/
static void       rreplace_init (RReplace *r_replace);

static RReplace * rreplace_new  (void) __attribute__ ((returns_nonnull));

static void       rreplace_free (RReplace *r_replace);
/*----------------------------------------------------------------------------*/
/**
 * @fn         static void rnumber_init (RNumber *r_number)
 * @brief      RNumber initialization.
 * @param[out] r_number Pointer to RNumber object
 * @return     none
 *
 * @fn         static RNumber * rnumber_new (void)
 * @brief      Create new RNumber object.
 * @return     New RNumber object
 *
 * @fn         static void rnumber_free (RNumber *r_number)
 * @brief      Free allocated memory.
 * @param[out] r_number Pointer to RNumber object
 * @return     none
 */
/*----------------------------------------------------------------------------*/
static void      rnumber_init (RNumber *r_number);

static RNumber * rnumber_new  (void) __attribute__ ((returns_nonnull));

static void      rnumber_free (RNumber *r_number);
/*----------------------------------------------------------------------------*/
/**
 * @brief  RenData initialization.
 *
 * @param[out] rd_data Pointer to RenData object
 */
static void      rendata_init (RenData *rd_data);
/*----------------------------------------------------------------------------*/
/**
 * @brief  RDelete initialization.
 *
 * @param[out] r_delete Pointer to RDelete object
 * @return     none
 */
static void
rdelete_init (RDelete *r_delete)
{
    r_delete->cnt = 0;
    r_delete->pos = 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief   Create new RDelete object.
 *
 * @return  New RDelete object
 */
static RDelete *
rdelete_new (void)
{
    RDelete *rd_new = NULL;

    rd_new = malloc (sizeof (RDelete));

    if (rd_new == NULL) {
        fputs ("Alloc error\n", stderr);
        exit (EXIT_FAILURE);
    }
    rdelete_init (rd_new);
    return rd_new;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free allocated memory.
 *
 * @param[out] r_delete Pointer to RDelete object
 * @return     none
 */
static void
rdelete_free (RDelete *r_delete)
{
    free (r_delete);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  RInsOvr initialization.
 *
 * @param[out] r_insovr Pointer to RInsOvr object
 * @return     none
 */
static void
rinsovr_init (RInsOvr *r_insovr)
{
    r_insovr->pos = 0;
    memset (r_insovr->s_text, '\0', sizeof (r_insovr->s_text));
}
/*----------------------------------------------------------------------------*/
/**
 * @brief   Create new RInsOvr object.
 *
 * @return  New RInsOvr object
 */
static RInsOvr *
rinsovr_new (void)
{
    RInsOvr *r_ins = NULL;

    r_ins = malloc (sizeof (RInsOvr));

    if (r_ins == NULL) {
        fputs ("Alloc error\n", stderr);
        exit (EXIT_FAILURE);
    }
    rinsovr_init (r_ins);
    return r_ins;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free allocated memory.
 *
 * @param[out] r_insovr Pointer to RInsOvr object
 * @return     none
 */
static void
rinsovr_free (RInsOvr *r_insovr)
{
    free (r_insovr);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  RReplace initialization.
 *
 * @param[out] r_replace Pointer to RReplace object
 * @return     none
 */
static void
rreplace_init (RReplace *r_replace)
{
    memset (r_replace->s_from, '\0', sizeof (r_replace->s_from));
    memset (r_replace->s_to,   '\0', sizeof (r_replace->s_to));
}
/*----------------------------------------------------------------------------*/
/**
 * @brief   Create new RReplace object.
 *
 * @return  New RReplace object
 */
static RReplace *
rreplace_new (void)
{
    RReplace *r_rep = NULL;

    r_rep = malloc (sizeof (RReplace));

    if (r_rep == NULL) {
        fputs ("Alloc error\n", stderr);
        exit (EXIT_FAILURE);
    }
    rreplace_init (r_rep);
    return r_rep;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free allocated memory.
 *
 * @param[out] r_replace Pointer to RReplace object
 * @return     none
 */
static void
rreplace_free (RReplace *r_replace)
{
    free (r_replace);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  RNumber initialization.
 *
 * @param[out] r_number Pointer to RNumber object
 * @return     none
 */
static void
rnumber_init (RNumber *r_number)
{
    r_number->opt   = 0;
    r_number->start = 0;
    r_number->pos   = 0;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief   Create new RNumber object.
 *
 * @return  New RNumber object
 */
static RNumber *
rnumber_new (void)
{
    RNumber *r_num = NULL;

    r_num = malloc (sizeof (RNumber));

    if (r_num == NULL) {
        fputs ("Alloc error\n", stderr);
        exit (EXIT_FAILURE);
    }
    rnumber_init (r_num);
    return r_num;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free allocated memory.
 *
 * @param[out] r_number Pointer to RNumber object
 * @return     none
 */
static void
rnumber_free (RNumber *r_number)
{
    free (r_number);
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  RenData initialization.
 */
static void
rendata_init (RenData *rd_data)
{
    rd_data->uplo     = 2;    /* upper/lower case setting */
    rd_data->spaces   = 2;    /* spaces/underscores setting */
    rd_data->applyto  = 2;    /* apply to names/ext/both setting */
    rd_data->renexit  = 1;    /* exit after rename setting */

    rd_data->names   = rfnames_new ();
    rd_data->del     = rdelete_new ();
    rd_data->ins     = rinsovr_new ();
    rd_data->ovrw    = rinsovr_new ();
    rd_data->replace = rreplace_new ();
    rd_data->number  = rnumber_new ();
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Create and return new RenData item.
 */
RenData *
rendata_new (void)
{
    RenData *rd_new = NULL;
    rd_new = malloc (sizeof (RenData));

    if (rd_new == NULL) {
        fputs ("Alloc error\n", stderr);
        exit (EXIT_FAILURE);
    }
    rendata_init (rd_new);
    return rd_new;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  Free allocated memory.
 */
void
rendata_free (RenData *rd_data)
{
    rfnames_free (rd_data->names);
    rdelete_free (rd_data->del);
    rinsovr_free (rd_data->ins);
    rinsovr_free (rd_data->ovrw);
    rreplace_free (rd_data->replace);
    rnumber_free (rd_data->number);
    free (rd_data);
}
/*----------------------------------------------------------------------------*/
RFnames *
rendata_get_names (const RenData *rd_data)
{
    return rd_data->names;
}
/*----------------------------------------------------------------------------*/

