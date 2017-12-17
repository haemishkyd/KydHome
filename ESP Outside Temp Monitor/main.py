import machine
import time
import onewire
import ds18x20
#import webrepl_setup
from umqtt.simple import MQTTClient

CONFIG = {
    "broker": "192.168.0.104",
    "client_id": "Front Outside Temp",
    "p1_topic": "homeassistant/sensor/frontoutsidetemperature",
}

def do_mqtt_connect(l_client):
    try:        
        l_client.connect()
        print("Connected to {}".format(CONFIG['broker']))        
    except OSError as e:
        print ("Could not connect!!")
    
client = MQTTClient(CONFIG['client_id'], CONFIG['broker'])
do_mqtt_connect(client)
main_scheduler_counter = 0

# the device is on GPIO12
dat = machine.Pin(12)

# create the onewire object
ds = ds18x20.DS18X20(onewire.OneWire(dat))

# scan for devices on the bus
roms = ds.scan()
print('found devices:', roms)
current_temp = 0
while True:   
    try:
        main_scheduler_counter = main_scheduler_counter + 1
        if (main_scheduler_counter > 30):
            main_scheduler_counter = 0
            print("Send Temp")
            client.publish(CONFIG['p1_topic'],
                           "{\"temp\":" + str(current_temp) + "}")
        ds.convert_temp()
        time.sleep(1)
        for rom in roms:
            current_temp = ds.read_temp(rom)
            print(current_temp, end=' ')
        print()
    except OSError as e:
        do_mqtt_connect(client)
        time.sleep(5)
    

machine.reset()
