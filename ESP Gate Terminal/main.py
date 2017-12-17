import machine
import time
#import webrepl_setup
from umqtt.simple import MQTTClient

CONFIG = {
    "broker": "192.168.0.104",
    "client_id": "GATE TERMINAL",
    "p1_topic": "homeassistant/sensor/gatelight/state",
    "p2_topic": "homeassistant/sensor/gateactual/state",
    "s1_topic": "homeassistant/switch/gatelight/state",
    "s2_topic": "homeassistant/switch/gateactual/state"
}

def subscription_callback(topic,msg):
    global gate_open_cmd_pin
    global gate_light_pin    
    global gate_light_state
    if topic == CONFIG['s1_topic'].encode('utf8'):
        if msg == b"ON":
            print ("Gate Light On")
            gate_light_state = 1
            gate_light_pin.value(0)
        if msg == b"OFF":
            print ("Gate Light Off")
            gate_light_state = 0
            gate_light_pin.value(1)
    if topic == CONFIG['s2_topic'].encode('utf8'):
        if msg == b"ON":
            print ("Gate Inactive")
            gate_open_cmd_pin.value(0)
        if msg == b"OFF":
            print ("Gate Active")
            gate_open_cmd_pin.value(1)

def do_mqtt_connect(l_client):
    try:        
        l_client.connect()
        print("Connected to {}".format(CONFIG['broker']))
        l_client.subscribe(CONFIG['s1_topic'])
        l_client.subscribe(CONFIG['s2_topic'])
    except OSError as e:
        print ("Could not connect!!")
    
client = MQTTClient(CONFIG['client_id'], CONFIG['broker'])
client.set_callback(subscription_callback)
do_mqtt_connect(client)
gate_light_state = 0
gate_open_state = 0
# set up the gate command pin and initialise the RELAY to off
gate_open_cmd_pin = machine.Pin(4, machine.Pin.OUT)
gate_open_cmd_pin.value(1)
# set up the gate light pin and initialise the RELAY to off
gate_light_pin = machine.Pin(5, machine.Pin.OUT)
gate_light_pin.value(1)
# the gate reed switch is shorted when closed. The pullups are required to make it high
# when the gate is open
gate_open_in_pin = machine.Pin(13, machine.Pin.IN, machine.Pin.PULL_UP)
time.sleep(5)

while True:   
    try:
        print("Send P1")
        if (gate_light_state == 1):
            client.publish(CONFIG['p1_topic'], "{\"state\":\"ON\"}")
        else:
            client.publish(CONFIG['p1_topic'], "{\"state\":\"OFF\"}")
        time.sleep(1)
        print("Send P2")
        if (gate_open_in_pin.value() == 1):
            client.publish(CONFIG['p2_topic'], "{\"state\":\"OFF\"}")
        else:
            client.publish(CONFIG['p2_topic'], "{\"state\":\"ON\"}")
        time.sleep(1)
        print("Check Subs")
        client.check_msg()
        time.sleep(1)
    except OSError as e:
        do_mqtt_connect(client)
        time.sleep(5)
    

machine.reset()
