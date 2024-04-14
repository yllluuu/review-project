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
#include"cli_sqlt.h"

static sqlite3		*db=NULL;
//创建打开数据库
/*  sqlite3* sqlite3_open_database(char * db_name)
{
	int			 	rc=0;
	sqlite3* 		db;

	if(0==access(db_name,F_OK))
	{
		if(SQLITE_OK!=sqlite3_open(db_name,&db))
		{
			dbg_print("open database failure:%s\n",            sqlite3_errmsg(db));
			return NULL;
		}
		dbg_print("open database ok\n");
	}

	rc=sqlite3_open(db_name,&db);
	if(rc!=SQLITE_OK)
	{
		dbg_print("Open db error:%s\n",sqlite3_errmsg(db));
		return NULL;
	}

	else
	{
		dbg_print("Open a sqlite3 database successfully!\n");
	}

	return db;
}*/

void sqlite3_close_database(void)
{
	if(db!=NULL)
	{
		sqlite3_close(db);
	}
	return ;
}

//创建表
int sqlite3_create_table(char *db_name,char * table_name)
{
	int 	rc=0;
	char	sql[128]={0};
	char	*err_msg=NULL;

	/* 数据库存在则打开 */
	if(0==access(db_name,F_OK))
	{
		if(SQLITE_OK!=sqlite3_open(db_name,&db))
		{
			dbg_print("open database failure:%s\n",sqlite3_errmsg(db));
			return -1;
		}
		dbg_print("open database ok\n");
		return 0;
	}
	/* 数据库不存在，创建它 */
	if(SQLITE_OK!=sqlite3_open(db_name,&db))
	{
		dbg_print("Open db error:%s\n",sqlite3_errmsg(db));
		sqlite3_close(db);
		return -1;
	}
	/* 创建表 */		
	sprintf(sql,"CREATE TABLE IF NOT EXISTS %s(ID TEXT,Temperature REAL,time1 TEXT)",table_name);
	rc=sqlite3_exec(db,sql,0,0,&err_msg);
	if(rc!=SQLITE_OK)
	{
		dbg_print("create table %s error:%s\n",table_name,err_msg);
		sqlite3_free(err_msg);
		sqlite3_close(db);
		return -1;
	}
	else
	{
		return 0;
	}
	return 0;
}

//删除表
int sqlite3_delete_table(char * table_name)
{
	int		rc=0;
	int 	ret=-1;
	char	sql[128]={0};
	char	*err_msg=NULL;

	sprintf(sql,"DROP TABLE %s",table_name);
	rc=sqlite3_exec(db,sql,0,0,&err_msg);
	if(rc!=SQLITE_OK)
	{
		dbg_print("delete table %s error:%s\n",table_name,err_msg);
		sqlite3_free(sql);
		ret=-1;
	}
	else
	{
		ret=0;
	}
	return ret;
}

//插入数据
int sqlite3_insert(char* table_name,sock_data *tdata)
{
	int		rc=0;
	int		ret=-1;
	char	sql[128];
	char	*err_msg=NULL;

	sprintf(sql,"INSERT INTO %s(ID,Temperature,time1) VALUES ('%s',%f,'%s')",table_name,tdata->Id,tdata->temp,tdata->localt);

	rc=sqlite3_exec(db,sql,0,0,&err_msg);
	if(rc!=SQLITE_OK)
	{
		dbg_print("Insert error:%s\n",err_msg);
		sqlite3_free(err_msg);
		ret=-1;
	}
	else
	{
		ret=0;
	}
	return ret;
}

//删除数据
int	sqlite3_delete(char* table_name)
{
	int 	rc=0;
	int		ret=-1;
	char	sql[128]={0};
	char	*err_msg=NULL;
	sprintf(sql,"DELETE FROM %s WHERE ROWID = (SELECT MIN(rowid) FROM %s);",table_name,table_name);
	rc=sqlite3_exec(db,sql,0,0,&err_msg);
	if(rc!=SQLITE_OK)
	{
		dbg_print("delete error:%s\n",err_msg);
		sqlite3_free(err_msg);
		ret=-1;
	}
	else{
		ret=0;
	}
	return ret;
}

//查询数据
int sqlite3_select(char* table_name,char *data_buf)
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
		dbg_print("Select error:%s\n",err_msg);
		sqlite3_free(err_msg);
		return -1;
	}	
	snprintf(data_buf,128,"%s %.2f %s\n",results[3],atof(results[4]),results[5]);		    
	return 1;
}

//查询数据库中是否含有数据
int sqlite3_select_all(char* table_name)
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
		dbg_print("Failed to prepare statement:%s\n",err_msg);
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
