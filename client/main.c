/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  main.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(01/04/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "01/04/24 14:06:58"
 *                 
 ********************************************************************************/
#include "main.h"

int main(int argc,char **argv)
{
	int							connfd=-1;
	struct sockaddr_in			servaddr;
	char						*servip;
	int							port;
	int							rv=-1;
	char						buf[1024];
	int							ch;
	sock_data					pack_data;
	int							len=20;
	int							time;//interval
	sqlite3						*db;
	int							rows;
	char						data_buf[1024];
	struct option				opts[]=
	{
		{"ipaddr",required_argument,NULL,'i'},
		{"port",required_argument,NULL,'p'},
		{"time",required_argument,NULL,'t'},
		{"help",no_argument,NULL,'h'},
		{NULL,0,NULL,0}
	};

	while((ch=getopt_long(argc,argv,"i:p:t:h",opts,NULL))!=-1)
	{
		switch(ch)
		{
			case 'i':
				servip=optarg;
				break;
			case 'p':
				port=atoi(optarg);
				break;
			case 't':
				time=atoi(optarg);
				break;
			case 'h':
				print_usage(argv[0]);
				return 0;
		}
	}

	if(!servip||!port)
	{
		print_usage(argv[0]);
		return 0;
	}

	connfd=sock_connect(servip,port);/* 连接服务器端 */

	db=sqlite3_open_database(DB_NAME);/*  创建打开数据库 */ 
	if(sqlite3_create_table(db,TABLE_NAME)<0)/*  创建表  */
	{
		dbg_print("Create table failure:%s\n",strerror(errno));
		sqlite3_close_database(db);
	}

	while(1)
	{
		memset(buf,0,sizeof(buf));
		if(get_temperature(&pack_data.temp)<0)
		{
			dbg_print("Get tempareture failure:%s\n",strerror(errno));
			return -1;
		}
		if(get_dev(pack_data.Id,len)<0)
		{
			dbg_print("Get ID failure:%s\n",strerror(errno));
			return -2;
		}
    	if(get_tm(pack_data.localt)<0)
		{
			dbg_print("Get time failure:%s\n",strerror(errno));
			return -3;
		}

		snprintf(buf,sizeof(buf),"%s %f %s",pack_data.Id,pack_data.temp,pack_data.localt);

		/* 判断服务器是否断开 */
		if(socket_alive(connfd)<0)
		{
			close(connfd);

			if(sqlite3_insert(db,TABLE_NAME,&pack_data)<0)
			{
				dbg_print("Insert data error\n");
				sqlite3_close_database(db);
			}
			dbg_print("Insert data successfully\n");

			/* 尝试重新连接 */
			if((connfd=sock_connect(servip,port))<0)
			{
				dbg_print("Reconnect server failure:%s\n",strerror(errno));
			}
			else
			{
				dbg_print("Reconnect server successfully\n");
			}
		}
		else/* 服务端未断开或已连上 */
		{
			memset(data_buf,0,sizeof(data_buf));
			if(sqlite3_select_all(db,TABLE_NAME)>0)/* 查询数据库中是否存有数据 */
			{
				/* 若存在数据 */
				if(sqlite3_select(db,TABLE_NAME,data_buf)>0)
				{
					dbg_print("从数据库中取出数据\n");
					if(write(connfd,data_buf,strlen(data_buf))<0)
					{
						dbg_print("Write data to server failure:%s\n",strerror(errno));
						goto CleanUp;
					}
					sqlite3_delete(db,TABLE_NAME);
				}
				else/*  若数据库中不存在数据则关闭数据库 */ 
				{
					sqlite3_close_database(db);
				}
			}
			/* 直接发送数据 */
			if(write(connfd,buf,strlen(buf))<0)
			{
				dbg_print("Write data to server failure:%s\n",strerror(errno));
				goto CleanUp;
			}
		}
		sleep(time);
	}//end of while

CleanUp:
	close(connfd);

	return 0;
}
