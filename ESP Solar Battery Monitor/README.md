# Micropython Hardware Platforms
Micropython can run on many platforms but most of these have an ARM type controller. The noteable exception is the port that runs on the ESP8266.
For a list of the hardware platforms supported have a look at the firmware download options [here](https://micropython.org/download).

In this instance we have loaded the Micropython firmware onto an ESP8266 (specifically the 12). For a very clear description of the hardware platform please refer to the document released by the Micropython team during their Kickstarter for the firmware. The document can be found [here](https://github.com/haemishkyd/KydHome/blob/master/Doc/ESP8266_Internals.pdf).

# Loading the Firmware
The ESP firmware needs to be loaded using a serial load tool. The tool I used was esptool which is available as a pypi package. This can be installed using *pip install esptool*

# Getting you Python Script Running
