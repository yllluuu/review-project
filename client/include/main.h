/********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  main.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(01/04/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "01/04/24 15:35:07"
 *                 
 ********************************************************************************/
#ifndef	_MAIN_H_
#define _MAIN_H_


#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>
#include <sqlite3.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <fcntl.h>
#include <libgen.h>
#include <netinet/tcp.h>
#include "database.h"
#include "ds18b20_get_temp.h"
#include "data_pack.h"
#include "sock.h"
#include "proc.h"
#include "logger.h"

#define DB_NAME		"cli_dtbase.db"
#define TABLE_NAME	"cli_user"
#define SN			1

int check_interval_time(time_t *last_time,int interval);

#endif

