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
	sock_s						sock;
	struct sockaddr_in			servaddr;
	char						*servip=NULL;
	int							port=0;
	int							rv=-1;
	char						buf[1024];
	int							ch;
	int							daemon_run=0;
	sock_data					pack_data;
	int							len=20;
	int							interval;
	time_t						last_time=0;
	sqlite3						*db;
	int							rows;
	char						data_buf[1024];
	int							sample_flag=0;
	struct option				opts[]=
	{
		{"ipaddr",required_argument,NULL,'i'},
		{"port",required_argument,NULL,'p'},
		{"interval",required_argument,NULL,'t'},
		{"help",no_argument,NULL,'h'},
		{"deamon",no_argument,NULL,'b'},
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
				interval=atoi(optarg);
				break;
			case 'd':
				daemon_run=1;
				break;
			case 'h':
				print_usage(argv[0]);
				return 0;
			default:
				break;
		}
	}

	if(!servip||!port)
	{
		print_usage(argv[0]);
		return 0;
	}

	//connfd=sock_connect(servip,port);/* 连接服务器端 */
	sock_init(&sock,servip,port);
// 	db=sqlite3_open_database(DB_NAME);
	if(sqlite3_create_table(DB_NAME,TABLE_NAME)<0)/*  创建表  */
	{
		dbg_print("Create table failure:%s\n",strerror(errno));
	}

	while(1)
	{
		sample_flag=0;
		if(check_interval_time(&last_time,interval)>0)
		{
			memset(buf,0,sizeof(buf));
			if(get_temperature(&pack_data.temp)<0)
			{
				dbg_print("Get tempareture failure:%s\n",strerror(errno));
			}
			if(get_dev(pack_data.Id,len)<0)
			{
				dbg_print("Get ID failure:%s\n",strerror(errno));
			}
    		if(get_tm(pack_data.localt)<0)
			{
				dbg_print("Get time failure:%s\n",strerror(errno));
			}

			snprintf(buf,sizeof(buf),"%s %f %s",pack_data.Id,pack_data.temp,pack_data.localt);
			sample_flag=1;
		}
		/* 服务器若断开则重连 */
		if(sock.connfd<0)
		{
			if((sock_connect(&sock))<0)
			{
				dbg_print("Reconnect server failure:%s\n",strerror(errno));
			}
			else
			dbg_print("Reconnect to server successfully\n");
		}
		/* 判断服务器是否断开 */
		if(socket_alive(sock.connfd)<0)
		{
			close(sock.connfd);
			sock.connfd=-1;
			if(sample_flag)
			{
				if(sqlite3_insert(TABLE_NAME,&pack_data)<0)
				{
					dbg_print("Insert data error\n");
					sqlite3_close_database();
				}
				else
				dbg_print("Insert data successfully\n");
			}
			continue;
		}
		/*  服务端未断开或已连上  */
		if(sample_flag)
		{
			if(write(sock.connfd,buf,strlen(buf))<0)
			{
				dbg_print("Write 1data to server failure:%s\n",strerror(errno));
				if(sqlite3_insert(TABLE_NAME,&pack_data)<0)
				{
					dbg_print("Insert data error\n");
					sqlite3_close_database();
					goto CleanUp;
				}
				else
				{
					dbg_print("Insert data successfully\n");
				}
				continue;
			}
		}
		/* 查询数据库中是否存有数据 */
		if(sqlite3_select_all(TABLE_NAME)>0)
		{
			memset(data_buf,0,sizeof(data_buf));
			if((sqlite3_select(TABLE_NAME,data_buf))>0)
			{
				printf("%s\n",data_buf);
				dbg_print("从数据库中取出数据\n");
				if(write(sock.connfd,data_buf,strlen(data_buf))<0)
				{
					dbg_print("Write data to server failure:%s\n",strerror(errno));
					goto CleanUp;
				}
				else
				{
					sqlite3_delete(TABLE_NAME);
				}
			}
		}
		usleep(50000);
	}//end of while

CleanUp:
	close(sock.connfd);
	sock.connfd=-1;

	return 0;
}

int check_interval_time(time_t *last_time,int interval)
{
	int			need=0;
	time_t		now;

	time(&now);
	if(now>=*last_time+interval)
	{
		need=1;
		*last_time=now;
	}

	return need;
}

