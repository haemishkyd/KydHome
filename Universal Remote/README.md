# Universal Remote Control
## Overview
This implementation uses LIRC - Linux Infrared Remote Control.
A basic circuit is needed to allow LIRC to use the Raspberry Pi's ports to flash the correct LED codes.

![Basic IR Circuit](/Doc/kydhome_img/IR-LED.png)

This [Link](http://www.raspberry-pi-geek.com/Archive/2015/10/Raspberry-Pi-IR-remote) describes the process of installing and configuring the software  and the hardware.

## Conf Directory
The __conf__ directory contains the LIRC files for the specific functions that have been learnt. These are the devices that I can currently controlling from this system.
These were all learnt manually and not retrieved from the LIRC database.

## Issues
1. The IR code for switching ON the denon receiver needs to be sent multiple times. I am not sure why. The off works correctly sending it once but the ON needs to be sent more than once.
