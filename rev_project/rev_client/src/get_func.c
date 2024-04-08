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
#include"get_func.h"

void print_usage(char* programe)
{
	dbg_print("%s usage:\n",programe);
	dbg_print("-i(--ipaddr):specify server IP address.\n");
	dbg_print("-p(--port):specify server port.\n");
	dbg_print("-t(--time):Sampling interval.\n");
	dbg_print("-h(--help):print this help information.\n");
	return ;
}

int get_dev(char *ID,int len)
{
	int		sn=1;
	int		ret;
	ret=snprintf(ID,len,"%05d",sn);
	if(ret<0 || ret>=len)
	{
		return -1;
	}
	return 0;
}

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
