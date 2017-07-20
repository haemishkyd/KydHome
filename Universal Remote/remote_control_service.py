#!/usr/bin/python
import time
from subprocess import check_output
import paho.mqtt.client as mqtt

def logdiagdata(logstring):
    global target_log       
    time_log_string = '{}-{}-{} {}:{}'.format(time.strftime("%d"),time.strftime("%m"),time.strftime("%Y"),time.strftime("%H:%M:%S"),logstring)
    print(time_log_string)
    target_log.write(time_log_string+"\n\r")

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    logdiagdata("Connected with result code "+str(rc))
    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("homeassistant/entertainment/tv/power")
    client.subscribe("homeassistant/entertainment/system/power")

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    logdiagdata(msg.topic+" "+str(msg.payload))
    if (msg.topic == "homeassistant/entertainment/tv/power"):
        if (msg.payload == b'ON'):
            logdiagdata("Telefunken TV On")
            check_output(['irsend','SEND_ONCE','telefunken_tv','KEY_POWER'],universal_newlines=True)
        if (msg.payload == b'OFF'):
            logdiagdata("Telefunken TV Off")
            check_output(['irsend','SEND_ONCE','telefunken_tv','KEY_POWER'],universal_newlines=True)            
    if (msg.topic == "homeassistant/entertainment/system/power"):
        if (msg.payload == b'ON'):
            logdiagdata("Denon On")
            check_output(['irsend','SEND_ONCE','denon_av','KEY_POWER'],universal_newlines=True)
            time.sleep(1)
            logdiagdata ("LG TV On")
            check_output(['irsend','SEND_ONCE','lg_tv','KEY_POWER'],universal_newlines=True)
        if (msg.payload == b'OFF'):
            logdiagdata("Denon Off")
            check_output(['irsend','SEND_ONCE','denon_av','KEY_SUSPEND'],universal_newlines=True)
            time.sleep(1)
            logdiagdata ("LG TV Off")
            check_output(['irsend','SEND_ONCE','lg_tv','KEY_POWER'],universal_newlines=True)

def on_publish(mosq, obj, mid):
    logdiagdata("mid: " + str(mid))

def on_subscribe(mosq, obj, mid, granted_qos):
    logdiagdata("Subscribed: " + str(mid) + " " + str(granted_qos))

def main():
    client = mqtt.Client()    
    client.on_connect = on_connect
    client.on_message = on_message
    client.on_publish = on_publish
    client.on_subscribe = on_subscribe
    
    client.connect("192.168.1.54",1883,60)    
    while True:
        client.loop_forever()        
        time.sleep(1)

target_log = open("remote_control_service.log",'w')
target_log.write("File Log Start.....")
logdiagdata("Waiting for startup to be complete...")
time.sleep(20)
logdiagdata("Startup complete!")

main()
