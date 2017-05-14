#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"
#include "user_interface.h"
#include "uart.h"
#include "mqtt.h"
#include "debug.h"
#include "mem.h"
#include "user_main.h"

/**********************************************************************
                    DEFINES
**********************************************************************/
#define user_procTaskPrio        0
#define user_procTaskQueueLen    1

/**********************************************************************
                    VARIABLES
**********************************************************************/
os_event_t    user_procTaskQueue[user_procTaskQueueLen];
static volatile os_timer_t some_timer;
unsigned char temp_string[50];
MQTT_Client mqttClient;
#if HOME_AUTOMATION_ALARM_INTERFACE == 'Y'
static unsigned char ParadoxMessageComplete = 0;
#endif

/**********************************************************************
                    PROTOTYPES
**********************************************************************/
static void user_procTask(os_event_t *events);
static void connect_wifi();
static void connect_mqtt();
void wifi_handle_event_cb(System_Event_t *evt);

static void ICACHE_FLASH_ATTR mqttConnectedCb(uint32_t *args);
static void ICACHE_FLASH_ATTR mqttDisconnectedCb(uint32_t *args);
static void ICACHE_FLASH_ATTR mqttPublishedCb(uint32_t *args);
static void ICACHE_FLASH_ATTR mqttDataCb(uint32_t *args, const char* topic, uint32_t topic_len, const char *data, uint32_t data_len);

/**********************************************************************
Function:       some_timerfunc
Description:
**********************************************************************/
void ICACHE_FLASH_ATTR some_timerfunc(void *arg)
{
  static uint8_t state_machine = 0;
  static uint16_t event_timer = 0;
  uint8_t ret_val;
#if HOME_AUTOMATION_ALARM_INTERFACE == 'Y'
  uint8_t led_flag;
#endif
  char mqtt_message[30];
  char mqtt_message_length;
  char alive_check = 0;
  uint32_t input_get;
  int16_t temperature_int;

  switch (state_machine)
  {
  case 0:
    event_timer++;
    /* This 100 relates to 2 seconds since this function executes every 20ms */
#if HOME_AUTOMATION_ALARM_INTERFACE == 'Y'
    /* Check the message that comes in from the alarm */
    ParadoxMessageComplete = Paradox_Parse_Incoming_Message();
    if (ParadoxMessageComplete == 1)
    {
      Paradox_Process_Incoming_Message(&mqttClient);
      if (led_flag == 0)
      {
        //Pull the line high
        //gpio_output_set(BIT0, 0, BIT0, 0);
        led_flag = 1;
      }
      else
      {
        //Pull the line low
        //gpio_output_set(0, BIT0, BIT0, 0);
        led_flag = 0;
      }
      ParadoxMessageComplete = 0;
    }
#endif
#if HOME_AUTOMATION_OUTSIDE_TEMP == 'Y'
    /* This 500 relates to 10 seconds since this function executes every 20ms */
    if (event_timer > 500)
    {
      event_timer = 0;
      state_machine = 1;
    }
#else    
    if (event_timer > 100)
    {
      event_timer = 0;
#if HOME_AUTOMATION_GATE == 'Y'
      strcpy(mqtt_message, "{\"state\":\"");
      input_get = GPIO_INPUT_GET(5);
      if (input_get == 0)
      {
        mqtt_message[10] = 'O';
        mqtt_message[11] = 'N';
        mqtt_message[12] = '\"';
        mqtt_message[13] = '}';
        mqtt_message_length = 14;
      }
      else
      {
        mqtt_message[10] = 'O';
        mqtt_message[11] = 'F';
        mqtt_message[12] = 'F';
        mqtt_message[13] = '\"';
        mqtt_message[14] = '}';
        mqtt_message_length = 15;
      }
      MQTT_Publish(&mqttClient, "homeassistant/sensor/gatelight/state", mqtt_message, mqtt_message_length, 0, 0);
#endif
#if HOME_AUTOMATION_FRONT_OUTSIDE_LIGHT == 'Y'
      strcpy(mqtt_message, "{\"state\":\"");
      input_get = GPIO_INPUT_GET(5);
      if (input_get == 1)
      {
        mqtt_message[10] = 'O';
        mqtt_message[11] = 'N';
        mqtt_message[12] = '\"';
        mqtt_message[13] = '}';
        mqtt_message_length = 14;
      }
      else
      {
        mqtt_message[10] = 'O';
        mqtt_message[11] = 'F';
        mqtt_message[12] = 'F';
        mqtt_message[13] = '\"';
        mqtt_message[14] = '}';
        mqtt_message_length = 15;
      }
      MQTT_Publish(&mqttClient, "homeassistant/sensor/frontoutsidelight/state", mqtt_message, mqtt_message_length, 0, 0);
#endif
    }
#endif    
    break;
  case 1:
    ret_val = DS18B20_Read_Temp();
    if (ret_val == 5)
    {
      state_machine = 2;
    }
    break;
  case 2:
    temperature_int = DS18B20_Get_Temp();
    os_printf("Temp %d\n", temperature_int);
    strcpy(mqtt_message, "{\"temp\":");
    esp_itoa(temperature_int, &mqtt_message[8], 10);
    /* We can't measure anything under 0 or over 99 */
    if (temperature_int > 9)
    {
      mqtt_message[10] = '}';
      mqtt_message_length = 11;
    }
    else if (temperature_int < 10)
    {
      mqtt_message[9] = '}';
      mqtt_message_length = 10;
    }    
    MQTT_Publish(&mqttClient, "homeassistant/sensor/frontoutsidetemperature", mqtt_message, mqtt_message_length, 0, 0);
    state_machine = 0;
    break;
  default:
    event_timer = 0;
    state_machine = 0;
    break;
  }
}

