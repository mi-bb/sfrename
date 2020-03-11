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
#include "strfn.h"
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
uint8_t
rdelete_get_cnt (const RDelete *r_delete)
{
    return r_delete->cnt;
}
/*----------------------------------------------------------------------------*/
void
rdelete_set_cnt (RDelete *r_delete,
                 uint8_t  val)
{
    r_delete->cnt = val;
}
/*----------------------------------------------------------------------------*/
uint8_t
rdelete_get_pos (const RDelete *r_delete)
{
    return r_delete->pos;
}
/*----------------------------------------------------------------------------*/
void
rdelete_set_pos (RDelete *r_delete,
                 uint8_t  val)
{
    r_delete->pos = val;
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
const char *
rinsovr_get_text (const RInsOvr *rio_item)
{
    return (const char *) rio_item->s_text;
}
/*----------------------------------------------------------------------------*/
void 
rinsovr_set_text (RInsOvr    *rio_item,
                  const char *val)
{
    size_t ui_len = get_valid_length (val, FN_LEN);

    memcpy (rio_item->s_text, val, ui_len);
    rio_item->s_text[ui_len] = '\0';
}
/*----------------------------------------------------------------------------*/
uint8_t
rinsovr_get_pos (const RInsOvr *rio_item)
{
    return rio_item->pos;
}
/*----------------------------------------------------------------------------*/
void
rinsovr_set_pos (RInsOvr       *rio_item,
                 const uint8_t  val)
{
    rio_item->pos = val;
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
const char *
rreplace_get_from (const RReplace *r_replace)
{
    return (const char *) r_replace->s_from;
}
/*----------------------------------------------------------------------------*/
void
rreplace_set_from (RReplace   *r_replace,
                   const char *val)
{
    size_t ui_len = get_valid_length (val, FN_LEN);

    memcpy (r_replace->s_from, val, ui_len);
    r_replace->s_from[ui_len] = '\0';
}
/*----------------------------------------------------------------------------*/
const char *
rreplace_get_to (const RReplace *r_replace)
{
    return (const char *) r_replace->s_to;
}
/*----------------------------------------------------------------------------*/
void
rreplace_set_to (RReplace   *r_replace,
                 const char *val)
{
    size_t ui_len = get_valid_length (val, FN_LEN);

    memcpy (r_replace->s_to, val, ui_len);
    r_replace->s_to[ui_len] = '\0';
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
int8_t
rnumber_get_opt (const RNumber *r_number)
{
    return r_number->opt;
}
/*----------------------------------------------------------------------------*/
void
rnumber_set_opt (RNumber      *r_number,
                 const int8_t  val)
{
    r_number->opt = val;
}
/*----------------------------------------------------------------------------*/
uint_fast32_t
rnumber_get_start (const RNumber *r_number)
{
    return r_number->start;
}
/*----------------------------------------------------------------------------*/
void
rnumber_set_start (RNumber             *r_number,
                   const uint_fast32_t  val)
{
    r_number->start = val;
}
/*----------------------------------------------------------------------------*/
uint8_t
rnumber_get_pos (const RNumber *r_number)
{
    return r_number->pos;
}
/*----------------------------------------------------------------------------*/
void
rnumber_set_pos (RNumber       *r_number,
                 const uint8_t  val)
{
    r_number->pos = val;
}
/*----------------------------------------------------------------------------*/
/**
 * @brief  RenData initialization.
 */
static void
rendata_init (RenData *rd_data)
{
    rd_data->uplo    = 2;    /* upper/lower case setting */
    rd_data->spaces  = 2;    /* spaces/underscores setting */
    rd_data->applyto = 2;    /* apply to names/ext/both setting */
    rd_data->renexit = 1;    /* exit after rename setting */
    rd_data->dirsel  = 1;    /* default select option to files */
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
/**
 * @brief  Get names object with file list.
 */
RFnames *
rendata_get_names (const RenData *rd_data)
{
    return rd_data->names;
}
/*----------------------------------------------------------------------------*/
int8_t
rendata_get_uplo (const RenData *rd_data)
{
    return rd_data->uplo;
}
/*----------------------------------------------------------------------------*/
void
rendata_set_uplo (RenData *rd_data,
                  int8_t   val)
{
    rd_data->uplo = val;
}
/*----------------------------------------------------------------------------*/
int8_t
rendata_get_spaces (const RenData *rd_data)
{
    return rd_data->spaces;
}
/*----------------------------------------------------------------------------*/
void
rendata_set_spaces (RenData *rd_data,
                    int8_t   val)
{
    rd_data->spaces = val;
}
/*----------------------------------------------------------------------------*/
int8_t
rendata_get_applyto (const RenData *rd_data)
{
    return rd_data->applyto;
}
/*----------------------------------------------------------------------------*/
void
rendata_set_applyto (RenData *rd_data,
                     int8_t   val)
{
    rd_data->applyto = val;
}
/*----------------------------------------------------------------------------*/
int8_t
rendata_get_renexit (const RenData *rd_data)
{
    return rd_data->renexit;
}
/*----------------------------------------------------------------------------*/
void
rendata_set_renexit (RenData *rd_data,
                     int8_t   val)
{
    rd_data->renexit = val;
}
/*----------------------------------------------------------------------------*/
int8_t
rendata_get_dirsel (const RenData *rd_data)
{
    return rd_data->dirsel;
}
/*----------------------------------------------------------------------------*/
void
rendata_set_dirsel (RenData *rd_data,
                    int8_t   val)
{
    rd_data->dirsel = val;
}
/*----------------------------------------------------------------------------*/

