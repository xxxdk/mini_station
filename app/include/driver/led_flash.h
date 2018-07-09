#ifndef __LED_FLASH_H__
#define __LED_FLASH_H__

void ICACHE_FLASH_ATTR led_init();
void ICACHE_FLASH_ATTR led_flash();
void ICACHE_FLASH_ATTR led_dis_flash();
void ICACHE_FLASH_ATTR led_open();
void ICACHE_FLASH_ATTR led_close();

#endif