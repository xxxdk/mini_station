#ifndef __UTILS_H__
#define	__UTILS_H__

#include "c_types.h"

uint32_t ICACHE_FLASH_ATTR UTILS_Atoh(const int8_t *s);
uint8_t ICACHE_FLASH_ATTR UTILS_StrToIP(const int8_t* str, void *ip);
uint8_t ICACHE_FLASH_ATTR UTILS_IsIPV4 (int8_t *str);
#endif
