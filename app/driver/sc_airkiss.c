#include "ets_sys.h"
#include "osapi.h"
#include "ip_addr.h"
#include "espconn.h"
#include "mem.h"
#include "user_interface.h"
#include "smartconfig.h"
#include "airkiss.h"
#include "driver/sc_airkiss.h"
#include "driver/sc_airkiss.h"
#include "driver/led_flash.h"

#define DEVICE_TYPE 		"gh_9e2cff3dfa51" 	//wechat public number
#define DEVICE_ID 			"122475" 			//model ID
#define DEFAULT_LAN_PORT 	12476				//微信端口

LOCAL esp_udp ssdp_udp;
LOCAL struct espconn pssdpudpconn;
LOCAL os_timer_t ssdp_time_serv, smcfg_stop_timer;

uint8_t  lan_buf[200];
uint16_t lan_buf_len;
uint8 	 udp_sent_cnt = 0;

const airkiss_config_t akconf =
{
	(airkiss_memset_fn)&memset,
	(airkiss_memcpy_fn)&memcpy,
	(airkiss_memcmp_fn)&memcmp,
	0,
};

void ICACHE_FLASH_ATTR stop_smartconfig()		//停止函数
{
	led_dis_flash();
	os_timer_disarm(&smcfg_stop_timer);
	smartconfig_stop();
	os_timer_disarm(&ssdp_time_serv);		//关闭定时器
	os_printf("smartconfig_stop/time out\n");
}

LOCAL void ICACHE_FLASH_ATTR
airkiss_wifilan_time_callback(void)	//airkiss内网发现回调函数
{
	uint16 i;
	airkiss_lan_ret_t ret;
	
	if ((udp_sent_cnt++) >30) {
		udp_sent_cnt = 0;
		os_timer_disarm(&ssdp_time_serv);
	}

	ssdp_udp.remote_port = DEFAULT_LAN_PORT;
	ssdp_udp.remote_ip[0] = 255;
	ssdp_udp.remote_ip[1] = 255;
	ssdp_udp.remote_ip[2] = 255;
	ssdp_udp.remote_ip[3] = 255;
	lan_buf_len = sizeof(lan_buf);
	ret = airkiss_lan_pack(AIRKISS_LAN_SSDP_NOTIFY_CMD,
		DEVICE_TYPE, DEVICE_ID, 0, 0, lan_buf, &lan_buf_len, &akconf);	//检测函数
	if (ret != AIRKISS_LAN_PAKE_READY) {
		os_printf("Pack lan packet error!");
		return;
	}
	
	ret = espconn_sendto(&pssdpudpconn, lan_buf, lan_buf_len);
	if (ret != 0) {
		os_printf("UDP send error!");
	}
	os_printf("Finish send notify!\n");
}

LOCAL void ICACHE_FLASH_ATTR
airkiss_wifilan_recv_callbk(void *arg, char *pdata, unsigned short len)		//内网发现接收回调函数
{
	uint16 i;
	remot_info* pcon_info = NULL;
		
	airkiss_lan_ret_t ret = airkiss_lan_recv(pdata, len, &akconf);
	airkiss_lan_ret_t packret;
	
	switch (ret){
	case AIRKISS_LAN_SSDP_REQ:
		espconn_get_connection_info(&pssdpudpconn, &pcon_info, 0);
		os_printf("remote ip: %d.%d.%d.%d \r\n",pcon_info->remote_ip[0],pcon_info->remote_ip[1],
			                                    pcon_info->remote_ip[2],pcon_info->remote_ip[3]);
		os_printf("remote port: %d \r\n",pcon_info->remote_port);
      
        pssdpudpconn.proto.udp->remote_port = pcon_info->remote_port;
		os_memcpy(pssdpudpconn.proto.udp->remote_ip,pcon_info->remote_ip,4);
		ssdp_udp.remote_port = DEFAULT_LAN_PORT;
		
		lan_buf_len = sizeof(lan_buf);
		packret = airkiss_lan_pack(AIRKISS_LAN_SSDP_RESP_CMD,
			DEVICE_TYPE, DEVICE_ID, 0, 0, lan_buf, &lan_buf_len, &akconf);
		
		if (packret != AIRKISS_LAN_PAKE_READY) {
			os_printf("Pack lan packet error!");
			return;
		}

		os_printf("\r\n\r\n");
		for (i=0; i<lan_buf_len; i++)
			os_printf("%c",lan_buf[i]);
		os_printf("\r\n\r\n");
		
		packret = espconn_sendto(&pssdpudpconn, lan_buf, lan_buf_len);
		if (packret != 0) {
			os_printf("LAN UDP Send err!");
		}
		
		break;
	default:
		os_printf("Pack is not ssdq req!%d\r\n",ret);
		break;
	}
}

