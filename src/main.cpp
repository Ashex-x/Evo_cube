#include <Arduino.h>
#include <Adafruit_NeoPixel.h> //WS2812B driver
#include <WiFi.h> //WiFi connection

// put function declarations here:


// config


void setup() {
  // general initialization
  Serial.begin(115200);

  //WiFi initialization
  WiFi.begin(WiFi_SSID, WiFi_password);


  // read config parameters

}

void loop() {
  // put your main code here, to run repeatedly:
}

// put function definitions here: