/* Main program for ESP32
* Notice: You can fix VAD application
*
*
*/

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>  // WS2812B driver
#include <WiFi.h>               // WiFi connection
#include <driver/i2s.h>         // i2s protocol
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <lwip/sockets.h>

WiFiClient client;
TaskHandle_t audioReadHandle;
TaskHandle_t audioSendHandle;

Adafruit_NeoPixel WS2812B(NUMS_WS2812B, DIN_WS2812B, NEO_GRB + NEO_KHZ800); // init WS2812


// put function declarations here:
void initLED();
void audio_i2s();
void audio_read(void *parameter);
void initPCM();
void led_screen(const int num_pixels);
void initWifi();
void connectServer();
void audio_send(void *parameter);

// Global parameter for audio
int16_t samples_a[INMPBUFFER_SIZE];
int16_t samples_b[INMPBUFFER_SIZE];
int16_t* currentBuffer = samples_a;
int16_t* sendBuffer = samples_b;

static unsigned long lastStatus = 0;
SemaphoreHandle_t audio_mutex = NULL;          // mutex to protect buffer pointers
SemaphoreHandle_t audio_ready_sem = NULL;      // Binary Semaphore to signal buffer readiness

// Debug varialbe
bool debug_1 = false;
bool debug_2 = false;
bool debug_3 = false;
bool debug_4 = false;

void setup() {
  // general initialization
  Serial.begin(115200);

  initLED();
  
  // -----------------WiFi and server--------------------
  initWifi();
  Serial.print("\nConnecting to server.");
  connectServer();

  // -----------------Audio protocol---------------------
  audio_i2s();

  audio_mutex = xSemaphoreCreateMutex();
  // Handle error: Mutex creation failed
  if (audio_mutex == NULL) {
    Serial.print("\nError: Failed to create audio_mutex!");
    return;
  }
  
  audio_ready_sem = xSemaphoreCreateBinary();
  if (audio_ready_sem == NULL) {
    // Handle error: Semaphore creation failed
    Serial.print("\nError: Failed to create audio_ready_sem!");
    return;
  }
    
  // -----------------FreeRTOS task---------------------
  xTaskCreatePinnedToCore(
    audio_read,         // Function name
    "Audio Task",       // Task name
    8192,               // Memory
    NULL,               // Parameter
    5,              
    &audioReadHandle,   // Handle
    1                   // Core
  );
    
  xTaskCreatePinnedToCore(
    audio_send,         // Function name
    "Network Task",     // Task name
    8192,               // Memory
    NULL,               // Parameter
    3,              
    &audioSendHandle,   // Handle
    0                   // Core
  );
  
}

void loop() {
  led_screen(NUMS_WS2812B);
  
  // Connection block
  /*
  if (millis() - lastStatus > 5000) {
    
    Serial.printf("\nDebug_1: %s", debug_1 ? "True" : "False");
    Serial.printf("Debug_2: %s", debug_2 ? "True" : "False");
    Serial.printf("\nDebug_3: %s", debug_3 ? "True" : "False");
    Serial.printf("Debug_4: %s", debug_4 ? "True" : "False");
  }
  */

  if (!client.connected()) {

    Serial.print("\nConnection lost. Reconnecting to server.");
    connectServer();
  } else {

    if (client.available()) {

      String response = client.readStringUntil('\n');
      Serial.print("\nServer: ");
      Serial.println(response);
    }
  }
}


// Put function definitions here:
void initLED() {
  WS2812B.begin();
  WS2812B.setBrightness(20);
}

void initWifi() {
  WiFi.begin(WiFi_SSID, WiFi_password);
  Serial.print("\nConnecting.");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected! local adress:");
  Serial.print(WiFi.localIP());
}

void connectServer() {
  while (!client.connect(SERVER_IP, SERVER_PORT)) {

    Serial.print(".");
    // Add a delay to avoid overwhelming the network and prevent stack overflow
    vTaskDelay(pdMS_TO_TICKS(500)); 
  }
  Serial.println("\nServer connected.");
  initPCM();
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
  
}

// -----------------Audio send---------------------

void audio_i2s() {
  const i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX), // Master mode, Receive (RX)
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = BITS_PER_SAMPLE,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_STAND_I2S),
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,            // Interrupt level 1
    .dma_buf_count = 4,                                  // 4 DMA buffers
    .dma_buf_len = INMPBUFFER_SIZE / 4,                  // DMA buffer length (in 32-bit samples)
    .use_apll = false,                                   // Use regular clock
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0                                      // Auto calculate clock
  };

  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);

  const i2s_pin_config_t pin_config = {
    .bck_io_num = INMP_SCK,
    .ws_io_num = INMP_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,                   // Not used for a microphone (RX mode)
    .data_in_num = INMP_SD
  };

  // Set the pin configuration
  i2s_set_pin(I2S_PORT, &pin_config);
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

