/*********************************************************************************
 *  MIT License
 *  
 *  Copyright (c) 2024 Gregg E. Berman
 *  
 *  https://github.com/HomeSpan/HomeSpan
 *  
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *  
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *  
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 *  
 ********************************************************************************/

// This example demonstrates how HomeSpan can control both a remote ESP32 and a remote ESP8266 via SpanPoint.
//
// The sketch below represent the Central Hub that is connected to HomeKit.  It is basically the same
// as Tutorial Example 5 that implements control for two LEDs.  However, instead of this sketch turning on/off
// LEDs that are connected to pins on the device, HomeSpan will use SpanPoint to transmit an on/off instruction
// to an ESP32 and/or an ESP8266.  When either of those devices receives the on/off instruction, it will turn on/off
// an LED accordingly.

// Though we only need to transmit instructions from the Central Hub to the two remote devices, this sketch is configured
// for both sending and receiving data from the remote devices to help demonstrate the method.  Even if you don't need
// the remote devices to transmit anything to the Central Hub, having a periodic heartbeat transmission is a good way
// of ensuring things are all working.  Also, if the remote ESP32 periodically transmits data to the Central Hub, SpanPoint
// will automatically calibrate the WiFi channel to ensure tranmission succeeds.

// Since SpanPoint only runs on an ESP32, we will be using the underlying ESP-NOW functions for the ESP8266 sketch.  In that
// sketch you'll note that there is no automatic calibration and the WiFi channel instead needs to be manually specified to match
// whatever channel the Central Hub uses once it is connected to your Home Network.

#include "HomeSpan.h"

//////////////////////////////////////

struct RemoteLight : Service::LightBulb {

  Characteristic::On power;             // this Characteristic determines if power to LED is on or off
  SpanPoint *remoteDevice;              // this is the SpanPoint connection used to transmit/receive data from the remote devices
  
  RemoteLight(const char *macAddress, boolean isESP8266) : Service::LightBulb(){        // the contructor takes two arguments - the MAC Address of the remote device, and a flag is remote device is ESP8266

    // Here we create the SpanPoint control using the MAC Address of the remote device:
    // the send size is set to the size of a boolean, since we will be transmitting nothing but a 1 or 0 to request power to be turned on or off to the remote LED;
    // the receive size is set to the size of an int - this will allow us to accept an arbitrary "ID" as part of a heartbeat message from each device to keep tabs on its status;
    // the queue-depth is set to 1; and
    // the last argument controls whether SpanPoint will listen for incoming data on its STA MAC Address (false), or its AP MAC Address (true), which is need when receiving data from an ESP8266
    
    remoteDevice=new SpanPoint(macAddress,sizeof(boolean),sizeof(int),1,isESP8266);
  }

  boolean update(){

    boolean lightStatus=power.getNewVal<boolean>();
    
    boolean success = remoteDevice->send(&lightStatus);
    Serial.printf("Send %s\n",success?"Succeded":"Failed");        
   
    return(true);
  }

  void loop(){          // this loop is optional -- it allows for the sketch to scan for any incoming 4-byte (int) messages from either of the remote devices

    int id;
    if(remoteDevice->get(&id))
      LOG0("Heartbeat received from: %d\n",id);       
  }

};

//////////////////////////////////////
     
void setup() {

  Serial.begin(115200);

  homeSpan.begin(Category::Lighting,"Central Hub");

  new SpanAccessory();  
  new Service::AccessoryInformation();
    new Characteristic::Identify();
  
  new SpanAccessory();  
    new Service::AccessoryInformation(); 
      new Characteristic::Identify();
      new Characteristic::Name("Remote ESP32");               
    new RemoteLight("84:CC:A8:11:B4:84",false);           // this is the MAC Address of a remote ESP32 device running SpanPoint

  new SpanAccessory();  
    new Service::AccessoryInformation(); 
      new Characteristic::Identify();
      new Characteristic::Name("Remote ESP8266");               
    new RemoteLight("BC:FF:4D:40:8E:71",true);           // this is the MAC Address of a remote ESP8266 device running ESP-NOW functions (note second argument is true)
}

//////////////////////////////////////

void loop(){
  
  homeSpan.poll();
  
}
