/********************************************************************************
 *      Copyright:  (C) 2024 LiYi<1751425323@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  mqtt_config.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(26/04/24)
 *         Author:  LiYi <1751425323@qq.com>
 *      ChangeLog:  1, Release initial version on "26/04/24 20:51:51"
 *                 
 ********************************************************************************/
#ifndef		_MQTT_CONFIG_H_
#define 	_MQTT_CONFIG_H_

#define	SIZE	1024
#define	DEFAULT_BROKER_ADDRESS		"iot-06z00htt4df99dk.mqtt.iothub.aliyuncs.com"
#define	DEFAULT_BROKER_PORT			1883
#define DEFAULT_USERNAME			"temp&k16b1aH4UjO"
#define DEFAULT_PASSWORD			"924000ba040582d759c3d1b41f209745b6d1b74bea10120ccdffd846794fc8c1"
#define DEFAULT_CLIENTID			"k16b1aH4UjO.temp|securemode=2,signmethod=hmacsha256,timestamp=1714134288886|"
#define DEFAULT_METHOD				"thing.event.property.post"
#define DEFAULT_VERSION				"1.0.0"
#define DEFAULT_Qos					0
#define DEFAULT_JSONIDTF			"CurrentTemperature"
#define  DEFAULT_PUB_TOPIC			"/sys/k16b1aH4UjO/temp/thing/event/property/post"

typedef struct mqtt_user_s
{
	char 		brokeraddress[SIZE];
	int			brokerport;
	char		username[SIZE];
	char		password[SIZE];
	char		clientid[SIZE];
	char		identifier[SIZE];
	char		topic[SIZE];
	char		method[SIZE];
	char		version[SIZE];
	int			Qos;
//	int			id;
}mqtt_user_t;

extern int get_config(char *path,mqtt_user_t *mqtt);

#endif
