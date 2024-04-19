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
#include <netdb.h>

#include "sock.h"
#include "logger.h"

/* Description:Determine whether the server is disconnected */
int socket_alive(int fd)
{
	struct tcp_info		info;
	int					len=sizeof(info);
	getsockopt(fd,IPPROTO_TCP,TCP_INFO,&info,(socklen_t *)&len);
	if((info.tcpi_state==TCP_ESTABLISHED))
	{
		return 0;
	}
	else
	{
		log_error("The server is disconnected\n");
		return -1;
	}
}

/* Description:Connect to server */
int sock_connect(sock_t *sock)
{
	struct sockaddr_in		servaddr;
	int						connfd;
	log_info("Trying to connect server...\n");

	connfd=socket(AF_INET,SOCK_STREAM,0);
	if(connfd<0)
	{
		log_error("Create socket failure:%s\n",strerror(errno));
		return -1;
	}

	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(sock->port);
	inet_aton(sock->host,&servaddr.sin_addr);
	if(connect(connfd,(struct sockaddr*)&servaddr,sizeof(servaddr))<0)
	{
		log_error("Connect to server failure:%s\n",strerror(errno));
		close(connfd);
		return -1;
	}
	else
	{
		sock->connfd = connfd;
		log_error("Connect to server successfully\n");
		return 0;
	}
}


/* Description:connect to server */
int socket_connect(sock_t *sock)
{
	int						rv=0;
	int						fd=-1;
	int 					err;//getaddrinfo返回值
	struct addrinfo			hints,*result,*rp;
	struct in_addr			inaddr;
	struct sockaddr_in		addr;
	char					service[20];
	char					Buf[32];

	if(!sock)
	{
		return -1;
	}

	memset(&hints,0,sizeof(struct addrinfo));
	hints.ai_family=AF_INET;
	hints.ai_socktype=SOCK_STREAM;
	hints.ai_protocol=IPPROTO_TCP;

	/* No domain name resolution is required */
	if(inet_aton(sock->host,&inaddr))
	{
		hints.ai_flags |= AI_NUMERICHOST;
	}

	snprintf(service,sizeof(service),"%d",sock->port);
	if((err=getaddrinfo(sock->host,service,&hints,&result)!=0))
	{
		log_error("getaddrinfo failure\n");
		return -2;
	}
	log_info("getaddrinfo ok\n");

	for(rp=result;rp!=NULL;rp=rp->ai_next)//遍历result指针中的所有套接字地址结构
	{
		fd=socket(rp->ai_family,rp->ai_socktype,rp->ai_protocol);

		if(fd<0)
		{
			log_error("create socket failure\n");
			rv=-3;
			continue;
		}
		
		rv=connect(fd,rp->ai_addr,sizeof(addr));
		if(rv == 0)
		{
			sock->connfd=fd;
			log_info("connect to server[%d] successfully\n",sock->connfd);
			break;
		}
		else
		{
//			inet_ntop(rp->ai_family,&(((struct sockaddr_in*)(rp->ai_addr))->sin_addr),Buf,sizeof(Buf));
//			printf("%s\n",Buf);
			close(fd);
			continue;
		}
	}

	freeaddrinfo(result);
	return rv;
}

/* Description:Close socket */
int sock_close(sock_t *sock)
{
	if( !sock )
	{
		return -1;
	}

	close(sock->connfd);
	sock->connfd = -1;

	return 0;
}

/* Description:Initialize the struct */
int sock_init(sock_t *sock,char *host,int port)
{
	if( !sock || port<=0 )
	{
		return -1;
	}

	memset(sock,0,sizeof(*sock));
	sock->connfd=-1;
	sock->port=port;
	if(host)
	{
		strncpy(sock->host,host,HOST_LEN);
	}

	return 0;
}

/* Description:Send data to the server */
int sock_send_data(sock_t *sock,char *buf,int bytes)
{
	int 		rv=0;
	int			i=0;
	int			left_bytes=bytes;

	if(!sock||!buf||bytes<0)
	{
		return -1;
	}

	while(left_bytes > 0)
	{
		rv=write(sock->connfd,&buf[i],left_bytes);
		if(rv<0)
		{
			log_error("socket[%d] write data failure\n",sock->connfd);
			sock_close(sock);
			return -1;
		}
		else if(rv==left_bytes)
		{
			log_info("socket[%d] write %d bytes data over\n",bytes);
			return 0;
		}
		else
		{
			i+=rv;
			left_bytes-=rv;
			continue;
		}
	}
	return 0;
}
