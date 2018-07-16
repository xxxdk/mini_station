#ifndef __MYMQTT_H__
#define __MYMQTT_H__

typedef struct mqtt_topic_sms
{
	const char* topic;
	const char* message;
}mqtt_topic_sms;


void ICACHE_FLASH_ATTR mqttInit();
void ICACHE_FLASH_ATTR mqttDisConnect();
void ICACHE_FLASH_ATTR mqttReConnect();
void ICACHE_FLASH_ATTR mqttConnect();
void ICACHE_FLASH_ATTR mqttPublish(mqtt_topic_sms* mts);
void ICACHE_FLASH_ATTR mqttInfoInit();
void ICACHE_FLASH_ATTR mqtt_timer_close();

#endif
