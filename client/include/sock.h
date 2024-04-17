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
#ifndef _SOCK_H
#define _SOCK_H

#define HOST_LEN		128

/* Description:Define a new struct named sock_t */
typedef struct sock_s
{
	char		host[HOST_LEN];
	int			port;
	int			connfd;
}sock_t;

/* Description:Initialize the struct */
extern int sock_init(sock_t *sock,char *host,int port);

/* Description:Determine whether the server is disconnected */
extern int socket_alive(int fd);

/* Description:Connect to server */
extern int sock_connect(sock_t *sock);

/* Description:Send data to the server */
extern int sock_send_data(sock_t *sock,char *buf,int bytes);

/* Description:Close socket */
extern int sock_close(sock_t *sock);

#endif