void audio_read(void *parameter) {
  size_t bytes_read = 0;
  int32_t raw_samples[INMPBUFFER_SIZE];
  int is_speaking = 0;
  long long sum_energy = 0;
  int avg_energy = 0;

  // Initializing VAD(Voice Acivity Detection)
  Serial.print("\nKeep silence for a moment.Inintializing VAD.");
  for (int j = 0; j < 20; j++) // read 10 times
  {
    esp_err_t read_err = i2s_read(I2S_PORT, raw_samples, INMPBUFFER_SIZE * sizeof(int32_t), &bytes_read, portMAX_DELAY);
    
    for (int i = 0; i < INMPBUFFER_SIZE; i++) { 

      if (read_err != ESP_OK) {
        Serial.print("\nI2S read error.");
        break;
      } else {

        // Cast to int16_t automatically takes the lower 16 bits after the shift.
        currentBuffer[i] = (int16_t)(raw_samples[i] >> 16);
        // Serial.print("\n currentBuffer:");
        // Serial.print(currentBuffer[i]);
        // Calculate Sum
        if (j > 0) {
          if (currentBuffer[i] >= 0) {
            sum_energy += currentBuffer[i];
            /*
            if (sum_energy < currentBuffer[i]) {
              max_energy = currentBuffer[i];
              Serial.println(max_energy);
            }
            */
          } else {
            sum_energy -= currentBuffer[i];
            /*
            int a = 0 - currentBuffer[i];
            if (sum_energy < a) {
              max_energy = a;
              Serial.println(max_energy);
            }
              */
          }
        }
        

        if (currentBuffer[i] > 100 || (0 - currentBuffer[i]) > 100) {
          Serial.printf("Large energy: %d: %d", i, currentBuffer[i]);
        }

      }
    }

    Serial.printf("\nVAD process: %d %%", (j+1)*5);
  }
  
  avg_energy = (int)(sum_energy / (INMPBUFFER_SIZE*19));
  int cri_energy = 4 * avg_energy;
  Serial.print("\nVAD energy critical point:");
  Serial.print(cri_energy);
  
  while (true) {

    // -----------------------read audio---------------------------
    sum_energy = 0;
    // Serial.print("\nStart read audio");
    i2s_read(I2S_PORT, raw_samples, INMPBUFFER_SIZE * sizeof(int32_t), &bytes_read, portMAX_DELAY);
    // Serial.print("\nReading ");

    // Check for I2S read success before processing
    if (bytes_read != INMPBUFFER_SIZE * sizeof(int32_t)) {
      // Serial.print("not finish.");
      continue;
    }
    // Serial.print("finish.");

    // ------------------------detect voice------------------------
    for (int i = 0; i < INMPBUFFER_SIZE; i++) {

      // Cast to int16_t automatically takes the lower 16 bits after the shift.
      currentBuffer[i] = (int16_t)(raw_samples[i] >> 16);
      if (currentBuffer[i] >= 0) {
        sum_energy += currentBuffer[i];
      } else {
        sum_energy -= currentBuffer[i];
      }
    }

    avg_energy = sum_energy / INMPBUFFER_SIZE;
    debug_1 = true;
    



    if (xSemaphoreTake(audio_mutex, portMAX_DELAY) == pdTRUE) {

      int16_t* temp = sendBuffer;
      sendBuffer = currentBuffer;
      currentBuffer = temp;

      xSemaphoreGive(audio_mutex);
      
      if (avg_energy > cri_energy || is_speaking > 0) {
        is_speaking = 20; // Wait for next voice(0.064 * 20 = 1.28s)
        xSemaphoreGive(audio_ready_sem);
        is_speaking--;
      }
      
    }
    
  }
}


void audio_send(void *parameter) {
  while (xSemaphoreTake(audio_ready_sem, portMAX_DELAY) == pdTRUE)
  {

    if (!client.connected()) {

      Serial.print("\nConnection lost, attempting to reconnect...");
      
      // Try connecting once. If it fails, VTaskDelay will yield and try again later.
      if (!client.connect(SERVER_IP, SERVER_PORT)) {

        Serial.print(".");
        // Yield and wait 500ms before retrying
        vTaskDelay(pdMS_TO_TICKS(500));
        continue;
      }
      Serial.println("Reconnected.");

      initPCM();
    }

    size_t bytes_to_send = INMPBUFFER_SIZE * sizeof(int16_t);

    if (xSemaphoreTake(audio_mutex, portMAX_DELAY) == pdTRUE) {

      // Safely read/send the data from sendBuffer
      client.write((uint8_t*)sendBuffer, bytes_to_send);
      debug_4 = true;
      xSemaphoreGive(audio_mutex);
      
    }
    
    vTaskDelay(pdMS_TO_TICKS(1));

    /*
    if (bufferReady && client.connected()) {
      if (xSemaphoreTake(audio_mutex, portMAX_DELAY) == pdTRUE) {
        if (bufferReady) { 
          // Re-check after acquiring mutex
          // Safely read/send the data from sendBuffer
          client.write((uint8_t*)sendBuffer, INMPBUFFER_SIZE * sizeof(int16_t));
          bufferReady = false; // Reset the flag
        }
        xSemaphoreGive(audio_mutex);
      }
    }*/
  }
}