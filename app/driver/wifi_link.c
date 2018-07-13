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


LOCAL os_timer_t check_connect_timer,sntp_timer;
static uint8 count;

void ICACHE_FLASH_ATTR sntp_cb()
{
    unsigned long ts = 0;
    ts = sntp_get_current_timestamp();
    INFO("current time : %s\n", sntp_get_real_time(ts));
    if (ts == 0) {
    	INFO("did not get a valid time from sntp server\n");
    } else {
		os_timer_disarm(&sntp_timer);
		mqttInit();
    }
}

void ICACHE_FLASH_ATTR check_connect_cb()	//���Ӽ��ص�����
{
    uint8_t status;
    struct ip_addr udpip;
    struct ip_info ipconfig;
    struct station_config stationConf;

    INFO("check_connect_cb\r\n");
    status = wifi_station_get_connect_status(); //ģ��״̬��ȡ
    if(status == STATION_GOT_IP){				//�ж��Ƿ���IP
        INFO("wifi connect success!\r\n");
        os_timer_disarm(&check_connect_timer);
        if(wifi_get_ip_info(STATION_IF, &ipconfig)){	//��ȡIP
        	INFO("----------------\r\n");
        	sntp_setservername(0, "0.cn.pool.ntp.org");        // set sntp server after got ip address
            sntp_setservername(0, "1.cn.pool.ntp.org");
            sntp_init();
            os_timer_arm(&sntp_timer,5000,1);
        }
    }else{
        count++;
        if(count == 5){
            os_timer_disarm(&check_connect_timer);	//��ʱ���
            count = 0;
            INFO("wifi connect fail!\r\n");
        }
    }
}

void ICACHE_FLASH_ATTR user_set_station_config()      //���ӵ�wifi���������ú���
{
	INFO("user_set_station_config\r\n");

    struct station_config stationConf;
    stationConf.bssid_set = 0;
    os_memcpy(&stationConf.ssid, STA_SSID, 32);
    os_memcpy(&stationConf.password, STA_PASS, 64);
    wifi_station_set_config(&stationConf);				//���ò�������
    wifi_station_connect();								//����AP

    os_timer_disarm(&check_connect_timer);
	os_timer_setfn(&check_connect_timer,(os_timer_func_t *)check_connect_cb,NULL);
	os_timer_disarm(&sntp_timer);
	os_timer_setfn(&sntp_timer,(os_timer_func_t *)sntp_cb,NULL);

	os_timer_arm(&check_connect_timer,5000,1);
}
