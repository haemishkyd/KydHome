#!/usr/bin/python
import time
from subprocess import check_output
import paho.mqtt.client as mqtt
from gtts import gTTS
import os
import _thread

def logdiagdata(logstring):
    global target_log       
    time_log_string = '{}-{}-{} {}:{}'.format(time.strftime("%d"),time.strftime("%m"),time.strftime("%Y"),time.strftime("%H:%M:%S"),logstring)
    print(time_log_string)
    target_log.write(time_log_string+"\n\r")

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    global Connected
    logdiagdata("Connected with result code "+str(rc))
    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("homeassistant/entertainment/tv/power")
    client.subscribe("homeassistant/entertainment/system/power")
    client.subscribe("homeassistant/entertainment/system/streaming")
    client.subscribe("homeassistant/entertainment/system/volume")
    client.subscribe("homeassistant/audio_prompt/message")
    Connected = True

def on_disconnect(client, userdata, rc):
    global Connected
    Connected = False

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    logdiagdata(msg.topic+" "+str(msg.payload))
    if (msg.topic == "homeassistant/audio_prompt/message"):
        # os.system("espeak '"+msg.payload.decode('utf-8')+"'")
        logdiagdata("Generate mp3 name from text sent.")
        mp3_file_name = msg.payload.decode('utf-8')
        mp3_file_name = mp3_file_name.replace('\'','')
        mp3_file_name = mp3_file_name.replace(' ','_')
        mp3_file_name = mp3_file_name.lower()
        if (not (os.path.exists("sound_files/"+mp3_file_name+".mp3"))):
            logdiagdata("Generate mp3 of given message.")
            tts = gTTS(text=msg.payload.decode('utf-8'), lang='en')
            logdiagdata("Save the mp3.")
            tts.save("sound_files/"+mp3_file_name+".mp3")
        logdiagdata("Start the thread to play the mp3.")
        _thread.start_new_thread( play_sound, ("Playing Sound", mp3_file_name) )
    if (msg.topic == "homeassistant/entertainment/tv/power"):
        if (msg.payload == b'ON'):
            logdiagdata("Telefunken TV On")
            check_output(['irsend','SEND_ONCE','telfunken_tv','KEY_POWER'],universal_newlines=True)
        if (msg.payload == b'OFF'):
            logdiagdata("Telefunken TV Off")
            check_output(['irsend','SEND_ONCE','telfunken_tv','KEY_POWER'],universal_newlines=True)            
    if (msg.topic == "homeassistant/entertainment/system/power"):
        if (msg.payload == b'ON'):
            logdiagdata ("LG TV On")
            check_output(['irsend','SEND_ONCE','lg_tv','KEY_POWER'],universal_newlines=True)
            time.sleep(1)
            logdiagdata("Denon On")
            check_output(['irsend','SEND_ONCE','denonav','KEY_POWER'],universal_newlines=True)            
            check_output(['irsend','SEND_ONCE','denonav','KEY_POWER'],universal_newlines=True)
            check_output(['irsend','SEND_ONCE','denonav','KEY_POWER'], universal_newlines=True)
        if (msg.payload == b'OFF'):
            logdiagdata("Denon Off")
            check_output(['irsend','SEND_ONCE','denonav','KEY_SUSPEND'],universal_newlines=True)
            time.sleep(1)
            logdiagdata ("LG TV Off")
            check_output(['irsend','SEND_ONCE','lg_tv','KEY_POWER'],universal_newlines=True)
    if (msg.topic == "homeassistant/entertainment/system/streaming"):
        if (msg.payload == b'ON'):
            logdiagdata("Switch to SAT")
            check_output(['irsend','SEND_ONCE','denonav','KEY_SAT'],universal_newlines=True)
            time.sleep(1)
            logdiagdata ("DVD Player Off")
            check_output(['irsend','SEND_ONCE','lg_dvd','KEY_POWER'],universal_newlines=True)
        if (msg.payload == b'OFF'):
            logdiagdata("Switch To DVD")
            check_output(['irsend','SEND_ONCE','denonav','KEY_DVD'],universal_newlines=True)
            time.sleep(1)
            logdiagdata ("DVD Player On")
            check_output(['irsend','SEND_ONCE','lg_dvd','KEY_POWER'],universal_newlines=True)
    if (msg.topic == "homeassistant/entertainment/system/volume"):
        if (msg.payload == b'UP_S'):
            logdiagdata("Volum Up Single")
            volume_up(2)
        if (msg.payload == b'DOWN_S'):
            logdiagdata("Volum Down Single")
            volume_down(2)
        if (msg.payload == b'UP_D'):
            logdiagdata("Volum Up Double")
            volume_up(10)
        if (msg.payload == b'DOWN_D'):
            logdiagdata("Volum Down Double")
            volume_down(10)
        if (msg.payload == b'UP_T'):
            logdiagdata("Volum Up Triple")
            volume_up(20)
        if (msg.payload == b'DOWN_T'):
            logdiagdata("Volum Down Triple")
            volume_down(20)

def volume_up(number_of_presses):
    for presses in range(number_of_presses):
        check_output(['irsend','SEND_ONCE','denonav','KEY_VOLUMEUP'],universal_newlines=True)
        time.sleep(0.5)

def volume_down(number_of_presses):
    for presses in range(number_of_presses):
        check_output(['irsend','SEND_ONCE','denonav','KEY_VOLUMEDOWN'],universal_newlines=True)
        time.sleep(0.5)

def on_publish(mosq, obj, mid):    
    logdiagdata("mid: " + str(mid))

def on_subscribe(mosq, obj, mid, granted_qos):
    logdiagdata("Subscribed: " + str(mid) + " " + str(granted_qos))

def play_sound(thread_name,mp3_name):
    logdiagdata("Start playing mp3 in thread.")
    os.system("omxplayer 'sound_files/"+mp3_name+".mp3'")
    logdiagdata("Playing complete.")

def main():
    global Connected
    client = mqtt.Client()    
    client.on_connect = on_connect
    client.on_message = on_message
    client.on_publish = on_publish
    client.on_subscribe = on_subscribe
    client.on_disconnect    = on_disconnect    

    while True:
        if (Connected == False):
            client.connect("192.168.0.104",1883,60)
            client.loop_start()   
            logdiagdata("Connecting!")
            time.sleep(1)
        time.sleep(1)

Connected = False
target_log = open("remote_control_service.log",'w')
target_log.write("File Log Start.....")
logdiagdata("Waiting for startup to be complete...")
time.sleep(20)
logdiagdata("Startup complete!")

main()
