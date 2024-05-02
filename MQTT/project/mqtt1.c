/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  ds18b20_get_temp.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(31/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "31/03/24 16:12:55"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "mosquitto.h"
#include "cJSON.h"
#include "dictionary.h"
#include "iniparser.h"
#include "mqtt_config.h"

#define	LEN_T		64
#define LEN_ID		20
#define	PATH		"./Mqtt.ini"
#define KEEPALIVE	60

void connect_callback(struct mosquitto *mosq, void *obj, int rc);
int get_tm(char* localt, int size);
int get_dev(char *id,int len,int sn);
int get_temperature(float *temp);

int main(int argc,char **argv)
{
	int					rv = 0;
	struct mosquitto	*mosq = NULL;
	void				*obj=NULL;
	mqtt_user_t			mqtt;

	memset(&mqtt, 0, sizeof(mqtt));
	get_config(PATH, &mqtt);

	rv = mosquitto_lib_init();
	if( rv < 0 )
	{
		printf("mosquitto init failed:%s\n",strerror(errno));
		goto Cleanup;
	}
	
	mosq = mosquitto_new(mqtt.clientid, true, (void *)&mqtt);
	if( !mosq )
	{
		printf("create new mosq failed:%s\n",strerror(errno));
		goto Cleanup;
	}

	rv = mosquitto_username_pw_set(mosq, mqtt.username, mqtt.password);
	if( rv != MOSQ_ERR_SUCCESS )
	{
		printf("set username and password failed:%s\n",strerror(errno));
		goto Cleanup;
	}
	printf("create mosquitto successfully");

	mosquitto_connect_callback_set(mosq, connect_callback);

	while(1)
	{
		rv = mosquitto_connect(mosq, mqtt.brokeraddress, mqtt.brokerport, KEEPALIVE);
		if( rv != MOSQ_ERR_SUCCESS )
		{
			printf("connect to broker failed:%s\n",strerror(errno));
			goto Cleanup;
		}
		printf("connect to broker ok\n");
	//	mosquitto_loop_start(mosq);
		mosquitto_loop_forever(mosq,-1,1);
		//mosquitto_disconnect(mosq);

		sleep(5);
	}

Cleanup:
	mosquitto_lib_cleanup();
	mosquitto_destroy(mosq);

	return 0;
}

/*  Description:Get id of the device */
int get_dev(char *id,int len,int sn)
{
	int                     ret;

	if( !id || len<LEN_ID )
	{
		printf("Invalid input arugments\n");
		return -1;
	}

	ret=snprintf(id,len,"%05d",sn);
	if(ret<0 || ret>=len)
	{
		return -1;
	}
	return 0;
}

/*  Description:Get current time */
int get_tm(char* localt, int size)
{
	time_t          seconds;
	struct tm   *local;
	int                     ret;

	time(&seconds);

	local=localtime(&seconds);
	if(local==NULL)
	{
		return -2;
	}

	ret=snprintf(localt,size,"%d/%d/%d-%d:%d:%d",local->tm_year+1900,local->tm_mon+1,local->tm_mday,local->tm_hour,local->tm_min,local->tm_sec);

	if(ret<0||ret>=64)
	{
		return -1;
	}
	return 0;
}

/* Description:get temperature */
int get_temperature(float *temp)
{
	int                             fd=-1;
	char                            buf[128];
	char                            *ptr=NULL;
	DIR                             *dirp=NULL;
	struct dirent           		*direntp=NULL;
	char                            w1_path[64]="/sys/bus/w1/devices/";
	char                            chip_sn[32];
	int                             found=0;

	dirp=opendir(w1_path);
	if(!dirp)
	{
		printf("Open folder %s failure:%s\n",w1_path,strerror(errno));
		return -1;
	}

	while(NULL!=(direntp=readdir(dirp)))
	{
		if(strstr(direntp->d_name,"28-"))
		{
			strncpy(chip_sn,direntp->d_name,sizeof(chip_sn));
			found=1;
		}
	}

	closedir(dirp);

	if(!found)
	{
		printf("Cannot find ds18b20 chipset\n");
		return -2;
	}

	strncat(w1_path,chip_sn,sizeof(w1_path)-strlen(w1_path));
	strncat(w1_path,"/w1_slave",sizeof(w1_path)-strlen(w1_path));

	if((fd=open(w1_path,O_RDONLY))<0)
	{
		printf("Open file failure:%s\n",strerror(errno));
		goto cleanup;
		return -3;
	}

	memset(buf,0,sizeof(buf));
	if(read(fd,buf,sizeof(buf))<0)
	{
		printf("Read data from fd[%d] failure:%s\n",fd,strerror(errno));
		goto cleanup;
		return -4;
	}

	ptr=strstr(buf,"t=");
	if(!ptr)
	{
		printf("Cannot find t= string\n");
		goto cleanup;
		return -5;
	}

	ptr +=2;
	*temp=atof(ptr)/1000;

cleanup:
	if(fd)
		close(fd);
	if(!dirp)
		closedir(dirp);

	return 0;
}

void connect_callback(struct mosquitto *mosq, void *obj, int rc)
{
	float		temp;
	char		local_t[LEN_T];
	int			size=sizeof(local_t);
	char		id[LEN_ID];
	int			sn=1;
	cJSON		*root;
	cJSON		*item;
	mqtt_user_t	*mqtt;
	char		*msg;

	printf("connection successful cJSON cal packaging\n");

	if( get_temperature(&temp) < 0 )
	{
		printf("get temperature failed:%s\n",strerror(errno));
	}

	if( get_tm(local_t,size) < 0 )
	{
		printf("get time failed:%s\n",strerror(errno));
	}

	if( get_dev(id, LEN_ID, sn) < 0 )
	{
		printf("get device id failed:%s\n",strerror(errno));
	}

	mqtt = (mqtt_user_t *)obj;

	root = cJSON_CreateObject();
	item = cJSON_CreateObject();

	cJSON_AddItemToObject(root, "method", cJSON_CreateString(mqtt->method));
	cJSON_AddItemToObject(root, "version", cJSON_CreateString(mqtt->version));
	cJSON_AddItemToObject(item, "CurrentTemperature", cJSON_CreateNumber(temp));
	cJSON_AddItemToObject(root, "params", item);
	cJSON_AddItemToObject(root, "time", cJSON_CreateString(local_t));
	cJSON_AddItemToObject(root, "device id", cJSON_CreateString(id));

	msg = cJSON_Print(root);
	printf("%s\n",msg);

	if( !rc )
	{
		if( mosquitto_publish(mosq, NULL, mqtt->topic, strlen(msg), msg, mqtt->Qos, true) != MOSQ_ERR_SUCCESS )
		{
			printf("publish topic failed:%s\n",strerror(errno));
		}
	}
	mosquitto_disconnect(mosq);
	return ;
}
