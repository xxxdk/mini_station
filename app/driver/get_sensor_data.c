/*
 *  Pin number:
 *  -----------
 *  Pin 0 = GPIO16
 *  Pin 1 = GPIO5
 *  Pin 2 = GPIO4
 *  Pin 3 = GPIO0
 *  Pin 4 = GPIO2
 *  Pin 5 = GPIO14
 *  Pin 6 = GPIO12
 *  Pin 7 = GPIO13
 *  Pin 8 = GPIO15
 *  Pin 9 = GPIO3
 *  Pin 10 = GPIO1
 *  Pin 11 = GPIO9
 *  Pin 12 = GPIO10
 */

#include "ets_sys.h"
#include "osapi.h"
#include "mem.h"
#include "os_type.h"
#include "user_interface.h"

#include "mqtt/debug.h"
#include "driver/dht22.h"
#include "driver/bh1750.h"
#include "driver/mymqtt.h"
#include "driver/gpio_append.h"

#define DELAY 180000
#define TEMP_STR	"{\"name\":\"temperature\",\"temp\":%d.%d,\"time\":%lu000,\"dev\":\"x1\"}"
#define HUM_STR		"{\"name\":\"humidity\",\"hum\":%d.%d,\"time\":%lu000,\"dev\":\"x1\"}"
#define LIGHT_STR	"{\"name\":\"illuminance\",\"light\":%d,\"time\":%lu000,\"dev\":\"x1\"}"

uint32 Unix_timestamp;

LOCAL os_timer_t sensor_timer,sntp_timer,get_data_timer;
DHT_Sensor sensor;
LOCAL float tempData;
LOCAL float humData;
LOCAL int lightData;
LOCAL int get_count;

LOCAL float temp_array[3], hum_array[3];
LOCAL int light_array[3];

LOCAL DHT_Sensor_Data ICACHE_FLASH_ATTR dht22Cb()
{
	static uint8_t i;
	uint8_t pin;
	DHT_Sensor_Data dht_data;

	if (DHTRead(&sensor, &dht_data))
	{
	    char buff[20];
	    INFO("\r\nTemp: %s  ", DHTFloat2String(buff, dht_data.temperature));
	    INFO("Hum: %s\r\n", DHTFloat2String(buff, dht_data.humidity));
	} else {
	    INFO("Failed to read temperature and humidity sensor on GPIO%d\n", pin);
	}

	return dht_data;
}

LOCAL void ICACHE_FLASH_ATTR dht22Start()
{
	sensor.pin = 5;
	sensor.type = DHT22;
	DHTInit(&sensor);
}

LOCAL int ICACHE_FLASH_ATTR bh1750Cb()
{
	int data;
	data = GetLight();
    if(data == -1)
    	INFO("Error get light value\r\n");
    else
    	INFO("\r\nlight: %d\r\n", data);

    return data;
}

LOCAL void ICACHE_FLASH_ATTR bh1750Start()
{
	BH1750Init();
}

void ICACHE_FLASH_ATTR sensorInit()
{
	dht22Start();
	bh1750Start();
}

void ICACHE_FLASH_ATTR sensorStop()
{
	os_timer_disarm(&sensor_timer);
	os_timer_disarm(&sntp_timer);
	os_timer_disarm(&get_data_timer);
}

void ICACHE_FLASH_ATTR getTimeStamp()
{
	os_timer_disarm(&sntp_timer);
	Unix_timestamp = sntp_get_current_timestamp() - 28800;
	INFO("current time : %lu, %s\r\n", Unix_timestamp, sntp_get_real_time(Unix_timestamp));
}

void ICACHE_FLASH_ATTR pubSensorData()
{
	mqtt_topic_sms ts;

	getTimeStamp();
	if (Unix_timestamp == 0) {
		INFO("did not get a valid time from sntp server\n");
		os_timer_disarm(&sensor_timer);
		os_timer_arm(&sntp_timer, 1000, 1);
	} else {
		ts.topic = "dev";
		ts.message = (char*)os_zalloc(128);

		os_sprintf(ts.message, TEMP_STR, (int)(tempData),
				(int)((tempData - (int)tempData)*100),Unix_timestamp);
		mqttPublish(&ts);

		os_sprintf(ts.message, HUM_STR, (int)(humData),
					(int)((humData - (int)humData)*100),Unix_timestamp);
		mqttPublish(&ts);

		os_sprintf(ts.message, LIGHT_STR, lightData, Unix_timestamp);
		mqttPublish(&ts);

		os_free(ts.message);
	}
}

void ICACHE_FLASH_ATTR getSensorData()
{
	DHT_Sensor_Data dht_data;
	sensorInit();
	dht_data = dht22Cb();

	temp_array[get_count] = dht_data.temperature;
	hum_array[get_count] = dht_data.humidity;
	light_array[get_count] = bh1750Cb();
	get_count++;
	if(get_count == 3){
		tempData = (temp_array[0] + temp_array[1] + temp_array[2]) / 3;
		humData = (hum_array[0] + hum_array[1] + hum_array[2]) / 3;
		lightData = (light_array[0] + light_array[1] + light_array[2]) / 3;
		get_count = 0;
	}
}

void ICACHE_FLASH_ATTR sensorStart()
{
	get_count = 0;
	os_timer_disarm(&sensor_timer);
	os_timer_setfn(&sensor_timer, (os_timer_func_t *)pubSensorData, NULL);

	os_timer_disarm(&sntp_timer);
	os_timer_setfn(&sntp_timer, (os_timer_func_t *)pubSensorData, NULL);

	os_timer_disarm(&get_data_timer);
	os_timer_setfn(&get_data_timer, (os_timer_func_t *)getSensorData, NULL);

	os_timer_arm(&sensor_timer, DELAY, 1);
	os_timer_arm(&get_data_timer, 50000, 1);
}
