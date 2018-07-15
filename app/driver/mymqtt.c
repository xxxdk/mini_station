#include "ets_sys.h"
#include "osapi.h"
#include "os_type.h"
#include "c_types.h"
#include "mem.h"

#include "mqtt/mqtt.h"
#include "mqtt/debug.h"
#include "mqtt_config.h"
#include "driver/mymqtt.h"
#include "driver/led_flash.h"
#include "driver/get_sensor_data.h"

MQTT_Client mqttClient;
MQTT_Client* client = NULL;
os_timer_t mqttimer;

char mqtt_client_id[14];

void ICACHE_FLASH_ATTR mqttInfoInit()
{
	os_sprintf(mqtt_client_id,"client_%06x",system_get_chip_id());
	mqtt_client_id[13] = '\0';
	INFO("mqtt_client_id: %s \r\n",mqtt_client_id);
}


void ICACHE_FLASH_ATTR mqttPublish(mqtt_topic_sms* mts)
{
	MQTT_Publish(client, mts->topic, mts->message, os_strlen(mts->message), 0, 0);//client,topic,data,len,qos,retain
	INFO("mqttPublish\r\n");
}

void ICACHE_FLASH_ATTR mqttConnect()
{
	led_stop_flash();
//	struct ip_info ipconfig;
//	wifi_get_ip_info(STATION_IF, &ipconfig);
//	if(wifi_station_get_connect_status() == STATION_GOT_IP && ipconfig.ip.addr != 0)
//	{
		MQTT_Connect(&mqttClient);
		INFO("mqttConnect\n\n");
//	}else{
//		wifi_station_connect();
//	}
}

void ICACHE_FLASH_ATTR mqttReConnect()
{
	INFO("mqttReConnect\n\n");
}

void ICACHE_FLASH_ATTR mqttDisConnect()
{
	MQTT_Disconnect(&mqttClient);
	led_start_flash();
	INFO("mqttDisConnect\n\n");
}

void ICACHE_FLASH_ATTR mqttConnectedCb(uint32_t *args)
{
	os_timer_disarm(&mqttimer);
	client = (MQTT_Client*)args;
	sensorStart();
//	MQTT_Subscribe(client, "dev", 0);			//client,topic,qos
	INFO("MQTT: Connected\r\n");
}

void ICACHE_FLASH_ATTR mqttDisconnectedCb(uint32_t *args)
{
	client = (MQTT_Client*)args;
	INFO("MQTT: Disconnected\r\n");
}

void ICACHE_FLASH_ATTR mqttPublishedCb(uint32_t *args)
{
	client = (MQTT_Client*)args;
	INFO("MQTT: Published\r\n");
}

void ICACHE_FLASH_ATTR mqttDataCb(uint32_t *args, const char* topic, uint32_t topic_len, const char *data, uint32_t data_len)
{
	char *topicBuf = (char*)os_zalloc(topic_len+1),
			*dataBuf = (char*)os_zalloc(data_len+1);

	client = (MQTT_Client*)args;

	os_memcpy(topicBuf, topic, topic_len);
	topicBuf[topic_len] = 0;

	os_memcpy(dataBuf, data, data_len);
	dataBuf[data_len] = 0;

	INFO("Receive topic: %s, data: %s \r\n", topicBuf, dataBuf);
	os_free(topicBuf);
	os_free(dataBuf);
}

void ICACHE_FLASH_ATTR mqttInit()
{
	INFO("\r\nSystem started ...\r\n");

	os_timer_disarm(&mqttimer);
	os_timer_setfn(&mqttimer,(os_timer_func_t *)mqttConnect,NULL);

	MQTT_InitConnection(&mqttClient, MQTT_HOST, MQTT_PORT, 0);//host,port,ssl(MQTT_SECURITY)
	MQTT_InitClient(&mqttClient, mqtt_client_id, MQTT_USER, MQTT_PASS, MQTT_KEEPALIVE, 1);//id,user,pw,alive

	MQTT_OnConnected(&mqttClient, mqttConnectedCb);
	MQTT_OnDisconnected(&mqttClient, mqttDisconnectedCb);
	MQTT_OnPublished(&mqttClient, mqttPublishedCb);
	MQTT_OnData(&mqttClient, mqttDataCb);

	os_timer_arm(&mqttimer,3000,1);
}
