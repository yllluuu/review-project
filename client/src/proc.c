/*********************************************************************************
 *      Copyright:  (C) 2024 yanglu
 *                  All rights reserved.
 *
 *       Filename:  proc.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(17/04/24)
 *         Author:  yanglu
 *      ChangeLog:  1, Release initial version on "17/04/24 22:53:15"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "proc.h"
#include "logger.h"

proc_signal_t		g_signal={0};

void proc_default_sighandler(int sig)
{
	switch(sig)
	{
		case SIGINT:
			log_warn("SIGINT - stopping\n");
			g_signal.stop=1;
			break;

		case SIGTERM:
			log_warn("SIGTERM - stopping\n");
			g_signal.stop=1;
			break;

		case SIGSEGV:
			log_warn("SIGSEGV - stopping\n");
#if 0
			if(g_signal.stop)
				exit(0);

			g_signal.stop=1;
#endif
			break;

		case SIGPIPE:
			log_warn("SIGPIPE - stopping\n");
			break;

		default:
			break;
	}
}

/* Description:install default signal process functions */
void install_default_signal(void)
{
	struct sigaction	sigact,sigign;

	sigemptyset(&sigact.sa_mask);
	sigact.sa_flags=0;
	sigact.sa_handler=proc_default_sighandler;

	sigemptyset(&sigign.sa_mask);
	sigign.sa_flags=0;
	sigign.sa_handler=SIG_IGN;

	sigaction(SIGTERM,&sigact,0);
	sigaction(SIGINT,&sigact,0);
	sigaction(SIGPIPE,&sigact,0);
}
