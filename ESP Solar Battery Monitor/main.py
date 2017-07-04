import machine
import time
from umqtt.simple import MQTTClient

# define the voltage divider
R1 = 21.6
R2 = 0.994

CONFIG = {
    "broker": "192.168.1.54",
    "client_id": "POWER MONITOR",
    "topic": "homeassistant/sensor/powersystem/solarbat",
}

adc = machine.ADC(0)

while True:
    try:
        client = MQTTClient(CONFIG['client_id'], CONFIG['broker'])
        client.connect()
        print("Connected to {}".format(CONFIG['broker']))
        raw_val = adc.read()
        print ("Measured Voltage: {} mv".format(raw_val / 1.024))
        # R2 - 1k ohm
        # R1 - 22k ohm
        current_voltage = ((raw_val / 1024) * (R1 + R2)) / R2
        print ("Battery Voltage: {}V".format(current_voltage))
        client.publish(CONFIG['topic'], str(current_voltage))
        client.disconnect()
    except OSError as e:
        print ("Could not connect!!")
    time.sleep(60)

machine.reset()
