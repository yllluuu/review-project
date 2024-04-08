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
	//socket
	int							connfd=-1;
	struct sockaddr_in			servaddr;
	char						*servip;
	int							port;
	int							k;
	int							rv=-1;
	char						buf[1024];
	int							ch;
	//data
	sock_data					pack_data;
	int							len=20;
	//interval
	int							time;//休眠时间
	//sqlite3
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

	if(!servip||!port||!time)
	{
		print_usage(argv[0]);
		return 0;
	}

	connfd=sock_connect(servip,port);/* 连接服务器端 */

	while(1)
	{
		memset(buf,0,sizeof(buf));
		get_temperature(&pack_data.temp);
		get_dev(pack_data.Id,len);
		get_tm(pack_data.localt);

		snprintf(buf,sizeof(buf),"%s %f %s",pack_data.Id,pack_data.temp,pack_data.localt);/* 将数据写入buf中 */

		k=socket_alive(connfd);/* 判断服务器是否断开 */
		if(k<0)/* 若断开 */
		{
			close(connfd);

			db=sqlite3_open_database(DB_NAME);/* 创建打开数据库 */
			if(sqlite3_create_table(db,TABLE_NAME)==0)/* 创建表 */
			{
				if(sqlite3_insert(db,TABLE_NAME,&pack_data)==0)/* 数据写入表 */
				{
					dbg_print("Insert data successfully\n");
				}
			}
			sqlite3_close_database(db);
			/* 尝试重新连接 */
			if((connfd=sock_connect(servip,port))<0)/* 未连上 */
			{
				dbg_print("Reconnect server failure:%s\n",strerror(errno));
			}

			else/* 已连上 */
			{
				dbg_print("Reconnect server successfully\n");
			}
		}
		else/* 服务端未断开或已连上 */
		{
			db=sqlite3_open_database(DB_NAME);
			memset(data_buf,0,sizeof(data_buf));
			if(sqlite3_select_all(db,TABLE_NAME)>0)/* 查询数据库中是否存有数据 */
			{
				dbg_print("有数据\n");
				/* 若存有数据 */
				if(sqlite3_select(db,TABLE_NAME,data_buf)>0)/* 从数据库中取出数据 */
				{
					dbg_print("从数据库中取出数据\n");
					if(write(connfd,data_buf,strlen(data_buf))<0)
					{
						dbg_print("Write data to server failure:%s\n",strerror(errno));
						goto CleanUp;
					}
					sqlite3_delete(db,TABLE_NAME);/* 发送一条数据则删除一条数据 */
				}
			
				else
				{
					sqlite3_close_database(db);/* 若数据库中不存在数据则关闭数据库 */
				}
			}
			/* 直接发送数据 */
			dbg_print("直接发送数据\n");
			if(write(connfd,buf,strlen(buf))<0)
			{
				dbg_print("Write data to server failure:%s\n",strerror(errno));
				goto CleanUp;
			}

     	    memset(buf,0,sizeof(buf));
			rv=read(connfd,buf,sizeof(buf));
			if(rv<0)
			{
				dbg_print("Read data from server failure:%s\n",strerror(errno));
				goto CleanUp;
			}
			else if(rv==0)
			{
				dbg_print("Client connect to server get disconnected\n");
				goto CleanUp;
			}
		}

		sleep(time);/* 间隔时间 */
	}
CleanUp:
	close(connfd);

	return 0;
}
