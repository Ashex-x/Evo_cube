#include <Arduino.h>
#include <Adafruit_NeoPixel.h> // WS2812B driver
#include <WiFi.h> // WiFi connection
#include <HTTPClient.h> // proxy need
#include <WiFiClient.h> // ptoxy need

#define TURL "api.openai.com"

WiFiClient client; // TCP socket object
HTTPClient http;   // HTTP

// put function declarations here:
// connect to WiFi and proxy
void network();
// get whisper
void sendAudioToWhisper(const byte* audioData, size_t audioSize, const char* fileName);

void setup() {
  // general initialization
  Serial.begin(115200);
  network();


  // read config parameters

}

void loop() {
  // put your main code here, to run repeatedly:
}

// put function definitions here:

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