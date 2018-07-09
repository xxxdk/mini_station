#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__

#define FUNC			FUNC_GPIO12					//GPIO12管脚的功能名
#define PIN_NAME		PERIPHS_IO_MUX_MTDI_U		//GPIO12管脚的管脚名
#define GPIO_NO			GPIO_ID_PIN(12)				//GPIO12管脚的管脚号，需要启用其他管脚只需修改括号中数字即可
#define BIT_VALUE		0 							//置1则LED灯点亮，置0则LED灯熄灭

#define KEY_PIN			13
#define KEY_FUNC		FUNC_GPIO13
#define KEY_NAME		PERIPHS_IO_MUX_MTCK_U

#endif