/********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  sqlt.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(24/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "24/03/24 19:46:47"
 *                 
 ********************************************************************************/

#ifndef _DATABASE_H
#define _DATABASE_H

#include "data_pack.h"

/* Description:close the database */
extern void database_close(void);

/* Description:Create database and table */
extern int database_table_init(char *db_name,char *table_name);

/* Description:Delete the table */
extern int table_delete(char *table_name);

/* Description:Insert data to database */
extern int database_data_insert(char *table_name,pack_data_t *pack_data);

/* Description:Delete the first row of data from the database */
extern int database_data_delete(char *table_name);

/* Description:Take the first row of data from database */
extern int database_data_take(char *table_name,char *data_buf);

/* Description:Query whether there is data in the database */
extern int database_data_select(char *table_name);

#endif
