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
## Home Assistant 
Home Assistant is the main interface to the system as currently implemented. I am using the base installation and all that is configured as part of this project is the configuration file. Features that are currently used:
* MQTT Publish/Subscribe
* Automation (switches are changed when the sun rises or sets)
* Notifications through push bullet
* REST API interface from IFTTT 

## MQTT

## ESP8266

### Embdedded Bare Metal 

### Micropython

## UPS Power Monitor

## Hardware Discussion

## Google Home Integration
