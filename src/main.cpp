// since this is just a small project, I only use one file to finish it.
// TODO: compelet FunASR
// TODO: compelet DPSK

#include <Arduino.h>
#include <Adafruit_NeoPixel.h> // WS2812B driver
#include <WiFi.h> // WiFi connection
#include <driver/i2s.h> // i2s protocol
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <lwip/sockets.h>

WiFiClient client; 

Adafruit_NeoPixel WS2812B(NUMS_WS2812B, DIN_WS2812B, NEO_GRB + NEO_KHZ800); // init WS2812


// put function declarations here:
void initLED();
void audio_i2s();
void audio_read();
void initPCM();
void led_screen(const int num_pixels);
void initWifi();
void audio_send();

// Global parameter for audio
int16_t samples_a[INMPBUFFER_SIZE];
int16_t samples_b[INMPBUFFER_SIZE];
int16_t* currentBuffer = samples_a;
int16_t* sendBuffer = samples_b;
volatile bool bufferReady = false; // Buffer ready or not

void setup() {
  // general initialization
  Serial.begin(115200);
  initLED();
  initWifi();
  initPCM();
  audio_i2s();

  
}

void loop() {
  led_screen(NUMS_WS2812B);
  
}


// put function definitions here:
void initLED() {
  WS2812B.begin();
  WS2812B.setBrightness(20);
}

void initWifi() {
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
  Serial.println("\nServer connected.");
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

void audio_i2s() {
  const i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX), // Master mode, Receive (RX)
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = BITS_PER_SAMPLE,
    .channel_format = MONO_CHANNEL,
    .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_STAND_I2S),
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1, // Interrupt level 1
    .dma_buf_count = 4,                       // 4 DMA buffers
    .dma_buf_len = INMPBUFFER_SIZE / 4,             // DMA buffer length (in 32-bit samples)
    .use_apll = false,                        // Use regular clock
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
  };

  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);

  const i2s_pin_config_t pin_config = {
    .bck_io_num = INMP_SCK,
    .ws_io_num = INMP_WS,
    .data_out_num = I2S_PIN_NO_CHANGE, // Not used for a microphone (RX mode)
    .data_in_num = INMP_SD
  };

  // Set the pin configuration
  i2s_set_pin(I2S_PORT, &pin_config);
}

void audio_read() {
  size_t bytes_read = 0;
  int32_t raw_samples[INMPBUFFER_SIZE];
  
  while (true) {
    bufferReady = false;
    i2s_read(I2S_PORT, raw_samples, INMPBUFFER_SIZE * sizeof(int32_t), &bytes_read, portMAX_DELAY);
    for (int i = 0; i < INMPBUFFER_SIZE; i++) {
      // Converte right-justified bits to low 16 bits
      currentBuffer[i] = (int16_t)(raw_samples[i] >> 16); 
    }
    int samples_count = bytes_read / sizeof(int32_t);
    int16_t processed_samples[samples_count]; 

    noInterrupts();
    int16_t* temp = sendBuffer;
    sendBuffer = currentBuffer;
    currentBuffer = temp;
    bufferReady = true;
    interrupts();
    
    delay(1);
  }
}

void initPCM() {
  // Create a header
  struct PCMHeader {
  uint32_t magic;           // 0x50434D31 "PCM1"
  uint32_t sampleRate;      // 16000
  uint16_t channels;        // 1
  uint16_t bitsPerSample;   // 16
  uint32_t dataSize;
};

  PCMHeader pcmHeader;

  pcmHeader.magic = 0x50434D31;
  pcmHeader.sampleRate = SAMPLE_RATE;
  pcmHeader.channels = 1;
  pcmHeader.bitsPerSample = 16;
  pcmHeader.dataSize = INMPBUFFER_SIZE * sizeof(int16_t);

  // Send PDM header
  if (client.connected()) {
    client.write((uint8_t*)&pcmHeader, sizeof(pcmHeader));
  } else {
    Serial.println("Connection error.");
  }
}

void audio_send() {
  if (bufferReady && client.connected()) {
    noInterrupts();
    // Send PCM
    client.write((uint8_t*)sendBuffer, INMPBUFFER_SIZE * sizeof(int16_t));
    bufferReady = false;
    interrupts();
  }
  
  // Connect error
  if (!client.connected()) {
    Serial.println("Connection lost, attempting to reconnect...");
    Serial.print("\nconnecting to server.");
    while(client.connect(SERVER_IP, SERVER_PORT)) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("\nServer connected.");
  }
  
  delay(1);
}