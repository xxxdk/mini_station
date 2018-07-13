/*
 * debug.h
 *
 *  Created on: Dec 4, 2014
 *      Author: Minh
 */

#ifndef __DEBUG_H__
#define __DEBUG_H__

#define GLOBAL_DEBUG_ON
#if defined(GLOBAL_DEBUG_ON)
#define MQTT_DEBUG_ON
#endif
#if defined(MQTT_DEBUG_ON)
#define INFO( format, ... ) os_printf( format, ## __VA_ARGS__ )
#else
#define INFO( format, ... )
#endif
// #ifndef INFO
// #define INFO os_printf
// #endif

#endif /* USER_DEBUG_H_ */
