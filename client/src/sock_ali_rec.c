/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  sock_ali_rec.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(01/04/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "01/04/24 10:42:55"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netinet/tcp.h>
#include"sock_ali_rec.h"

int socket_alive(int fd)
{
	struct tcp_info		info;
	int					len=sizeof(info);

	if(fd<0)
	{
		return -1;
	}
	getsockopt(fd,IPPROTO_TCP,TCP_INFO,&info,(socklen_t *)&len);
	if((info.tcpi_state==TCP_ESTABLISHED))
	{
		return 1;
	}
	else
	{
		dbg_print("The server is disconnected\n");
		return -1;
	}
}
/* 连接socket */
int sock_connect(sock_s *sock)
{
	struct sockaddr_in		servaddr;
	int						connfd;

	dbg_print("Trying to connect server...\n");

	connfd=socket(AF_INET,SOCK_STREAM,0);
	if(connfd<0)
	{
		dbg_print("Create socket failure:%s\n",strerror(errno));
	}

	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(sock->port);
	inet_aton(sock->host,&servaddr.sin_addr);
	if(connect(connfd,(struct sockaddr*)&servaddr,sizeof(servaddr))<0)
	{
		dbg_print("Connect to server failure:%s\n",strerror(errno));
		close(connfd);
		return -1;
	}
	else
	{
		sock->connfd=connfd;
		dbg_print("Connect to server successfully\n");
		return 1;
	}
}
/* 初始化sock结构体*/
int sock_init(sock_s *sock,char *host,int port)
{
	if(!sock||port<=0)
	{
		return -1;
	}

	memset(sock,0,sizeof(sock));
	sock->connfd=-1;
	sock->port=port;
	if(host)
	{
		strncpy(sock->host,host,sizeof(host));
	}
	return 0;
}
