// since this is just a small project, I only use one file to finish it.
// TODO: test internet connection and proxy
// TODO: compelet Whisper API
// TODO: compelet GTP3.5 API

#include <Arduino.h>
#include <Adafruit_NeoPixel.h> // WS2812B driver

#include <WiFi.h> // WiFi connection
#include <HTTPClient.h> // proxy need
#include <WiFiClient.h> // ptoxy need
#include <WireGuard-ESP32.h> // for VPN

#define TURL "api.openai.com"

WiFiClient client; // TCP socket object
HTTPClient http;   // HTTP

// put function declarations here:
// connect to WiFi and proxy
void network();

// get whisper
void sendAudioToWhisper(const byte* audioData, size_t audioSize, const char* fileName);

// WS2812B module
void led_screen(const int num_pixels);

// init WS2812 global value
Adafruit_NeoPixel WS2812B(NUMS_WS2812B, DIN_WS2812B, NEO_GRB + NEO_KHZ800);

void setup() {
  // general initialization
  Serial.begin(115200);
  WS2812B.begin();
  network();
  WS2812B.setBrightness(50);
}

void loop() {
  led_screen(NUMS_WS2812B);
  
}


// put function definitions here:
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


void network() {
  // connect to wifi
  WiFi.begin(WiFi_SSID, WiFi_password);
  if (client.connect(WiFi_proxyhost, WiFi_proxyport)) { // connect to proxy
    Serial.println("Connected to proxy."); 
  
    if (http.begin(client, TURL)) {
      // Manually initiate the HTTP request through the proxy connection
      int httpCode = http.GET();

      if (httpCode > 0) {
        String payload = http.getString();
        Serial.printf("HTTP Response Code: %d\n", httpCode);
        Serial.println(payload);
      } else {
        Serial.printf("HTTP GET failed, error: %s\n", http.errorToString(httpCode).c_str());
      }
      http.end();
    }
  } else {
    Serial.println("Failed to connect to proxy.");
  }
  
}

void sendAudioToWhisper(const byte* audioData, size_t audioSize, const char* fileName) {
  // Model
  String modelPart = "";
  modelPart += "--"; modelPart += BOUNDARY; modelPart += "\r\n";
  modelPart += "Content-Disposition: form-data; name=\"model\"\r\n\r\n";
  modelPart += WHISPER_MODEL; modelPart += "\r\n";
  
  // File Field Header
  String fileHeaderPart = "";
  fileHeaderPart += "--";
  fileHeaderPart += BOUNDARY;
  fileHeaderPart += "\r\n";
  fileHeaderPart += "Content-Disposition: form-data; name=\"file\"; filename=\"";
  fileHeaderPart += fileName; fileHeaderPart += "\"\r\n";
  fileHeaderPart += "Content-Type: audio/wav\r\n\r\n"; // use wav audio file as default
  
  // The Footer
  String footer = "\r\n--"; footer += BOUNDARY; footer += "--\r\n";

  // Calculate the Total Content Length
  // Total Length = modelPart_length + fileHeaderPart_length + audioSize + footer_length
  size_t totalLength = modelPart.length() + fileHeaderPart.length() + audioSize + footer.length();

  // Begin the HTTP Request
  http.begin(client, WHISPER_ENDPOINT);

  // Add Required Headers
  String authHeader = "Bearer ";
  authHeader += API_KEY;
  http.addHeader("Authorization", authHeader);
  http.addHeader("Content-Type", "multipart/form-data; boundary=" + String(BOUNDARY));
  http.addHeader("Content-Length", String(totalLength));

  Serial.printf("Sending %d bytes...\n", totalLength);

  // Send the Request (Header + Body Content)
  int httpCode = http.sendRequest("POST", modelPart);
  
  // Send the Binary File Data
  if (httpCode > 0) {
    client.write(fileHeaderPart.c_str(), fileHeaderPart.length());
    client.write(audioData, audioSize);
    client.write(footer.c_str(), footer.length());

    // Wait for and Process the Response
    if (httpCode == HTTP_CODE_OK) { // 200
      String payload = http.getString();
      Serial.println("\n--- WHISPER TRANSCRIPTION SUCCESS ---");
      // The response is usually a JSON string like: {"text": "Hello, world."}
      Serial.println(payload); 
    } else {
      Serial.printf("HTTP POST failed. Code: %d. Error: %s\n", httpCode, http.errorToString(httpCode).c_str());
      String response = http.getString();
      Serial.println("Server Response:");
      Serial.println(response);
    }
  } else {
    Serial.printf("Connection failed or initial request error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}