void ICACHE_FLASH_ATTR
airkiss_start_discover(void)		//开始内网发现函数
{
	ssdp_udp.local_port = DEFAULT_LAN_PORT;
	pssdpudpconn.type = ESPCONN_UDP;
	pssdpudpconn.proto.udp = &(ssdp_udp);
	espconn_regist_recvcb(&pssdpudpconn, airkiss_wifilan_recv_callbk);
	espconn_create(&pssdpudpconn);

	os_timer_disarm(&ssdp_time_serv);
	os_timer_setfn(&ssdp_time_serv, (os_timer_func_t *)airkiss_wifilan_time_callback, NULL);
	os_timer_arm(&ssdp_time_serv, 1000, 1);//1s
}

void ICACHE_FLASH_ATTR
smartconfig_done(sc_status status, void *pdata)		//smartconfig状态事件处理函数
{
	switch(status) {
	    case SC_STATUS_WAIT:					//等待
	        os_printf("SC_STATUS_WAIT\n");
	        break;
	    case SC_STATUS_FIND_CHANNEL:			//寻找wifi频段
	        os_printf("SC_STATUS_FIND_CHANNEL\n");
	        break;
	    case SC_STATUS_GETTING_SSID_PSWD:			//获取ssid和password
	        os_printf("SC_STATUS_GETTING_SSID_PSWD\n");
			sc_type *type = pdata;
	        if (*type == SC_TYPE_ESPTOUCH) {		//进入esptouch，或airkiss
	            os_printf("SC_TYPE:SC_TYPE_ESPTOUCH\n");
	        } else {
	            os_printf("SC_TYPE:SC_TYPE_AIRKISS\n");
	        }
	        break;
	    case SC_STATUS_LINK:					//连接wifi
	        os_printf("SC_STATUS_LINK\n");
	        struct station_config *sta_conf = pdata;

	        wifi_station_set_config(sta_conf);
	        wifi_station_disconnect();
	        wifi_station_connect();
	        break;
	    case SC_STATUS_LINK_OVER:				//连接完毕进入内网发现
	        os_printf("SC_STATUS_LINK_OVER\n");
	        if (pdata != NULL) {
				//SC_TYPE_ESPTOUCH
	            uint8 phone_ip[4] = {0};

	            os_memcpy(phone_ip, (uint8*)pdata, 4);
	            os_printf("Phone ip: %d.%d.%d.%d\n",phone_ip[0],phone_ip[1],phone_ip[2],phone_ip[3]);
	        } else {
	        	//SC_TYPE_AIRKISS - support airkiss v2.0
				airkiss_start_discover();
	        }
	        smartconfig_stop();
	        os_printf("smartconfig_stop\n");
	        led_dis_flash();
	        break;
	    }
}

void ICACHE_FLASH_ATTR sc_airkiss_init()			//smartconfig初始化函数
{
	led_flash();				//LED闪烁
	smartconfig_set_type(SC_TYPE_ESPTOUCH_AIRKISS); //设置类型，SC_TYPE_ESPTOUCH,SC_TYPE_AIRKISS,SC_TYPE_ESPTOUCH_AIRKISS
    wifi_set_opmode(STATION_MODE);
    smartconfig_start(smartconfig_done);		//开始smartconfig
    os_timer_disarm(&smcfg_stop_timer);			//启动停止smartconfig函数
    os_timer_setfn(&smcfg_stop_timer,(os_timer_func_t *)stop_smartconfig,NULL);
    os_timer_arm(&smcfg_stop_timer,95000,0);		//9.5秒
}
