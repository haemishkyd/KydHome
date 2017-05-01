#include "user_interface.h"
#include "osapi.h"
#include "espconn.h"
#include "os_type.h"
#include "mem.h"
#include "mqtt_msg.h"
#include "debug.h"
#include "user_config.h"
#include "mqtt.h"
#include "mqtt_parse.h"
#include "queue.h"

/**********************************************************************
                    DEFINES
**********************************************************************/

/**********************************************************************
                    PROTOTYPES
**********************************************************************/
void run_Outside_Light_Action(char* dataBuf);
void run_Gate_Actual_Action(char* dataBuf);
void ICACHE_FLASH_ATTR run_Front_Outside_Light_Action(char* dataBuf);

/**********************************************************************
                    VARIABLES
**********************************************************************/
Topic_Type TheTopicList[NUMBER_TOPICS]=
{
	{GATE_LIGHT_TOPIC,		    "homeassistant/switch/gatelight/state",			36, (void*)&run_Outside_Light_Action			},
	{GATE_OPEN_TOPIC,			"homeassistant/switch/gateactual/state",		37, (void*)&run_Gate_Actual_Action				},
	{FRONT_OUTSIDE_LIGHT_TOPIC,	"homeassistant/switch/frontoutsidelight/state",	44, (void*)&run_Front_Outside_Light_Action		},
};

/**********************************************************************
Function:       MQTT_Parse_Topic
Description:    
**********************************************************************/
void ICACHE_FLASH_ATTR MQTT_Parse_Topic(char* topicData, char* dataBuf)
{
	uint8_t topic_idx,letter_idx;
	uint8_t this_is_a_match = 1;

	for (topic_idx = 0; topic_idx < NUMBER_TOPICS; topic_idx++)
	{
		this_is_a_match = 1;
		for (letter_idx = 0; letter_idx < TheTopicList[topic_idx].topic_string_len; letter_idx++)
		{
			if (topicData[letter_idx] != TheTopicList[topic_idx].topic_string[letter_idx])
			{				
				this_is_a_match = 0;
				break;
			}
		}
		if (this_is_a_match == 1)
		{			
			(*(TheTopicList[topic_idx].Handler))(dataBuf);
			break;
		}
	}
}

/**********************************************************************
Function:       run_Outside_Light_Action
Description:    
**********************************************************************/
void ICACHE_FLASH_ATTR run_Outside_Light_Action(char* dataBuf)
{
  if ((dataBuf[0] == 'O')&&(dataBuf[1] == 'F')&&(dataBuf[2] == 'F'))
  {
    GPIO_OUTPUT_SET(5,1);
    os_printf("Pin 5 High\n\r");
  }
  if ((dataBuf[0] == 'O')&&(dataBuf[1] == 'N'))
  {
    GPIO_OUTPUT_SET(5,0);
    os_printf("Pin 5 Low\n\r");
  }
}

/**********************************************************************
Function:       run_Gate_Actual_Action
Description:    
**********************************************************************/
void ICACHE_FLASH_ATTR run_Gate_Actual_Action(char* dataBuf)
{
  if ((dataBuf[0] == 'O')&&(dataBuf[1] == 'F')&&(dataBuf[2] == 'F'))
  {
    GPIO_OUTPUT_SET(4,1);
    os_printf("Pin 4 High\n\r");
  }
  if ((dataBuf[0] == 'O')&&(dataBuf[1] == 'N'))
  {
    GPIO_OUTPUT_SET(4,0);
    os_printf("Pin 4 Low\n\r");
  }
}

/**********************************************************************
Function:       run_Front_Outside_Light_Action
Description:    
**********************************************************************/
void ICACHE_FLASH_ATTR run_Front_Outside_Light_Action(char* dataBuf)
{
  if ((dataBuf[0] == 'O')&&(dataBuf[1] == 'F')&&(dataBuf[2] == 'F'))
  {
    GPIO_OUTPUT_SET(5,0);
    os_printf("Pin 5 Low\n\r");
  }
  if ((dataBuf[0] == 'O')&&(dataBuf[1] == 'N'))
  {
    GPIO_OUTPUT_SET(5,1);
    os_printf("Pin 5 High\n\r");
  }
}