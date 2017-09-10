# ESP Embedded C Terminals
## Overview
The entire project consists of network enabled terminals around the house. There are three main types.
* Python running on Raspberry Pis/Computers
* ESP8266 Modules running [Micropython](https://micropython.org/)
* ESP8266 Modules running embedded C application

This section describes those modules running embedded C.

## Types
There are four type of functionality built into the same code base for the embedded C ESP modules.
* AC Switch (for a light)
* Temperature Sensor (1 wire interface)
* Gate Switch
* Alarm System Interface (Paradox alarm system)

## Hardware Description
For all of these I have used a basic breakout hardware platform that allows for a triac controlled switch, power regulation and basic IO breakout. The implementations below are based on this and supporting hardware has been added as/if required. See [this](https://github.com/haemishkyd/KydHome/blob/master/Doc/ESP-Switch-Board.pdf) document for a description of this base hardware.

### Temperature Sensor
The temperature sensor used is a [DS18B20](https://github.com/haemishkyd/KydHome/blob/master/Doc/DS18B20.pdf). This is a one wire temperature sensor, which is a pain in the ass, but given the low pin count on ESP chips this seemed like a good idea.

## Implementation Notes

### Gate Control
The gate control uses basic IO. There is nothing other than GPIO manipulation (and of course the MQTT interface).
Setting and getting GPIO on the ESP can be a bit confusing. A good place to start is making sure you understand the multiplexed pins and what each can do.
The diagram below gives a basic idea of some of the pins:
![ESP GPIO Layout](https://github.com/haemishkyd/KydHome/blob/master/Doc/1.png)

### Alarm System Interface
The Paradox alarm system outputs a serial message whenever an event occurrs in the system. Events include arming and disarming of alarm, activation of PIR sensors, the alarm going off etc. The protocol description is given in the picture below.
![Paradox Serial](https://github.com/haemishkyd/KydHome/blob/master/Doc/kydhome_img/paradox_serial.png)
