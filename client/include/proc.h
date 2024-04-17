/********************************************************************************
 *      Copyright:  (C) 2024 yanglu
 *                  All rights reserved.
 *
 *       Filename:  proc.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(16/04/24)
 *         Author:  yanglu
 *      ChangeLog:  1, Release initial version on "16/04/24 13:12:31"
 *                 
 ********************************************************************************/

#ifndef  _PROC_H_
#define  _PROC_H_

#include <signal.h>


typedef struct proc_signal_s
{
	int			signal;
	unsigned	stop;
}proc_signal_t;

extern proc_signal_t	g_signal;

extern void proc_default_sighandler(int sig);

/* Description:install default signal process functions */ 
extern void install_default_signal(void);

#endif
