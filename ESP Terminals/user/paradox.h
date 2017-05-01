#define ARMED     0x01
#define DISARMED  0x02
#define TRIGGERED 0x04
#define SILENT    0x08

typedef struct{
  char AlarmZone;
  char AlarmStatus;
}AlarmDataType;

#define RING_BUFFER_SIZE 255

typedef struct
{
	unsigned char Head;
	unsigned char Tail;
	unsigned char Buffer[RING_BUFFER_SIZE];
}MyRingBufferType;

void Paradox_Init();
void Paradox_Add_New_Byte(char *new_byte_array, unsigned short len);
AlarmDataType *Paradox_Get_Alarm_Data(void);
uint8_t Paradox_Parse_Incoming_Message();
void Paradox_Process_Incoming_Message(MQTT_Client *client);
