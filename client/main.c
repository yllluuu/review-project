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


static void print_usage(char *progname);

int main(int argc,char **argv)
{
	sock_t						sock;
	struct sockaddr_in			servaddr;
	char						*servip=NULL;
	int							port=0;
	int							rv=-1;
	int							ch;
	char						*progname=NULL;
	char						*logfile="sock_client.log";
	int							loglevel=LOG_LEVEL_INFO;
	int							logsize=10;
	int							daemon_run=0;
	int							pack_bytes=0;
	pack_data_t					pack_data;/* Declare a struct variable */
	pack_t						pack=packet_data;/* using string packet */
	int							len=20;
	int							interval;
	time_t						last_time=0;
	char						buf[1024];
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

	progname = (char *)basename(argv[0]);

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

	if( log_open( logfile, loglevel, logsize, THREAD_LOCK_NONE) < 0 )
	{
		printf("Initial log system failed\n");
		return 1;
	}

	install_default_signal();

	if(daemon_run)
	{
		daemon(0,0);
	}

	sock_init(&sock,servip,port);

	if(database_table_init(DB_NAME,TABLE_NAME)<0)
	{
		log_error("Create table failure:%s\n",strerror(errno));
	}

	while( !g_signal.stop )
	{
		sample_flag=0;/* set a flag */
		if(check_interval_time(&last_time,interval)>0)
		{
			if(get_temperature(&pack_data.tempera)<0)
			{
				log_error("Get tempareture failure:%s\n",strerror(errno));
			}
			if(get_dev(pack_data.Id,len,SN)<0)
			{
				log_error("Get ID failure:%s\n",strerror(errno));
			}
    		if(get_tm(pack_data.local_t)<0)
			{
				log_error("Get time failure:%s\n",strerror(errno));
			}

			pack_bytes = pack(&pack_data,buf,sizeof(buf));
			if( pack_bytes>0)
			{
				sample_flag = 1;
			}
		}
		/* if the server is disconnected then reconnect it */
		if(sock.connfd<0)
		{
			if((sock_connect(&sock))<0)
			{
			//	printf("Reconnect server failure:%s\n",strerror(errno));
			}
			else
			log_info("Reconnect to server successfully\n");
		}
		/* Check whether the server is disconnected */
		if(socket_alive(sock.connfd)<0)
		{
			sock_close(&sock);
			if(sample_flag)
			{
				if(database_data_insert(TABLE_NAME,&pack_data)<0)
				{
					log_error("Insert data error\n");
					database_close();
				}
				else
				{
					log_info("Insert data successfully\n");
				}
			}
			continue;
		}

		/* The socket is connected */
		if(sample_flag)
		{
			if(sock_send_data(&sock,buf,pack_bytes)<0)
			{
				log_error("Write 1data to server failure:%s\n",strerror(errno));
				if(database_data_insert(TABLE_NAME,&pack_data)<0)
				{
					log_info("Insert data error\n");
					database_close();
					goto CleanUp;
				}
				else
				{
					log_info("Insert data successfully\n");
				}
				continue;
			}
		}

		/* Check if there is data in the database */
		if(database_data_select(TABLE_NAME)>0)
		{
			memset(data_buf,0,sizeof(data_buf));
			if((database_data_take(TABLE_NAME,data_buf))>0)
			{
				if(sock_send_data(&sock,data_buf,pack_bytes)<0)
				{
					//printf("Write data to server failure:%s\n",strerror(errno));
					goto CleanUp;
				}
				else
				{
					database_data_delete(TABLE_NAME);
				}
			}
		}
		usleep(50000);
	}//end of while

CleanUp:
	sock_close(&sock);

	return 0;
}

void print_usage(char *progname)
{
	printf("%s usage:\n",progname);
	printf("-i(--ipaddr):specify server IP address.\n");
	printf("-p(--port):specify server port.\n");
	printf("-t(--time):sampling interval.\n");
	printf("-b(--daemon):runs in the background.\n");
	printf("-h(--help):print this help information.\n");

	return ;
}

/* set interval */
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

