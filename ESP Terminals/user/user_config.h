#define SSID "KYDHOME"
#define SSID_PASSWORD "skattiepoo"

#define MQTT_SSL_ENABLE

/*DEFAULT CONFIGURATIONS*/

#define MQTT_HOST     	"192.168.1.54" //or "mqtt.yourdomain.com"
#define MQTT_PORT     	1883
#define MQTT_BUF_SIZE   1024
#define MQTT_KEEPALIVE  120  /*second*/

#define MQTT_USER     "USER"
#define MQTT_PASS     "PASS"
#define MQTT_CLEAN_SESSION 1
#define MQTT_KEEPALIVE 120

#define DEFAULT_SECURITY	0

#define MQTT_RECONNECT_TIMEOUT  5 /*second*/

#define PROTOCOL_NAMEv31  /*MQTT version 3.1 compatible with Mosquitto v0.15*/

#define HOME_AUTOMATION_GATE					'Y'
#define HOME_AUTOMATION_OUTSIDE_TEMP			'N'
#define HOME_AUTOMATION_FRONT_OUTSIDE_LIGHT		'N'
#define HOME_AUTOMATION_ALARM_INTERFACE			'N'

#if HOME_AUTOMATION_GATE == 'Y'
	#define MQTT_CLIENT_ID    "MAIN GATE CONTROL"
#endif
#if HOME_AUTOMATION_OUTSIDE_TEMP == 'Y'
	#define MQTT_CLIENT_ID    "FRONT OUTSIDE TEMPERATURE"
#endif
#if HOME_AUTOMATION_FRONT_OUTSIDE_LIGHT == 'Y'
	#define MQTT_CLIENT_ID    "FRONT OUTSIDE LIGHT"
#endif
#if HOME_AUTOMATION_ALARM_INTERFACE == 'Y'
	#define MQTT_CLIENT_ID    "ALARM INTERFACE"
#endif