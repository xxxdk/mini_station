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
#include "driver/i2c.h"
#include "driver/get_sensor_data.h"

void ICACHE_FLASH_ATTR all_init_cb();

void ICACHE_FLASH_ATTR all_init_cb()		          //初始化回调函数
{
	os_printf("SDK version:%s\n", system_get_sdk_version());
	os_printf("MS version: 0.2.2\n");
	i2c_init();
    key_timer_init();
    os_delay_us(500);
    sensor_start();
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
