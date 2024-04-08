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

#ifndef SQLT_H
#define SQLT_H

#define CONFIG_DEBUG
#ifdef  CONFIG_DEBUG
#define dbg_print(format,args...)	printf(format,##args)
#else
#define dbg_print(format,args...)	do{} while(0)
#endif

typedef struct Sock_data
{
	char		Id[16];
	char		localt[128];
	float		temp;
}sock_data;

sqlite3* sqlite3_open_database(char * db_name);
void sqlite3_close_database(sqlite3* db);
int sqlite3_exist_table(sqlite3* db,char* table_name);
int sqlite3_create_table(sqlite3* db,char* table_name);
int sqlite3_delete_table(sqlite3* db,char* table_name);
int sqlite3_insert(sqlite3* db,char* table_name,sock_data *tdata);
int sqlite3_delete(sqlite3* db,char* table_name);
int sqlite3_select(sqlite3* db,char* table_name,char* data_buf);
int sqlite3_select_all(sqlite3* db,char* table_name);

#endif
