#include "ets_sys.h"
#include "eagle_soc.h"
#include "osapi.h"
#include "os_type.h"
#include "gpio.h"
#include "user_interface.h"

#include "driver/mykey.h"
#include "driver/sc_airkiss.h"
#include "driver/led_flash.h"

#define KEY_FUNC		FUNC_GPIO13
#define KEY_NAME		PERIPHS_IO_MUX_MTCK_U
#define KEY_PIN			13

#define WAIT_TIME		2000

os_timer_t intr_timer;

void ICACHE_FLASH_ATTR timer_intr_cb(void)
{
	os_timer_disarm(&intr_timer);
	if (GPIO_INPUT_GET(GPIO_ID_PIN(KEY_PIN)) == 0)
	{
		uart0_sendStr("timer_intr_cb\r\n");
		gpio_pin_intr_state_set(GPIO_ID_PIN(KEY_PIN),GPIO_PIN_INTR_NEGEDGE);
		sc_airkiss_init();/*************************Æô¶¯SC_Airkiss**************************************/
	}
	else
	{
		led_open();
		gpio_pin_intr_state_set(GPIO_ID_PIN(KEY_PIN),GPIO_PIN_INTR_NEGEDGE);	
	}
}

LOCAL void key_intr(void)
{	
	uart0_sendStr("GPIO13_Intr\r\n");
	uint32 gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
	if(gpio_status & BIT(KEY_PIN)){
		gpio_pin_intr_state_set(GPIO_ID_PIN(KEY_PIN),GPIO_PIN_INTR_DISABLE);
		GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS,gpio_status & BIT(KEY_PIN));
		os_timer_disarm(&intr_timer);
		os_timer_setfn(&intr_timer,(os_timer_func_t *)timer_intr_cb,NULL);
		os_timer_arm(&intr_timer,WAIT_TIME,0);
	}
}

void ICACHE_FLASH_ATTR key_timer_init()
{
	led_init();
	ETS_GPIO_INTR_ATTACH(key_intr,NULL);
	ETS_GPIO_INTR_DISABLE();
	PIN_FUNC_SELECT(KEY_NAME,KEY_FUNC);
	GPIO_DIS_OUTPUT(GPIO_ID_PIN(KEY_PIN));
	gpio_register_set(GPIO_PIN_ADDR(KEY_PIN),GPIO_PIN_INT_TYPE_SET(GPIO_PIN_INTR_DISABLE)
		|GPIO_PIN_PAD_DRIVER_SET(GPIO_PAD_DRIVER_DISABLE)
		|GPIO_PIN_SOURCE_SET(GPIO_AS_PIN_SOURCE));
	GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS,BIT(KEY_PIN));
	gpio_pin_intr_state_set(GPIO_ID_PIN(KEY_PIN),GPIO_PIN_INTR_NEGEDGE);
	ETS_GPIO_INTR_ENABLE();
}
