# SpanPointLightSwitch - A HomeSpan Project

SpanPointLightSwitch is a working example of how to use HomeSpan's **SpanPoint** class to implement bi-directional ESP-NOW communication between a Central Hub running HomeSpan that is connected to HomeKit, and two separate remote devices that are not connected to HomeKit but directly control two LEDs.  Built using the [HomeSpan](https://github.com/HomeSpan/HomeSpan) HomeKit Library, SpanPointLightSwitch is designed to run on an ESP32 device as an Arduino sketch.  

Hardware used for this project:

* Two ESP32 boards, such as the [Adafruit HUZZAH32 – ESP32 Feather Board](https://www.adafruit.com/product/3405)
* One ESP8266 board, such as the [Adafruit Feather HUZZAH with ESP8266](https://www.adafruit.com/product/2821)
* The internal LEDs built into each board

## Overview

This example contains three sketches:

 * **CentralHub.ino**:  a full HomeSpan sketch that implements two HomeKit LightBulb Accessories, but instead of connecting two LEDs attached to pins on the device, it uses HomeSpan's **SpanPoint** class to transmit on/off instructions to a remote ESP32 and a remote ESP8266 device that each control their own LED
   
 * **RemoteLight32.ino**: a lightweight HomeSpan sketch that does not connect to HomeKit at all, but still uses **SpanPoint** to receive messages from the Central Hub requesting that the on-board LED turned on or off
   
 * **RemoteLight8266.ino**: a standalone sketch that does not use HomeSpan[^1], but instead directly uses ESP8286 ESP-NOW functions to receive messages from the Central Hub requesting that the on-board LED turned on or off

## Notes

 * Though using a Central Hub connected to HomeKit to communicate on/off LED instructions to two remote devices via **SpanPoint** requires only the *one-way* transmission of messages from the Central Hub to the remote devices, the sketches in this example demonstrate *bi-directional* SpanPoint/ESP-NOW communication by having the remote devices also send periodic "heatbeat" message back to the Central Hub

 * Though the heatbeat messages are primarily for illustrative purposes, for the remote ESP32 device it also has the practical benefit of causing the instance of SpanPoint running on that device to auto-calibrate the WiFi channel used for ESP-NOW.  This enures the two ESP32 devices are always on the same channel even if the Central Hub device needs to change channels to connect to your central WiFi network
 
 * Bi-Directional communication would also be used if you wanted to extend the example sketches by adding local pushbutton switches on each remote device to manually turn on or off the light connected to that device.  In such a setup each remote device would transmit an on/off status update to the Central Hub whenever the pushbuton switch was pressed so that the Central Hub can communicate the change of state back to HomeKit where it will be properly reflected in your Home App (see the [HomeSpan Tutorials](https://github.com/HomeSpan/HomeSpan/blob/master/docs/Tutorials.md) page for detailed examples of how to implement pushbutton switches and communicste status changes to HomeKit) 
  
 * Since HomeSpan (and thus SpanPoint) does not run on an ESP8266 chip, automatic calibration is not performed and the ESP8266 sketch has the WiFi channel hardcoded.  Please make sure to change this to match the channel used by the Central Hub[^2]
   
 * The primary purpose of this sketch is to demonstrate how to configure SpanPoint/ESP-NOW to facilate *bi-directional communication between the Central Hub ESP32 and the remote ESP8266*.  If all you are using is ESP32 devices, you would more likely not bother with a Central Hub and simply run HomeSpan on each ESP32 device that controls a light
  
 * Please pay special attention to which MAC addresses are need for each pair of SpanPoint/ESP-NOW connections.  For ESP32 devices, you can simultaneously use ESP-NOW and connect to a WiFi network as a Station.  But when an ESP8266 transmits data to an ESP32, it must used the Soft-AP MAC Address of that ESP32 device.  The **SpanPoint** constructor has a parameter to specify whether any given instant of **SpanPoint** should use the STA or AP address.  Since our example includes one remote ESP32 and one remote ESP8266 device, both the STA and AP MAC Addresses of the Central Hub ESP32 device are used, and both will be printed on the Serial Monitor under "SpanPoint Connections" when the Central Hub first boots 

[^1]: HomeSpan (and thus SpanPoint) does not run on ESP8266 devices
[^2]: You could of course develop your own auto-calibration logic for the ESP8266 sketch, such as connecting it *temporarily* to your central WiFi Network to identify the channel used by your WiFi Network 