char *esp_itoa (int value, char *result, int base)
{
  // check that the base if valid
  if (base < 2 || base > 36) { *result = '\0'; return result; }

  char* ptr = result, *ptr1 = result, tmp_char;
  int tmp_value;

  do {
    tmp_value = value;
    value /= base;
    *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
  } while ( value );

  // Apply negative sign
  if (tmp_value < 0) *ptr++ = '-';
  *ptr-- = '\0';
  while (ptr1 < ptr) {
    tmp_char = *ptr;
    *ptr-- = *ptr1;
    *ptr1++ = tmp_char;
  }
  return result;
}

/**********************************************************************
Function:       user_procTask
Description:
**********************************************************************/
static void ICACHE_FLASH_ATTR user_procTask(os_event_t *events)
{
  os_delay_us(10);
}

/**********************************************************************
Function:       user_init
Description:
**********************************************************************/
void ICACHE_FLASH_ATTR user_init()
{
  // Initialize the GPIO subsystem.
  gpio_init();

#if HOME_AUTOMATION_GATE == 'Y'
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4);
  GPIO_OUTPUT_SET(4, 1);
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U, FUNC_GPIO5);
  GPIO_OUTPUT_SET(5, 1);
#endif
#if HOME_AUTOMATION_OUTSIDE_TEMP == 'Y'
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO4_U, FUNC_GPIO4);
  PIN_PULLUP_EN(PERIPHS_IO_MUX_GPIO4_U);
  GPIO_OUTPUT_SET(4, 1);
#endif
#if HOME_AUTOMATION_FRONT_OUTSIDE_LIGHT == 'Y'
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U, FUNC_GPIO5);
  GPIO_OUTPUT_SET(5, 1);
#endif
#if HOME_AUTOMATION_ALARM_INTERFACE == 'Y'
  Paradox_Init();
#endif


  //connect wifi
  connect_wifi();
  connect_mqtt();

  //Disarm timer
  os_timer_disarm(&some_timer);

  //Setup timer
  os_timer_setfn(&some_timer, (os_timer_func_t *)some_timerfunc, NULL);

  //Arm the timer
  //&some_timer is the pointer
  //500 is the fire time in ms
  //0 for once and 1 for repeating
  os_timer_arm(&some_timer, 20, 1);

  //Start os task
  system_os_task(user_procTask, user_procTaskPrio, user_procTaskQueue, user_procTaskQueueLen);
}

/**********************************************************************
Function:       connect_mqtt
Description:
**********************************************************************/
static void connect_mqtt()
{
  /*MQTT Init */
  MQTT_InitConnection(&mqttClient, MQTT_HOST, MQTT_PORT, DEFAULT_SECURITY);

  if ( !MQTT_InitClient(&mqttClient, MQTT_CLIENT_ID, MQTT_USER, MQTT_PASS, MQTT_KEEPALIVE, MQTT_CLEAN_SESSION) )
  {
    MQTT_INFO("Failed to initialize properly. Check MQTT version.\r\n");
    return;
  }
  //MQTT_InitClient(&mqttClient, "client_id", "user", "pass", 120, 1);
  MQTT_InitLWT(&mqttClient, "/lwt", "offline", 0, 0);
  MQTT_OnConnected(&mqttClient, mqttConnectedCb);
  MQTT_OnDisconnected(&mqttClient, mqttDisconnectedCb);
  MQTT_OnPublished(&mqttClient, mqttPublishedCb);
  MQTT_OnData(&mqttClient, mqttDataCb);
}

