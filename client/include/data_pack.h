/********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  get_func.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(01/04/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "01/04/24 13:27:47"
 *                 
 ********************************************************************************/

#ifndef	_DATA_PACK_H
#define _DATA_PACK_H

#define DEV_LEN			16
#define TIME_LEN		32

/* Description:Define a new struct named pack_data_t */
typedef struct pack_data_s
{
	char	Id[DEV_LEN];
	char	local_t[TIME_LEN];
	float	tempera;
}pack_data_t;

/* Description:Define a new struct pointer */
typedef int (*pack_t)(pack_data_t *pack_data,char *buf,int size);

/* Description:Get id of the device */
extern int get_dev(char* ID,int len,int sn);

/* Description:Get current time */
extern int get_tm(char* localt);

extern int packet_data(pack_data_t *pack_info,char *buf,int size);

#endif
