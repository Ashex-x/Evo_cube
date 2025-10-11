// since this is just a small project, I only use one file to finish it.
// TODO: compelet FunASR
// TODO: compelet DPSK

#include <Arduino.h>
#include <Adafruit_NeoPixel.h> // WS2812B driver
#include <WiFi.h> // WiFi connection
#include <driver/i2s.h> // i2s protocol

WiFiClient client; 

Adafruit_NeoPixel WS2812B(NUMS_WS2812B, DIN_WS2812B, NEO_GRB + NEO_KHZ800); // init WS2812


// put function declarations here:
void init();
void audio();
void led_screen(const int num_pixels);


void setup() {
  // general initialization
  Serial.begin(115200);
  init();
  
}

void loop() {
  led_screen(NUMS_WS2812B);
  
}


// put function definitions here:
void init() {
  // ESP32 datasheets define
  

  // led init
  WS2812B.begin();
  WS2812B.setBrightness(20);

  // wifi connection
  WiFi.begin(WiFi_SSID, WiFi_password);
  Serial.print("Connecting.");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected! local adress:");
  Serial.print(WiFi.localIP()); 
  Serial.print("\nconnecting to server.");
  while(client.connect(SERVER_IP, SERVER_PORT)) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Server connected.");
}

void led_screen(const int num_pixels) {
  //TODO: finish this block
  
  WS2812B.clear();
  WS2812B.show();
  delay(2000);
  WS2812B.setPixelColor(9 , WS2812B.Color(255, 21, 21));
  WS2812B.setPixelColor(10, WS2812B.Color(255, 21, 21));
  WS2812B.setPixelColor(13, WS2812B.Color(255, 21, 21));
  WS2812B.setPixelColor(14, WS2812B.Color(255, 21, 21));
  WS2812B.setPixelColor(16, WS2812B.Color(255, 21, 21));
  WS2812B.setPixelColor(19, WS2812B.Color(255, 21, 21));
  WS2812B.setPixelColor(20, WS2812B.Color(255, 21, 21));
  WS2812B.setPixelColor(23, WS2812B.Color(255, 21, 21));
  WS2812B.setPixelColor(24, WS2812B.Color(255, 21, 21));
  WS2812B.setPixelColor(31, WS2812B.Color(255, 21, 21));
  WS2812B.setPixelColor(33, WS2812B.Color(255, 21, 21));
  WS2812B.setPixelColor(38, WS2812B.Color(255, 21, 21));
  WS2812B.setPixelColor(42, WS2812B.Color(255, 21, 21));
  WS2812B.setPixelColor(45, WS2812B.Color(255, 21, 21));
  WS2812B.setPixelColor(51, WS2812B.Color(255, 21, 21));
  WS2812B.setPixelColor(52, WS2812B.Color(255, 21, 21));
  WS2812B.show();
  delay(2000);
  
  
  /*
  WS2812B.clear();
  for (int pixel = 0; pixel < num_pixels; pixel++) {         // for each pixel
    WS2812B.setPixelColor(pixel, WS2812B.Color(0, 255, 0));  // it only takes effect if pixels.show() is called
    WS2812B.show();                                          // update to the WS2812B Led Strip

    delay(10);  // 500ms pause between each pixel
  }

  // turn off all pixels for two seconds
  WS2812B.clear();
  WS2812B.show();  // update to the WS2812B Led Strip
  delay(2000);     // 2 seconds off time

  // turn on all pixels to red at the same time for two seconds
  for (int pixel = 0; pixel < num_pixels; pixel++) {         // for each pixel
    WS2812B.setPixelColor(pixel, WS2812B.Color(255, 0, 0));  // it only takes effect if pixels.show() is called
  }
  WS2812B.show();  // update to the WS2812B Led Strip
  delay(1000);     // 1 second on time

  // turn off all pixels for one seconds
  WS2812B.clear();
  WS2812B.show();  // update to the WS2812B Led Strip
  delay(1000);     // 1 second off time
  */
}

void audio() {
  
}