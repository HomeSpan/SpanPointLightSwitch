# SpanPointLightSwitch

 A HomeSpan demonstration using *SpanPoint* to transmit data to Remote Devices.

 This example contains three sketches:

 * **CentralHub.ino**:  a full HomeSpan sketch that implements two HomeKit LightBulb Accessories, but instead of connecting to LEDs attached to pins on the device, it uses *SpanPoint* to transmit on/off instructions to a remote ESP32 and a remote ESP8266 device that each control their own LED
   
 * **RemoteLight32.ino**: a lightweight HomeSpan sketch that does not connect to HomeKit at all, but instead uses *SpanPoint* to receive messages from the Central Hub to turn on/off a local LED accordingly
 * **RemoteLight8266.ino**: a stanadlone sketch that does not use HomeSpan[^1], but instead directly uses ESP-NOW functions to receive messages from the Central Hub to turn on/off a local LED accordingly

[^1]: HomeSpan does not run on ESP8266 devices
