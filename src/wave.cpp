#include "wave.h"  // 包含头文件

// 构造函数：初始化成员变量
WaveBreathEffect::WaveBreathEffect(uint16_t width, uint16_t height, CRGB* ledArray) {
  matrixWidth = width;
  matrixHeight = height;
  numLeds = width * height;
  leds = ledArray;  // 接收外部传入的LED数组（避免重复定义）
  
  // 初始化呼吸灯参数
  breathBrightness = 0;    // 初始亮度
  breathDirection = true;  // 亮度变化方向（true=递增，false=递减）
  breathSpeed = 5;         // 呼吸速度（数值越小越快）
  
  // 初始化波浪相关的时间变量
  prevWaveTime = 0;
  prevBreathTime = 0;
}

// 初始化函数：在setup()中调用
void WaveBreathEffect::begin() {
  initWaves();  // 初始化波浪
}

// 主更新函数：在loop()中调用，更新所有效果
void WaveBreathEffect::update() {
  unsigned long currentTime = millis();
  
  // 更新呼吸灯亮度（核心呼吸逻辑）
  if (currentTime - prevBreathTime >= breathSpeed) {
    if (breathDirection) {
      // 亮度递增
      breathBrightness++;
      if (breathBrightness >= 35) {
        breathDirection = false;  // 到达最大亮度，反转方向
      }
    } else {
      // 亮度递减
      breathBrightness--;
      if (breathBrightness <= 0) {
        breathDirection = true;  // 到达最小亮度，反转方向
      }
    }
    prevBreathTime = currentTime;
  }
  
  // 定时生成新波浪（图案控制逻辑）
  if (currentTime - prevWaveTime >= 1000) {  // 每1秒尝试生成一个新波浪
    spawnRandomWave();
    prevWaveTime = currentTime;
  }
  
  // 更新所有活跃的波浪
  for (int i = 0; i < MAX_WAVES; i++) {
    if (waves[i].active) {
      updateWave(i);  // 更新波浪参数（如半径增大）
      drawWave(i);    // 绘制波浪到LED矩阵
    }
  }
}

// 设置呼吸速度（外部可调用）
void WaveBreathEffect::setBreathSpeed(uint8_t speed) {
  breathSpeed = speed;
}

// 获取当前呼吸亮度（外部可调用）
uint8_t WaveBreathEffect::getBreathBrightness() {
  return breathBrightness;
}

// 获取活跃波浪数量（外部可调用）
uint8_t WaveBreathEffect::getActiveWaveCount() {
  uint8_t count = 0;
  for (int i = 0; i < MAX_WAVES; i++) {
    if (waves[i].active) count++;
  }
  return count;
}

// 初始化波浪（私有方法，内部调用）
void WaveBreathEffect::initWaves() {
  // 先将所有波浪标记为非活跃
  for (int i = 0; i < MAX_WAVES; i++) {
    waves[i].active = false;
  }
  // 生成第一个波浪
  spawnRandomWave();
}

// 随机生成一个新波浪（私有方法，内部调用）
void WaveBreathEffect::spawnRandomWave() {
  // 找到第一个空闲的波浪位置
  for (int i = 0; i < MAX_WAVES; i++) {
    if (!waves[i].active) {
      // 随机初始化波浪参数（图案的核心参数）
      waves[i].centerX = random(matrixWidth);   // 随机X中心
      waves[i].centerY = random(matrixHeight);  // 随机Y中心
      waves[i].radius = 0;                      // 初始半径为0
      waves[i].speed = 0.5 + (random(10) / 10.0);  // 随机速度（0.5-1.5）
      waves[i].amplitude = 1.0 + (random(5) / 10.0);  // 随机振幅（1.0-1.5）
      waves[i].hue = random(256);               // 随机颜色（HSL色调）
      waves[i].active = true;                   // 标记为活跃
      break;
    }
  }
}

// 更新单个波浪的状态（私有方法，内部调用）
void WaveBreathEffect::updateWave(int waveIndex) {
  if (!waves[waveIndex].active) return;
  
  // 波浪半径随时间增大（模拟扩散效果）
  waves[waveIndex].radius += waves[waveIndex].speed;
  
  // 当波浪超出矩阵范围时，标记为非活跃
  float maxDistance = sqrt(matrixWidth * matrixWidth + matrixHeight * matrixHeight);
  if (waves[waveIndex].radius > maxDistance * 1.5) {
    waves[waveIndex].active = false;
  }
}

// 绘制单个波浪到LED矩阵（私有方法，内部调用）
void WaveBreathEffect::drawWave(int waveIndex) {
  if (!waves[waveIndex].active) return;
  
  Wave &wave = waves[waveIndex];  // 引用当前波浪，简化代码
  
  // 遍历矩阵中的每个LED
  for (uint8_t y = 0; y < matrixHeight; y++) {
    for (uint8_t x = 0; x < matrixWidth; x++) {
      // 计算当前LED到波浪中心的距离
      float dx = x - wave.centerX;
      float dy = y - wave.centerY;
      float distance = sqrt(dx * dx + dy * dy);
      
      // 计算该位置的波浪强度（核心图案算法）
      float waveIntensity = calculateWaveIntensity(distance, wave.radius, wave.amplitude);
      
      // 如果强度大于0，则绘制颜色
      if (waveIntensity > 0) {
        // 结合呼吸亮度和波浪强度计算最终颜色
        CRGB color = CHSV(wave.hue, 255, waveIntensity * breathBrightness);
        setMatrixPixel(x, y, color);  // 设置LED颜色
      }
    }
  }
}

// 计算波浪在某位置的强度（私有方法，内部调用）
float WaveBreathEffect::calculateWaveIntensity(float distance, float radius, float amplitude) {
  // 用正弦函数和指数衰减模拟波浪形状（核心数学模型）
  float diff = distance - radius;
  float intensity = cos(diff * 0.5) * exp(-0.1 * abs(diff));
  return max(0.0f, intensity * amplitude);  // 确保强度非负
}

// 设置矩阵中某个点的LED颜色（私有方法，内部调用）
void WaveBreathEffect::setMatrixPixel(uint8_t x, uint8_t y, CRGB color) {
  // 根据矩阵布局计算LED索引（这里假设是行优先布局）
  uint16_t index = y * matrixWidth + x;
  
  // 确保索引在有效范围内，避免越界
  if (index < numLeds) {
    leds[index] += color;  // 叠加颜色（多个波浪可混合）
  }
}
    