#ifndef __LED_FLASH_H__
#define __LED_FLASH_H__

void ICACHE_FLASH_ATTR led_init();
void ICACHE_FLASH_ATTR led_start_flash();
void ICACHE_FLASH_ATTR led_stop_flash();
void ICACHE_FLASH_ATTR led_open();
void ICACHE_FLASH_ATTR led_close();

void ICACHE_FLASH_ATTR scled_start_flash();
void ICACHE_FLASH_ATTR scled_stop_flash();

#endif
