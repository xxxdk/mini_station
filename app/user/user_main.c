/******************************************************************************
 * Copyright 2013-2014 Espressif Systems (Wuxi)
 *
 * FileName: user_main.c
 *
 * Description: entry file of user application
 *
 * Modification history:
 *     2014/1/1, v1.0 create this file.
*******************************************************************************/
#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"
#include "driver/uart.h"
#include "user_config.h"

#define SSID        "k"						 //wifi的SSID
#define PASSWORD    "123456qw"				 //wifi的password

void ICACHE_FLASH_ATTR all_init_cb();
void ICACHE_FLASH_ATTR user_set_station_config();

void ICACHE_FLASH_ATTR all_init_cb()		          //初始化回调函数
{
	os_printf("SDK version:%s\n", system_get_sdk_version());
    //user_set_station_config();
    key_timer_init();
}

void ICACHE_FLASH_ATTR user_set_station_config()      //连接的wifi参数的设置函数
{
    struct station_config stationConf;
    stationConf.bssid_set = 0;
    os_memcpy(&stationConf.ssid, SSID, 32);
    os_memcpy(&stationConf.password, PASSWORD, 64);
    wifi_station_set_config(&stationConf);				//设置参数函数
    wifi_station_connect();								//连接AP
}

uint32 ICACHE_FLASH_ATTR
user_rf_cal_sector_set(void)		   //flash保护函数
{
    enum flash_size_map size_map = system_get_flash_size_map();
    uint32 rf_cal_sec = 0;

    switch (size_map) {
        case FLASH_SIZE_4M_MAP_256_256:
            rf_cal_sec = 128 - 5;
            break;

        case FLASH_SIZE_8M_MAP_512_512:
            rf_cal_sec = 256 - 5;
            break;

        case FLASH_SIZE_16M_MAP_512_512:
        case FLASH_SIZE_16M_MAP_1024_1024:
            rf_cal_sec = 512 - 5;
            break;

        case FLASH_SIZE_32M_MAP_512_512:
        case FLASH_SIZE_32M_MAP_1024_1024:
            rf_cal_sec = 1024 - 5;
            break;

        default:
            rf_cal_sec = 0;
            break;
    }

    return rf_cal_sec;
}

void user_init(void)
{
	uart_init(BIT_RATE_115200, BIT_RATE_115200);		//设置串口波特率
    wifi_set_opmode(STATION_MODE);					//设置为STATION_MODE模式
    system_init_done_cb(all_init_cb);   		//全局初始化完成回调函数
}
