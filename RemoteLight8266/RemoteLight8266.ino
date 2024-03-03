
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
 
#ifndef ARDUINO_ARCH_ESP8266
#error ERROR: THIS SKETCH IS DESIGNED FOR ESP8266 MICROCONTROLLERS!
#endif

// This is the sketch for a remote ESP8266 device, which cannot run HomeSpan or SpanPoint.  The sketch therefore directly uses ESP-NOW functions to receive and transmit data to the Main Device (the CENTRAL HUB)

uint8_t MAIN_DEVICE_MAC[6]={0xAC,0x67,0xB2,0x77,0x42,0x21};       // use the corresponding "Local MAC Address" listed under SpanPoint section when typing 'i' into the Serial Monitor ON THE CENTRAL HUB SKETCH
#define LED_PIN               0
#define CALIBRATION_PERIOD    20
#define INTERNAL_ID           8266

#include <ESP8266WiFi.h>                 
#include <espnow.h>
#include <Crypto.h>

uint32_t calibrateTime=0;

void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.printf("Last Packet Sent to %02X:%02X:%02X:%02X:%02X:%02X Status: %s\n",mac_addr[0],mac_addr[1],mac_addr[2],mac_addr[3],mac_addr[4],mac_addr[5],sendStatus==0?"Success":"Fail");
}

void OnDataRecv(uint8_t *mac_addr, uint8_t *incomingData, uint8_t len) {
  Serial.printf("Packet Receieved from %02X:%02X:%02X:%02X:%02X:%02X: %d bytes\n",mac_addr[0],mac_addr[1],mac_addr[2],mac_addr[3],mac_addr[4],mac_addr[5],len);
  digitalWrite(LED_PIN,!incomingData[0]);     // the only byte we should receive from the Maon Device is a boolean indicating if LED should be on or off (note reverse LED: HIGH=OFF, LOW=ON)
}

void setup() {
  
  Serial.begin(115200);
  delay(1000);

  Serial.printf("\n\nThis is a REMOTE Device with MAC Address = %s\n",WiFi.macAddress().c_str());
  Serial.printf("NOTE: This MAC Address must be entered into the corresponding SpanPoint() call of the MAIN Device.\n\n");

  WiFi.mode(WIFI_STA);
  wifi_set_channel(6);              // Here we need to manually set the channel to match whatever the Central Hub reports when typing 'i' on the Serial Monitor

  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW.  HALTING!");
    while(1);
  }  

  uint8_t hash[32];                 // create space to store as 32-byte hash code
  char password[]="HomeSpan";       // specify the password
  
  experimental::crypto::SHA256::hash(password,strlen(password),hash);     // create the hash code to be used further below

  esp_now_register_send_cb(OnDataSent);                      // register the callback for sending data
  esp_now_register_recv_cb(OnDataRecv);                      // register the callback for receiving data
  esp_now_set_self_role(ESP_NOW_ROLE_COMBO);                 // set the role of this device to be a controller (i.e. it sends data to the ESP32)

  esp_now_set_kok(hash+16,16);                                    // next we set the PMK.  For some reason this is called KOK on the ESP8266.  Note you must set the PMK BEFORE adding any peers
        
  esp_now_add_peer(MAIN_DEVICE_MAC, ESP_NOW_ROLE_COMBO, 0, hash, 16);    // now we add in the peer, set its role, and specify the LMK

  pinMode(LED_PIN,OUTPUT);
  digitalWrite(LED_PIN,1);                                   // the built-in LED on the Adafruit 8266 Featherboard is reverse wired - HIGH=OFF, LOW=ON
}

void loop() {

  if(millis()>calibrateTime){
    int id=INTERNAL_ID;
    esp_now_send(MAIN_DEVICE_MAC, (uint8_t *)&id, sizeof(id));     // Send the Data to the Main Device!
    calibrateTime=millis()+CALIBRATION_PERIOD*1000;
  }
}
