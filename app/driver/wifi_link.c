/*
 * wifi.c
 *
 *  Created on: Dec 30, 2014
 *      Author: Minh
 */
#include "user_interface.h"
#include "osapi.h"
#include "espconn.h"
#include "os_type.h"
#include "mem.h"
#include "sntp.h"

#include "user_config.h"
#include "mqtt_config.h"

#include "mqtt/debug.h"
#include "driver/mymqtt.h"
#include "driver/wifi_link.h"
#include "driver/led_flash.h"

LOCAL os_timer_t check_connect_timer;
static uint8 count;

//void ICACHE_FLASH_ATTR getTimeStamp()
//{
//	Unix_timestamp = sntp_get_current_timestamp() - 28800;
//	INFO("current time : %lu, %s\r\n", Unix_timestamp, sntp_get_real_time(Unix_timestamp));
//}

//void ICACHE_FLASH_ATTR sntp_cb()
//{
//	Unix_timestamp = sntp_get_current_timestamp() - 28800;
//		INFO("current time : %lu, %s\r\n", Unix_timestamp, sntp_get_real_time(Unix_timestamp));
//	getTimeStamp();
//    if (Unix_timestamp == 0) {
//    	INFO("did not get a valid time from sntp server\n");
//    } else {
//		os_timer_disarm(&sntp_timer);
////		mqttInit();
//    }
//}
void ICACHE_FLASH_ATTR check_timer_close()
{
	os_timer_disarm(&check_connect_timer);
}

void ICACHE_FLASH_ATTR check_connect_cb()	//连接检测回调函数
{
    uint8_t status;
    struct ip_addr udpip;
    struct ip_info ipconfig;
    struct station_config stationConf;

    INFO("check_connect_cb\r\n");
    status = wifi_station_get_connect_status(); //模块状态获取
    if(status == STATION_GOT_IP){				//判断是否获得IP
        INFO("wifi connect success!\r\n");
        os_timer_disarm(&check_connect_timer);
        if(0 != wifi_get_ip_info(STATION_IF, &ipconfig)){	//获取IP
        	INFO("----------------\r\n");
        	sntp_stop();
        	if( true == sntp_set_timezone(8) ) {
            	sntp_setservername(0, "ntp1.aliyun.com");        // set sntp server after got ip address
                sntp_setservername(1, "ntp2.aliyun.com");
                sntp_setservername(2, "ntp3.aliyun.com");
                sntp_init();
        	}
        	mqttInit();
        	led_stop_flash();
//            os_timer_arm(&sntp_timer,5000,1);
        }
    }else{
        count++;
        if(count == 5){
            os_timer_disarm(&check_connect_timer);	//定时检测
            count = 0;
            INFO("wifi connect fail!\r\n");
        }
    }
}

void ICACHE_FLASH_ATTR user_set_station_config()      //连接的wifi参数的设置函数
{
	INFO("user_set_station_config\r\n");
	led_start_flash();
//    struct station_config stationConf;
//    stationConf.bssid_set = 0;
//    os_memcpy(&stationConf.ssid, STA_SSID, 32);
//    os_memcpy(&stationConf.password, STA_PASS, 64);
//    wifi_station_set_config(&stationConf);				//设置参数函数
    wifi_station_connect();								//连接AP

    os_timer_disarm(&check_connect_timer);
	os_timer_setfn(&check_connect_timer,(os_timer_func_t *)check_connect_cb,NULL);
//	os_timer_disarm(&sntp_timer);
//	os_timer_setfn(&sntp_timer,(os_timer_func_t *)sntp_cb,NULL);

	os_timer_arm(&check_connect_timer,5000,1);
}
