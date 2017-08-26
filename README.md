# KydHome Home Automation Project
## Introduction/Overview
This is a project based around [Home Assitant](https://home-assistant.io/), ESP8266 devices, Micropython, Google Home and MQTT. To this point these are the technologies that have been involved, other technologies may be used in the future. Currently, through Home Assistant as a control interface, the project can:
* Switch on/off various lights
* Open/Close the gate of the house
* Monitor the state of the house alarm (active zones as well as armed/diarmed status)
* Monitor the state of the power coming into the house through an interface to the UPS
* Monitor the temperature outside the house
* Monitor the voltage of a bank of solar charged batteries
### Overview Diagram
![KydHome Overview](/Doc/kydhome_img/overview.png)
* Black Arrows: The commmunications backbone of the system is MQTT.
* Blue Arrows: SSL Encrypted internet communications to the system.
* Yellow: ESP8266 Modules - Embedded C
* Purple: ESP8266 Module  - Micropython
* Green: Python scripts running on Raspberry Pi or PC

## Communications Backbone
The communications backbone of the system is MQTT. A good description of MQTT is given [here](https://www.ibm.com/developerworks/mydeveloperworks/blogs/aimsupport/entry/what_is_mqtt_and_how_does_it_work_with_websphere_mq?lang=en)
The basic premise is that the central authority (the Mosquitto broker in my case) is the mailbox for all data exchange. Devices will subscrive to topics they consider useful. Othe devices subscribe to those topics. This way anyway interested in the topic "main_house/front_porch/temperature" can subscrive to it. The front porch temperature sensor then publishes to this topic by sending it to the broker. The broker then passes this message on to the various subsribers.

As mentioned this system uses a Mosquitto MQTT broker which requires **NO** setup and can be used immediately after install.
The various topics can be found in the configuration files (yaml) of the Home Assistant section. 

## Home Assistant 
Home Assistant is the main interface to the system as currently implemented. I am using the base installation and all that is configured as part of this project is the configuration file. Features that are currently used:
* MQTT Publish/Subscribe
* Automation (switches are changed when the sun rises or sets)
* Notifications through push bullet
* REST API interface from IFTTT 

**Refer to the [HomeAssistant](https://github.com/haemishkyd/KydHome/tree/master/HomeAssistant) section for more information on the setup of Home Assistant.**

## ESP8266
### Embdedded C on an ESP8266
Various terminals throughout the house are running on ESP8266 chips. The basic idea is that one code base is used for:
* Outside Temperature Sensor
* Outside Light Switch
* Gate Light Switch
* Gate Opener

**Refer to the [ESP Terminals](https://github.com/haemishkyd/KydHome/tree/master/ESP%20Terminals) section for more information on the setup and implmenentation of this.**

### Micropython on an ESP8266
Micropython was created in 2013 by Damien George. Micropython started off being ported to a specific piece of hardware called the pyboard. Micropython has now been ported to run on many platforms. 
The [Micropython website](https://micropython.org) is a good source of information.

**Refer to the [ESP Solar Battery Monitor](https://github.com/haemishkyd/KydHome/tree/master/ESP%20Solar%20Battery%20Monitor) section for more information on the setup and implmenentation of this.**

## Python Scripts
### UPS Power Monitor on a connected PC
This is a simple python script that uses the linux program [UPSC](https://linux.die.net/man/8/upsc) to query a UPS. The UPS provides a wealth of information on the state of the mains power in your home/office. I extract the data that I want using standard regular expressions in python. I then use the [Paho MQTT](https://pypi.python.org/pypi/paho-mqtt/1.1) module to publish this status to the MQTT broker.

### Universal Remote on a Raspberry Pi
The idea here is to be able to control the TV, DVD Player and AV Receiver via Home Assistant. This involved using:
* A Raspberry Pi
* Circuit of my own construction for the IR
* Circuit of my own construction to learn the commands.
* Python script on the Raspberry Pi to receive the MQTT messages and act on them.

**Refer to the [Universal Remote](https://github.com/haemishkyd/KydHome/tree/master/Universal%20Remote) section for more information on the setup and implmenentation of this.**

## Google Home Integration
