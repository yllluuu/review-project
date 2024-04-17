/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  get_func.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(01/04/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "01/04/24 13:18:30"
 *                 
 ********************************************************************************/
#include<stdio.h>
#include<string.h>
#include<time.h>
#include"data_pack.h"
#include"sock.h"
#include"logger.h"

/* Description:Get id of the device */
int get_dev(char *ID,int len,int sn)
{
	int			ret;

	if( !ID || len<DEV_LEN )
	{
		log_error("Invalid input arugments\n");
		return -1;
	}

	ret=snprintf(ID,len,"%05d",sn);
	if(ret<0 || ret>=len)
	{
		return -1;
	}
	return 0;
}

/* Description:Get current time */
int get_tm(char* localt)
{
	time_t		seconds;
	struct tm   *local;
	int			ret;

	time(&seconds);

	local=localtime(&seconds);
	if(local==NULL)
	{
		return -2;
	}

	ret=snprintf(localt,64,"%d/%d/%d-%d:%d:%d\n",local->tm_year+1900,local->tm_mon+1,local->tm_mday,local->tm_hour,local->tm_min,local->tm_sec);

	if(ret<0||ret>=64)
	{
		return -1;
	}
	return 0;
}

/* Description:define a new string format */
int packet_data(pack_data_t *pack_info, char *buf, int size)
{
	if( !pack_info || !buf || size<0 )
	{
		log_error("Invalid input arguments\n");
		return -1;
	}

	memset(buf,0,size);
	snprintf(buf,size,"%s %f %s",pack_info->Id,pack_info->tempera,pack_info->local_t);

	return strlen(buf);
}
