/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  sqlt.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(20/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "20/03/24 20:17:15"
 *                 
 ********************************************************************************/
#include<stdio.h>
#include<sqlite3.h>
#include<string.h>
#include<errno.h>
#include<stdlib.h>
#include<unistd.h>
#include"database.h"
#include"logger.h"

static sqlite3	*db = NULL;

/* Description:close the database */
void database_close(void)
{
	if(db!=NULL)
	{
		sqlite3_close(db);
	}
}

/* Description:Create database and table */
int database_table_init(char *db_name,char * table_name)
{
	int 	rc=0;
	char	sql[128]={0};
	char	*err_msg=NULL;

	if( !db_name || !table_name )
	{
		log_error("%s() Invalid input arguments\n",__func__);
		return -1;
	}

	/* if database exists then open it*/
	if( 0 == access(db_name, F_OK))
	{
		if( SQLITE_OK != sqlite3_open(db_name, &db) )
		{
			log_error("open database failure\n");
			return -2;
		}
		log_info("open database ok\n");
		return 0;
	}

	/* if database doesn't exist ,create and open it */
	if( SQLITE_OK != sqlite3_open(db_name, &db))
	{
		log_error("create database failure\n");
		return -3;
	}
	log_info("open database ok\n");

	/* create table */
	sprintf(sql,"CREATE TABLE IF NOT EXISTS %s(ID TEXT,Temperature REAL,time1 TEXT)",table_name);
	rc=sqlite3_exec(db,sql,0,0,&err_msg);
	if(rc!=SQLITE_OK)
	{
		log_error("create table %s error:%s\n",table_name,err_msg);
		sqlite3_free(err_msg);
		return -4;
	}

	log_info("create table ok\n");

	return 0;
}

/* Description:delete the table */
int table_delete(char * table_name)
{
	int		rc=0;
	int 	ret=-1;
	char	sql[128]={0};
	char	*err_msg=NULL;

	sprintf(sql,"DROP TABLE %s",table_name);
	rc=sqlite3_exec(db,sql,0,0,&err_msg);
	if(rc!=SQLITE_OK)
	{
		log_error("delete table %s error:%s\n",table_name,err_msg);
		sqlite3_free(sql);
		ret=-1;
	}
	else
	{
		ret=0;
	}
	return ret;
}

/* Description:Insert data to database */
int database_data_insert(char* table_name,pack_data_t *pack_data)
{
	int		rc=0;
	char	sql[128];
	char	*err_msg=NULL;

	sprintf(sql,"INSERT INTO %s(ID,Temperature,time1) VALUES ('%s',%f,'%s')",table_name,pack_data->Id,pack_data->tempera,pack_data->local_t);
	
	rc=sqlite3_exec(db,sql,0,0,&err_msg);
	if(rc!=SQLITE_OK)
	{
		log_error("Insert error:%s\n",err_msg);
		sqlite3_free(err_msg);
		return -1;
	}
	
	return 0;
}

/* Description:Delete the first row of data from the database */
int	database_data_delete(char* table_name)
{
	int 	rc=0;
	char	sql[128]={0};
	char	*err_msg=NULL;
	sprintf(sql,"DELETE FROM %s WHERE ROWID = (SELECT MIN(rowid) FROM %s);",table_name,table_name);
	rc=sqlite3_exec(db,sql,0,0,&err_msg);
	if(rc!=SQLITE_OK)
	{
		log_error("delete error:%s\n",err_msg);
		sqlite3_free(err_msg);
		return -1;
	}
	
	return 0;
}

/* Description:Take the first row of data from database */
int database_data_take(char* table_name,char *data_buf)
{
	int		i;
	char	*err_msg=NULL;
	char	sql[128]={0};
	int		rc=0;
	int		rows,columns;
	char	**results;

	sprintf(sql,"SELECT * FROM %s",table_name);
	rc=sqlite3_get_table(db,sql,&results,&rows,&columns,&err_msg);
	if(rc!=SQLITE_OK)
	{
		log_error("Select error:%s\n",err_msg);
		sqlite3_free(err_msg);
		return -1;
	}	
	snprintf(data_buf,128,"%s %.2f %s\n",results[3],atof(results[4]),results[5]);		    

	return 1;
}

/* Description:Query whether there is data in the database */
int database_data_select(char* table_name)
{
	char			*err_msg=NULL;
	int				rc;
	char			sql[128]={0};
	sqlite3_stmt	*stmt;
	int				result=0;
	int				rv;

	sprintf(sql,"SELECT COUNT(*) FROM %s",table_name);
	rc=sqlite3_prepare_v2(db,sql,-1,&stmt,0);

	if(rc!=SQLITE_OK)
	{
		log_error("Failed to prepare statement:%s\n",err_msg);
		sqlite3_free(err_msg);
		return (0);
	}

	rc=sqlite3_step(stmt);
	if(rc==SQLITE_ROW)
	{
		result=sqlite3_column_int(stmt,0);
	}

	if(result>0)
	{
		rv=1;
	}
	else
	{
		rv=-1;
	}
	sqlite3_finalize(stmt);
	return rv;
}
