import machine
import time
#import webrepl_setup
from umqtt.simple import MQTTClient

CONFIG = {
    "broker": "192.168.0.104",
    "client_id": "FRONT OUTSIDE LIGHT",
    "p1_topic": "homeassistant/sensor/frontoutsidelight/state",
    "s1_topic": "homeassistant/switch/frontoutsidelight/state",
}

def subscription_callback(topic,msg):
    global outside_light_state
    global light_cmd_pin
    if topic == CONFIG['s1_topic'].encode('utf8'):
        if msg == b"ON":
            print ("Outside Light On")
            outside_light_state = 1
            light_cmd_pin.value(1)
        if msg == b"OFF":
            print("Outside Light Off")
            outside_light_state = 0
            light_cmd_pin.value(0)

def do_mqtt_connect(l_client):
    try:        
        l_client.connect()
        print("Connected to {}".format(CONFIG['broker']))
        l_client.subscribe(CONFIG['s1_topic'])
    except OSError as e:
        print ("Could not connect!!")
    
client = MQTTClient(CONFIG['client_id'], CONFIG['broker'])
client.set_callback(subscription_callback)
do_mqtt_connect(client)
outside_light_state = 0
# set up the outside light pin and initialise the RELAY to off
light_cmd_pin = machine.Pin(5, machine.Pin.OUT)
light_cmd_pin.value(0)
time.sleep(5)

while True:   
    try:
        print("Send P1")
        if (outside_light_state == 1):
            client.publish(CONFIG['p1_topic'], "{\"state\":\"ON\"}")
        else:
            client.publish(CONFIG['p1_topic'], "{\"state\":\"OFF\"}")
        time.sleep(1)        
        print("Check Subs")
        client.check_msg()
        time.sleep(1)
    except OSError as e:
        do_mqtt_connect(client)
        time.sleep(5)
    

machine.reset()
