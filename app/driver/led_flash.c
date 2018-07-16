#include "ets_sys.h"
#include "eagle_soc.h"
#include "osapi.h"
#include "os_type.h"
#include "gpio.h"
#include "user_interface.h"

#include "driver/led_flash.h"

#define LED_FUNC		FUNC_GPIO12
#define LED_NAME		PERIPHS_IO_MUX_MTDI_U
#define LED_PIN			12
#define FLASH_TIME		600	//ms

LOCAL os_timer_t flashtimer,sc_flashtimer;
LOCAL uint8 fflag, cflag;

void ICACHE_FLASH_ATTR timer_flash_cb(void)
{

	if(fflag==0)
	{
		GPIO_OUTPUT_SET(GPIO_ID_PIN(LED_PIN),1);
		fflag = 1;
	}
	else
	{
		GPIO_OUTPUT_SET(GPIO_ID_PIN(LED_PIN),0);
		fflag = 0;
	}
}

void ICACHE_FLASH_ATTR led_init()
{
	fflag = 0;
	cflag = 1;
	PIN_FUNC_SELECT(LED_NAME,LED_FUNC);
	GPIO_OUTPUT_SET(GPIO_ID_PIN(LED_PIN),0);
//	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U,FUNC_GPIO15);
//	GPIO_OUTPUT_SET(GPIO_ID_PIN(15),0);
	led_close();
}

void ICACHE_FLASH_ATTR led_start_flash()
{
	if(1 == cflag){
		os_timer_disarm(&flashtimer);
		os_timer_setfn(&flashtimer,(os_timer_func_t *)timer_flash_cb,NULL);
		os_timer_arm(&flashtimer,FLASH_TIME,1);
		cflag = 0;
	}
}

void ICACHE_FLASH_ATTR led_stop_flash()
{
	os_timer_disarm(&flashtimer);
	led_close();
	cflag = 1;
}

void ICACHE_FLASH_ATTR scled_start_flash()
{
	if(1 == cflag){
		os_timer_disarm(&sc_flashtimer);
		os_timer_setfn(&sc_flashtimer,(os_timer_func_t *)timer_flash_cb,NULL);
		os_timer_arm(&sc_flashtimer,180,1);
		cflag = 0;
	}
}

void ICACHE_FLASH_ATTR scled_stop_flash()
{
	os_timer_disarm(&sc_flashtimer);
	led_close();
	cflag = 1;
}

void ICACHE_FLASH_ATTR led_open()
{
	GPIO_OUTPUT_SET(GPIO_ID_PIN(LED_PIN),0);
}

void ICACHE_FLASH_ATTR led_close()
{
	GPIO_OUTPUT_SET(GPIO_ID_PIN(LED_PIN),1);
}
