/*********************************************************************************
 *      Copyright:  (C) 2024 LiYi<1751425323@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  mqtt_config.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(26/04/24)
 *         Author:  LiYi <1751425323@qq.com>
 *      ChangeLog:  1, Release initial version on "26/04/24 20:51:31"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <string.h>
#include "mqtt_config.h"
#include "iniparser.h"

int	get_config(char *path,mqtt_user_t *mqtt)
{
	dictionary		*ini = NULL;
	const char		*brokeraddress;
	int	  			brokerport;
	int				id;
	const char		*username;
	const char		*password;
	const char		*clientid;
	const char		*topic;
	int				Qos;

	const char		*method;
	const char		*time;
	const char		*jsonid;
	const char		*identifier;
	const char		*version;

	if( !path || !mqtt )
	{
		printf("invalid_argument:%s\n",__func__);
		return -1;
	}

	ini = iniparser_load(path);
	if( !ini )
	{
		printf("iniparser_load failure\n");
		return -2;
	}

	brokeraddress = iniparser_getstring(ini, "server:hostname", DEFAULT_BROKER_ADDRESS);
	brokerport = iniparser_getint(ini, "server:port", DEFAULT_BROKER_PORT);
	username = iniparser_getstring(ini, "user_name_pwd:username", DEFAULT_USERNAME);
	password = iniparser_getstring(ini, "user_name_pwd:pwd", DEFAULT_PASSWORD);
	clientid = iniparser_getstring(ini, "clientid:clientid", DEFAULT_CLIENTID);
	identifier = iniparser_getstring(ini, "json:identifier", DEFAULT_JSONIDTF);
	topic = iniparser_getstring(ini, "pub_topic:topic", DEFAULT_PUB_TOPIC);
	method = iniparser_getstring(ini, "json:method", DEFAULT_METHOD);
	Qos = iniparser_getint(ini, "Qos:Qos", DEFAULT_Qos);
	//id = iniparser_getstring(ini, "json:id", DEFAULT_ID);
	version = iniparser_getstring(ini, "json:version", DEFAULT_VERSION);

	strncpy(mqtt->brokeraddress, brokeraddress, SIZE);
	mqtt->brokerport = brokerport;
	strncpy(mqtt->username, username, SIZE);
	strncpy(mqtt->password, password, SIZE);
	strncpy(mqtt->clientid, clientid, SIZE);
	strncpy(mqtt->identifier, identifier, SIZE);
	strncpy(mqtt->topic, topic, SIZE);
	strncpy(mqtt->method, method, SIZE);
	strncpy(mqtt->version, version, SIZE);
	mqtt->Qos = Qos;
//	mqtt->id = id;

	iniparser_freedict(ini);

	return 0;
}
