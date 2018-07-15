#ifndef __MQTT_CONFIG_H__
#define __MQTT_CONFIG_H__

typedef enum{
  NO_TLS = 0,                       // 0: disable SSL/TLS, there must be no certificate verify between MQTT server and ESP8266
  TLS_WITHOUT_AUTHENTICATION = 1,   // 1: enable SSL/TLS, but there is no a certificate verify
  ONE_WAY_ANTHENTICATION = 2,       // 2: enable SSL/TLS, ESP8266 would verify the SSL server certificate at the same time
  TWO_WAY_ANTHENTICATION = 3,       // 3: enable SSL/TLS, ESP8266 would verify the SSL server certificate and SSL server would verify ESP8266 certificate
}TLS_LEVEL;

#define CA_CERT_FLASH_ADDRESS		0x77              // CA certificate address in flash to read, 0x77 means address 0x77000
#define CLIENT_CERT_FLASH_ADDRESS	0x78

/*DEFAULT CONFIGURATIONS*/

#define MQTT_HOST				"sni.mqtt.iot.gz.baidubce.com" // the IP address or domain name of your MQTT server or MQTT broker ,such as "mqtt.yourdomain.com"
#define MQTT_PORT				1883    // the listening port of your MQTT server or MQTT broker
#define MQTT_CLIENT_ID			"dev"    // the ID of yourself, any string is OK,client would use this ID register itself to MQTT server
#define MQTT_USER				"sni/dev" // your MQTT login name, if MQTT server allow anonymous login,any string is OK, otherwise, please input valid login name which you had registered
#define MQTT_PASS				"XXflnvr14kU7Ywj6qbFx+btUzjkaYQTg/hey46XlTO4=" // you MQTT login password, same as above
#define STA_SSID				"k"//"IDART_M204_2.4G"    // your AP/router SSID to config your device networking
#define STA_PASS				"12345678qw"//"mima111111" // your AP/router password

#define DEFAULT_SECURITY    		NO_TLS      // very important: you must config DEFAULT_SECURITY for SSL/TLS

#define MQTT_BUF_SIZE        		1024
#define MQTT_KEEPALIVE        		60     /*second*/
#define MQTT_RECONNECT_TIMEOUT		5    /*second*/
//#define MQTT_SSL_ENABLE   			//* Please don't change or if you know what you doing */

#define STA_TYPE AUTH_WPA2_PSK
#define QUEUE_BUFFER_SIZE			2048

//#define PROTOCOL_NAMEv31		/*MQTT version 3.1 compatible with Mosquitto v0.15*/  //v31
#define PROTOCOL_NAMEv311		/*MQTT version 3.11 compatible with https://eclipse.org/paho/clients/testing/*/

#endif // __MQTT_CONFIG_H__
