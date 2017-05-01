#include "mem.h"
#include "c_types.h"
#include "user_interface.h"
#include "ets_sys.h"
#include "uart.h"
#include "osapi.h"
#include "espconn.h"
#include "gpio.h"
#include "mqtt.h"
#include "paradox.h"
#include "user_main.h"

static MyRingBufferType AlarmRingBuffer;
static AlarmDataType AlarmData;
static unsigned char FromParadoxBuffer[255];
static unsigned char ResponseIndex = 0;

/********************************************************************
 *          Paradox_Init
 *******************************************************************/
void Paradox_Init()
{
  char idx;
  AlarmRingBuffer.Head = 0;
  AlarmRingBuffer.Tail = 0;
  AlarmData.AlarmZone = 0;
  AlarmData.AlarmStatus = 0;
}

/********************************************************************
 *          Paradox_Get_Alarm_Data
 *******************************************************************/
AlarmDataType *Paradox_Get_Alarm_Data(void)
{
  return &AlarmData;
}

/********************************************************************
 *          Paradox_Add_New_Byte
 *******************************************************************/
void Paradox_Add_New_Byte(char *new_byte_array, unsigned short len)
{
  unsigned char index;

  for (index = 0; index < len; index++)
  {
    AlarmRingBuffer.Buffer[AlarmRingBuffer.Tail] = new_byte_array[index];
    AlarmRingBuffer.Tail++;
    if (AlarmRingBuffer.Tail >= RING_BUFFER_SIZE)
    {
      AlarmRingBuffer.Tail = 0;
    }
  }
}

/********************************************************************
 *          Paradox_Parse_Incoming_Message
 *******************************************************************/
uint8_t Paradox_Parse_Incoming_Message()
{
  unsigned char currentByte;
  static unsigned char currentIndex;
  static unsigned char CurrentMessageState = 0;
  static unsigned char length_of_message;
  uint8_t message_complete = 0;

  while ((AlarmRingBuffer.Head != AlarmRingBuffer.Tail) && (message_complete == 0))
  {
    currentByte = AlarmRingBuffer.Buffer[AlarmRingBuffer.Head];
    FromParadoxBuffer[ResponseIndex++] = currentByte;
    switch (CurrentMessageState)
    {
    case 0:
      if ((currentByte & 0xF0) == 0xE0)
      {
        CurrentMessageState = 1;
        length_of_message = 37;
      }
      else
      {
        ResponseIndex = 0;
      }
      break;
    case 1:
      currentIndex++;
      if (currentIndex >= (length_of_message - 1))
      {
        message_complete = 1;
        currentIndex = 0;
        CurrentMessageState = 0;
      }
      break;
    }
    AlarmRingBuffer.Head++;
    if (AlarmRingBuffer.Head >= 255)
    {
      AlarmRingBuffer.Head = 0;
    }
  }

  return message_complete;
}

/********************************************************************
 *          Paradox_Process_Incoming_Message
 *******************************************************************/
void Paradox_Process_Incoming_Message(MQTT_Client *client)
{
  char mqtt_message[30];
  char mqtt_topic[50];
  char mqtt_message_length;

  if ((FromParadoxBuffer[7] == 0) ||
      (FromParadoxBuffer[7] == 1))
  {
    uint8_t temp_val = (1 << (FromParadoxBuffer[8] - 1));
    strcpy(mqtt_topic, "homeassistant/sensor/alarm/zone/");
    mqtt_topic[32] = FromParadoxBuffer[8]+0x30;
    mqtt_topic[33] = '\0';
    if (FromParadoxBuffer[7] == 1)
    {
      strcpy(mqtt_message, "{\"state\":1}");
      mqtt_message_length = 11;
      AlarmData.AlarmZone |= temp_val;
    }
    else
    {
      strcpy(mqtt_message, "{\"state\":0}");
      mqtt_message_length = 11;
      AlarmData.AlarmZone &= ~temp_val;
    }
    MQTT_Publish(client, mqtt_topic, mqtt_message, mqtt_message_length, 0, 0);
  }
  if ((FromParadoxBuffer[7] == 2) && (FromParadoxBuffer[8] == 11))
  {
    //reset all the bits in the alarm if we disarm then only
    //mark the disarm one
    AlarmData.AlarmStatus = 0;
    AlarmData.AlarmStatus |= DISARMED;
    AlarmData.AlarmStatus &= ~ARMED;
    strcpy(mqtt_message, "disarmed");    
    mqtt_message_length = 8;
    MQTT_Publish(client, "homeassistant/sensor/alarm/armed", mqtt_message, mqtt_message_length, 0, 0);
  }
  if ((FromParadoxBuffer[7] == 2) && (FromParadoxBuffer[8] == 12))
  {
    AlarmData.AlarmStatus |= ARMED;
    AlarmData.AlarmStatus &= ~DISARMED;
    strcpy(mqtt_message, "armed_away");    
    mqtt_message_length = 10;
    MQTT_Publish(client, "homeassistant/sensor/alarm/armed", mqtt_message, mqtt_message_length, 0, 0);
  }
  if ((FromParadoxBuffer[7] == 2) && (FromParadoxBuffer[8] == 4))
  {
    AlarmData.AlarmStatus |= TRIGGERED;
    AlarmData.AlarmStatus &= ~SILENT;
  }
  if ((FromParadoxBuffer[7] == 2) && (FromParadoxBuffer[8] == 2))
  {
    AlarmData.AlarmStatus |= SILENT;
    AlarmData.AlarmStatus &= ~TRIGGERED;
  }
  if (FromParadoxBuffer[7] == 36)
  {
    AlarmData.AlarmStatus &= 0x0F;
    AlarmData.AlarmStatus |= (FromParadoxBuffer[8] << 4);
  }
  ResponseIndex = 0;
}