/**********************************************************************
Function:       connect_wifi
Description:
**********************************************************************/
static void connect_wifi()
{
  char ssid[32] = SSID;
  char password[64] = SSID_PASSWORD;
  struct station_config stationConf;

#if HOME_AUTOMATION_ALARM_INTERFACE == 'Y'
  uart_init(BIT_RATE_9600, BIT_RATE_9600);
#else
  uart_init(BIT_RATE_115200, BIT_RATE_115200);
#endif
  wifi_set_opmode( STATION_MODE );

  //Set ap settings
  os_memcpy(&stationConf.ssid, ssid, 32);
  os_memcpy(&stationConf.password, password, 64);

  wifi_set_event_handler_cb(wifi_handle_event_cb);
  wifi_station_set_config(&stationConf);
  wifi_station_connect();
  os_sprintf(temp_string, "Pass: %s SSID: %s\r\n", password, ssid);
  uart0_sendStr(temp_string);
}

/**********************************************************************
Function:       wifi_handle_event_cb
Description:
**********************************************************************/
void wifi_handle_event_cb(System_Event_t *evt)
{
  os_printf("event %x\n", evt->event);
  switch (evt->event) {
  case EVENT_STAMODE_CONNECTED:
    os_printf("wifi connect to ssid %s, channel %d\n",
              evt->event_info.connected.ssid,
              evt->event_info.connected.channel);
    break;
  case EVENT_STAMODE_DISCONNECTED:
    os_printf("wifi disconnect\n");
    MQTT_Disconnect(&mqttClient);
    break;
  case EVENT_STAMODE_AUTHMODE_CHANGE:
    os_printf("mode: %d -> %d\n",
              evt->event_info.auth_change.old_mode,
              evt->event_info.auth_change.new_mode);
    break;
  case EVENT_STAMODE_GOT_IP:
    os_printf("ip:" IPSTR ",mask:" IPSTR ",gw:" IPSTR,
              IP2STR(&evt->event_info.got_ip.ip),
              IP2STR(&evt->event_info.got_ip.mask),
              IP2STR(&evt->event_info.got_ip.gw));
    os_printf("\n");
    MQTT_Connect(&mqttClient);
    break;
  case EVENT_SOFTAPMODE_STACONNECTED:
    os_printf("station: " MACSTR "join, AID = %d\n",
              MAC2STR(evt->event_info.sta_connected.mac),
              evt->event_info.sta_connected.aid);
    break;
  case EVENT_SOFTAPMODE_STADISCONNECTED:
    os_printf("station: " MACSTR "leave, AID = %d\n",
              MAC2STR(evt->event_info.sta_disconnected.mac),
              evt->event_info.sta_disconnected.aid);
    break;
  default:
    break;
  }
}

/**********************************************************************
Function:       mqttConnectedCb
Description:
**********************************************************************/
static void ICACHE_FLASH_ATTR mqttConnectedCb(uint32_t *args)
{
  MQTT_Client* client = (MQTT_Client*)args;
  MQTT_INFO("MQTT: Connected\r\n");
#if HOME_AUTOMATION_GATE == 'Y'
  MQTT_Subscribe(client, "homeassistant/switch/gatelight/state", 0);
  MQTT_Subscribe(client, "homeassistant/switch/gateactual/state", 0);
#endif
#if HOME_AUTOMATION_FRONT_OUTSIDE_LIGHT == 'Y'
  MQTT_Subscribe(client, "homeassistant/switch/frontoutsidelight/state", 0);
#endif
}

/**********************************************************************
Function:       mqttDisconnectedCb
Description:
**********************************************************************/
static void ICACHE_FLASH_ATTR mqttDisconnectedCb(uint32_t *args)
{
  MQTT_Client* client = (MQTT_Client*)args;
  MQTT_INFO("MQTT: Disconnected\r\n");
}

/**********************************************************************
Function:       mqttPublishedCb
Description:
**********************************************************************/
static void ICACHE_FLASH_ATTR mqttPublishedCb(uint32_t *args)
{
  MQTT_Client* client = (MQTT_Client*)args;
  MQTT_INFO("MQTT: Published\r\n");
}

/**********************************************************************
Function:       mqttDataCb
Description:
**********************************************************************/
static void ICACHE_FLASH_ATTR mqttDataCb(uint32_t *args, const char* topic, uint32_t topic_len, const char *data, uint32_t data_len)
{
  char *topicBuf = (char*)os_zalloc(topic_len + 1),
        *dataBuf = (char*)os_zalloc(data_len + 1);

  MQTT_Client* client = (MQTT_Client*)args;
  os_memcpy(topicBuf, topic, topic_len);
  topicBuf[topic_len] = 0;
  os_memcpy(dataBuf, data, data_len);
  dataBuf[data_len] = 0;
  MQTT_INFO("Receive topic: %s, data: %s \r\n", topicBuf, dataBuf);
  MQTT_Parse_Topic(topicBuf, dataBuf);
  os_free(topicBuf);
  os_free(dataBuf);
}