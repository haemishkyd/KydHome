typedef struct 
{
	uint8_t identifier;
	char* 	topic_string;
	char 	topic_string_len;
	void    (*Handler)();
}Topic_Type;

enum{
	GATE_LIGHT_TOPIC=0,
	GATE_OPEN_TOPIC,
	FRONT_OUTSIDE_LIGHT_TOPIC,
	NUMBER_TOPICS
};

void MQTT_Parse_Topic(char* topicData, char* dataBuf);