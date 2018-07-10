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
#include "os_type.h"
#include "user_interface.h"

#include "driver/dht22.h"
#include "driver/bh1750.h"
#include "driver/gpio_append.h"

#define DELAY 5000

LOCAL os_timer_t sensor_timer;

DHT_Sensor sensor;

LOCAL void ICACHE_FLASH_ATTR dht22_cb()
{
	static uint8_t i;
	DHT_Sensor_Data data;
	uint8_t pin;

	if (DHTRead(&sensor, &data))
	{
	    char buff[20];
	    os_printf("\r\nTemp: %s  ", DHTFloat2String(buff, data.temperature));
	    os_printf("Hum: %s\r\n", DHTFloat2String(buff, data.humidity));
	} else {
	    os_printf("Failed to read temperature and humidity sensor on GPIO%d\n", pin);
	}
}

LOCAL void ICACHE_FLASH_ATTR dht22_start()
{
	sensor.pin = 5;
	sensor.type = DHT22;
	DHTInit(&sensor);
}

LOCAL void ICACHE_FLASH_ATTR bh1750_cb()
{
	int data;
    data = GetLight();
    if(data == -1)
    	os_printf("Error get light value\r\n");
    else
    	os_printf("\r\nlight: %d\r\n", data);

}

LOCAL void ICACHE_FLASH_ATTR bh1750_start()
{
	BH1750Init();
}

LOCAL void ICACHE_FLASH_ATTR sensor_cb(void *arg)
{
	dht22_cb();
	bh1750_cb();
}

void ICACHE_FLASH_ATTR sensor_start()
{
	dht22_start();
	bh1750_start();

	os_timer_disarm(&sensor_timer);
	os_timer_setfn(&sensor_timer, (os_timer_func_t *)sensor_cb, NULL);
	os_timer_arm(&sensor_timer, DELAY, 1);
}
