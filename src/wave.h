#ifndef WAVE_BREATH_EFFECT_H
#define WAVE_BREATH_EFFECT_H

#include <FastLED.h>

class WaveBreathEffect {
  private:
    // LED矩阵配置
    uint16_t matrixWidth;
    uint16_t matrixHeight;
    uint16_t numLeds;
    CRGB* leds;
    
    // 波浪参数结构体
    struct Wave {
      float centerX, centerY;
      float radius;
      float speed;
      float amplitude;
      uint8_t hue;
      bool active;
    };
    
    static const uint8_t MAX_WAVES = 3;
    Wave waves[MAX_WAVES];
    
    // 呼吸参数
    uint8_t breathBrightness;
    bool breathDirection;
    uint8_t breathSpeed;
    unsigned long prevWaveTime;
    unsigned long prevBreathTime;
    
    // 私有方法
    void initWaves();
    void spawnRandomWave();
    void updateWave(int waveIndex);
    void drawWave(int waveIndex);
    float calculateWaveIntensity(float distance, float radius, float amplitude);
    void setMatrixPixel(uint8_t x, uint8_t y, CRGB color);
  
  public:
    // 构造函数
    WaveBreathEffect(uint16_t width, uint16_t height, CRGB* ledArray);
    
    // 初始化
    void begin();
    
    // 更新效果 - 主循环中调用的函数
    void update();
    
    // 参数设置
    void setBreathSpeed(uint8_t speed);
    
    // 状态获取
    uint8_t getBreathBrightness();
    uint8_t getActiveWaveCount();
};

#endif
