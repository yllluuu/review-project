/********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  sock_ali_rec.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(01/04/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "01/04/24 11:16:37"
 *                 
 ********************************************************************************/
#ifndef _SOCK_ALI_REC_H
#define _SOCK_ALI_REC_H

#define CONFIG_DEBUG
#ifdef  CONFIG_DEBUG
#define dbg_print(format,args...)	printf(format,##args)
#else
#define dbg_print(format,args...)	do{} while(0)
#endif

typedef struct sock_t
{
	char		host[128];
	int			port;
	int			connfd;
}sock_s;

int sock_init(sock_s *sock,char *host,int port);
int socket_alive(int fd);
int sock_connect(sock_s *sock);

#endif
