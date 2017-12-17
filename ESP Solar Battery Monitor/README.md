# Micropython Hardware Platforms
Micropython can run on many platforms but most of these have an ARM type controller. The noteable exception is the port that runs on the ESP8266.
For a list of the hardware platforms supported have a look at the firmware download options [here](https://micropython.org/download).

In this instance we have loaded the Micropython firmware onto an ESP8266 (specifically the 12). For a very clear description of the hardware platform please refer to the document released by the Micropython team during their Kickstarter for the firmware. The document can be found [here](https://github.com/haemishkyd/KydHome/blob/master/Doc/ESP8266_Internals.pdf).

# Loading the Firmware
The ESP firmware needs to be loaded using a serial load tool. The tool I used was esptool which is available as a pypi package. This can be installed using *pip install esptool*
This is a good resource to find out how to use *esptool.py*.
Micropython by default includes many modules to interface with the hardware but also to simply do common things. 
This system use the MQTT implementation as well as the ADC interface.

# Basics
Micropython looks for two files at startup. It first looks for *boot.py* and then for *main.py*.
In this instance the *boot.py* simply connects to the WiFi network and starts WebRepl. Webrepl is a web interface that emulates the normal REPL interface that you would get in Python.
After executing the *boot.py, main.py* will be executed.
The main concession Micropython makes to the hardware is encapsulated in the module __machine__.
```python
import machine

pin=machine.Pin(0)
pin.On()
```
This example code will switch on a given pin (in this case pin 0).

# Upload of Python Scripts
Micropython (the firmware) creates a small file system in the flash that *boot.py* and *main.py* are kept. To upload files to this file system the WebRepl could be used. The alternative is to use third party programs that upload the files to the file system.
This [page](https://learn.adafruit.com/micropython-basics-how-to-load-micropython-on-a-board/serial-terminal?view=all#serial-terminal) explains how to get files onto the ESP8266.
