#ifndef FORM_H  
#define FORM_H
#include <FastLED.h>
#define MATRIX_WIDTH 16
#define MATRIX_HEIGHT 16
#define NUM_LEDS 256       // 16×16点阵 = 256个LED
#define DATA_PIN 2         // 更换为GPIO 2，这是ESP32上的有效引脚
int getIndex(int x, int y);

extern const CRGB P = CRGB(128, 0, 128);    // 紫色轮廓
const CRGB Y = CRGB(255, 255, 0);    // 黄色填充
const CRGB B = CRGB(0, 0, 255);        // 蓝色元素
const CRGB R = CRGB(255, 0, 0);         // 红色装饰
const CRGB K = CRGB(0, 0, 0);         // 黑色细节
const CRGB W = CRGB(255, 255, 255);
// 定义LED数组
CRGB leds[NUM_LEDS];

// 定义点阵屏尺寸
#define MATRIX_WIDTH 16
#define MATRIX_HEIGHT 16
void colorWipe(CRGB color, int wait) 
{
  for(int i = 0; i < NUM_LEDS; i++) {
    leds[i] = color;
    FastLED.show();
    delay(wait);
  }
  delay(500);
  
  FastLED.show();
}



// 全亮闪烁
void blinkAll(CRGB color, int wait) {
  fill_solid(leds, NUM_LEDS, color);
  FastLED.show();
  delay(wait);
  FastLED.clear();
  FastLED.show();
  delay(wait);
}

// 棋盘格效果
void drawCheckerboard(CRGB color1, CRGB color2, int wait) {
  for(int y = 0; y < MATRIX_HEIGHT; y++) {
    for(int x = 0; x < MATRIX_WIDTH; x++) {
      int index = getIndex(x, y);
      if((x + y) % 2 == 0) {
        leds[index] = color1;
      } else {
        leds[index] = color2;
      }
    }
  }
  FastLED.show();
  delay(wait);
  FastLED.clear();
  FastLED.show();
}

// 绘制十字
void drawCross(CRGB color, int wait) {
  int center = MATRIX_WIDTH / 2;
  
  // 水平线
  for(int x = 0; x < MATRIX_WIDTH; x++) {
    int index = getIndex(x, center);
    leds[index] = color;
  }
  
  // 垂直线
  for(int y = 0; y < MATRIX_HEIGHT; y++) {
    int index = getIndex(center, y);
    leds[index] = color;
  }
  
  FastLED.show();
  delay(wait);
  
  FastLED.show();
}



void  Smile() 
{
const CRGB pattern[16][16] = 
{
  {Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y},
  {Y, Y, Y, Y, Y, P, P, P, P, P, P, Y, Y, Y, Y, Y},
  {Y, Y, Y, Y, P, Y, Y, Y, Y, Y, Y, P, Y, Y, Y, Y},
  {Y, Y, Y, P, Y, Y, Y, Y, Y, Y, Y, Y, P, Y, Y, Y},
  {Y, Y, P, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, P, Y, Y},
  {Y, P, Y, Y, Y, P, Y, Y, Y, Y, P, Y, Y, Y, P, Y},
  {Y, P, Y, Y, Y, P, Y, Y, Y, Y, P, Y, Y, Y, P, Y},
  {Y, P, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, P, Y},
  {Y, P, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, P, Y},
  {Y, P, Y, P, Y, Y, Y, Y, Y, Y, Y, Y, P, Y, P, Y},
  {Y, P, Y, Y, P, Y, Y, Y, Y, Y, Y, P, Y, Y, P, Y},
  {Y, Y, P, Y, Y, P, P, P, P, P, P, Y, Y, P, Y, Y},
  {Y, Y, Y, P, Y, Y, Y, Y, Y, Y, Y, Y, P, Y, Y, Y},
  {Y, Y, Y, Y, P, Y, Y, Y, Y, Y, Y, P, Y, Y, Y, Y},
  {Y, Y, Y, Y, Y, P, P, P, P, P, P, Y, Y, Y, Y, Y},
  {Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y}
  };
for(int y = 0; y < MATRIX_HEIGHT; y++) {
    for(int x = 0; x < MATRIX_WIDTH; x++) {
      int index = getIndex(x, y);  // 获取LED索引
      if(index >= 0 && index < NUM_LEDS) {  // 确保索引有效
        leds[index] = pattern[y][x];  // 将颜色值写入LED
      }
    }
  }
}


void sad()
{
 const CRGB pattern[16][16] = {
   {W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W},
   {W, W, W, W, W, B, B, B, B, B, B, W, W, W, W, W},
   {W, W, W, W, B, Y, Y, Y, Y, Y, Y, B, W, W, W, W},
   {W, W, W, B, Y, Y, Y, Y, Y, Y, Y, Y, B, W, W, W},
   {W, W, B, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, B, W, W},
   {W, B, Y, Y, Y, B, Y, Y, Y, Y, B, Y, Y, Y, B, W},
   {W, B, Y, Y, Y, B, Y, Y, Y, Y, B, Y, Y, Y, B, W},
   {W, B, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, B, W},
   {W, B, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, B, W},
   {W, B, Y, Y, Y, B, B, B, B, B, B, Y, Y, Y, B, W},
   {W, B, Y, Y, B, Y, Y, Y, Y, Y, Y, B, Y, Y, B, W},
   {W, W, B, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, B, W, W},
   {W, W, W, B, Y, Y, Y, Y, Y, Y, Y, Y, B, W, W, W},
   {W, W, W, W, B, Y, Y, Y, Y, Y, Y, B, W, W, W, W},
   {W, W, W, W, W, B, B, B, B, B, B, W, W, W, W, W},
   {W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W}
};
for(int y = 0; y < MATRIX_HEIGHT; y++) {
    for(int x = 0; x < MATRIX_WIDTH; x++) {
      int index = getIndex(x, y);  // 获取LED索引
      if(index >= 0 && index < NUM_LEDS) {  // 确保索引有效
        leds[index] = pattern[y][x];  // 将颜色值写入LED
      }
    }
  }
}
void Crying() {  
const CRGB pattern[16][16] = {
  {W, W, W, W, W, W, K, K, K, K, K, W, W, W, W, W},
  {W, W, W, W, K, K, Y, Y, Y, Y, Y, K, K, W, W, W},
  {W, W, W, K, Y, Y, Y, Y, Y, Y, Y, Y, Y, K, W, W},
  {W, W, K, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, K, W},
  {W, W, K, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, K, W},
  {W, K, Y, Y, K, K, K, K, Y, K, K, K, K, Y, Y, K},
  {W, K, Y, Y, K, K, W, K, Y, K, K, W, K, Y, Y, K},
  {W, K, Y, Y, K, W, K, K, Y, K, W, K, K, Y, Y, K},
  {W, K, Y, Y, B, B, K, K, Y, K, K, B, B, Y, Y, K},
  {W, K, Y, B, B, Y, Y, Y, Y, Y, Y, Y, B, B, Y, K},
  {W, W, K, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, K, W},
  {W, W, K, Y, Y, Y, Y, K, K, K, Y, Y, Y, Y, K, W},
  {W, W, W, K, Y, Y, Y, Y, Y, Y, Y, Y, Y, K, W, W},
  {W, W, W, W, K, K, Y, Y, Y, Y, Y, K, K, W, W, W},
  {W, W, W, W, W, W, K, K, K, K, K, W, W, W, W, W},
  {W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W}
};
for(int y = 0; y < MATRIX_HEIGHT; y++) {
    for(int x = 0; x < MATRIX_WIDTH; x++) {
      int index = getIndex(x, y);  // 获取LED索引
      if(index >= 0 && index < NUM_LEDS) {  // 确保索引有效
        leds[index] = pattern[y][x];  // 将颜色值写入LED
      }
    }
  }
}
  

void Sunglass() {
 const CRGB pattern[16][16] = {
  {W, W, W, W, W, W, K, K, K, K, K, W, W, W, W, W},
  {W, W, W, W, K, K, Y, Y, Y, Y, Y, K, K, W, W, W},
  {W, W, W, K, Y, Y, Y, Y, Y, Y, Y, Y, Y, K, W, W},
  {W, W, K, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, K, W},
  {W, W, K, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, K, W},
  {W, K, K, K, K, K, K, K, K, K, K, K, K, K, K, K},
  {W, K, Y, K, W, W, K, K, K, K, W, W, K, K, Y, K},
  {W, K, Y, K, W, K, K, K, Y, K, W, K, K, K, Y, K},
  {W, K, Y, Y, K, K, K, Y, Y, Y, K, K, K, Y, Y, K},
  {W, K, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, K},
  {W, W, K, Y, Y, Y, Y, Y, Y, Y, Y, K, Y, Y, K, W},
  {W, W, K, Y, Y, Y, Y, K, K, K, K, Y, Y, Y, K, W},
  {W, W, W, K, Y, Y, Y, Y, Y, Y, Y, Y, Y, K, W, W},
  {W, W, W, W, K, K, Y, Y, Y, Y, Y, K, K, W, W, W},
  {W, W, W, W, W, W, K, K, K, K, K, W, W, W, W, W},
  {W, W, W, W, W, W, W, W, W, W, W, W, W, W, W, W}
};
for(int y = 0; y < MATRIX_HEIGHT; y++) {
    for(int x = 0; x < MATRIX_WIDTH; x++) {
      int index = getIndex(x, y);  // 获取LED索引
      if(index >= 0 && index < NUM_LEDS) {  // 确保索引有效
        leds[index] = pattern[y][x];  // 将颜色值写入LED
      }
    }
  }
}



void Bow() {
  const CRGB pattern[16][16] = {    
    {W, W, W, W, W, W, K, K, K, K, K, W, W, W, W, W},        
    {W, W, W, W, K, K, Y, Y, Y, Y, Y, K, R, W, R, W},        
    {W, R, W, R, Y, Y, Y, Y, Y, Y, Y, R, R, R, R, R},        
    {R, R, R, R, R, Y, Y, Y, Y, Y, Y, Y, R, R, R, W},        
    {W, R, R, R, Y, Y, Y, Y, Y, Y, Y, Y, Y, R, K, W},        
    {W, K, R, Y, K, K, K, Y, Y, Y, K, K, K, Y, Y, K},        
    {W, K, Y, Y, K, Y, K, Y, Y, Y, K, Y, K, Y, Y, K},        
    {W, K, Y, K, Y, Y, Y, K, Y, K, Y, Y, Y, K, Y, K},        
    {W, K, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, K},        
    {W, K, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, Y, K},        
    {W, W, K, Y, Y, Y, K, Y, Y, Y, K, Y, Y, Y, K, W},        
    {W, W, K, Y, Y, Y, Y, K, K, K, Y, Y, Y, Y, K, W},        
    {W, W, W, K, Y, Y, Y, Y, Y, Y, R, Y, R, K, W, W},        
    {W, W, W, W, K, K, Y, Y, Y, R, R, R, R, R, W, W},        
    {W, W, W, W, W, W, K, K, K, K, R, R, R, W, W, W},        
    {W, W, W, W, W, W, W, W, W, W, W, R, W, W, W, W}
};
for(int y = 0; y < MATRIX_HEIGHT; y++) {
    for(int x = 0; x < MATRIX_WIDTH; x++) {
      int index = getIndex(x, y);  // 获取LED索引
      if(index >= 0 && index < NUM_LEDS) {  // 确保索引有效
        leds[index] = pattern[y][x];  // 将颜色值写入LED
      }
    }
  }
}


int spiralPhase = 0; // 动画相位变量

// 修复后的螺旋动画函数
void drawSpiral() {
  fill_solid(leds, NUM_LEDS, CRGB::Black);  // 清空屏幕
  
  const int centerX = 7, centerY = 7; // 中心坐标
  const int maxRadius = 8;            // 最大半径
  
  // 绘制螺旋
  for (int r = 0; r < maxRadius; r++) {
    for (int i = 0; i < 36; i++) {
      // 计算螺旋角度和坐标
      float angle = (i * 10 + r * 30 + spiralPhase) * PI / 180.0;
      int x = centerX + cos(angle) * r;
      int y = centerY + sin(angle) * r;
      
      // 计算索引并设置颜色（修复了取模运算错误）
      if (x >= 0 && x < 16 && y >= 0 && y < 16) {
        int index = getIndex(x, y);  // 使用已有的坐标转换函数
        // 关键修复：先将浮点数转换为整数再取模
        leds[index] = CHSV(((int)(angle * 57 + r * 10)) % 256, 255, 255);
      }
    }
  }
  
  spiralPhase = (spiralPhase + 2) % 360;  // 更新相位
  FastLED.show();
  delay(50);
}






void lightUpLed(int x, int y, CRGB color) {
  int index = getIndex(x, y);  // 获取索引
  if (index != -1) {  // 索引有效时才设置颜色
    leds[index] = color;
  }
}
  
 

// 将(x,y)坐标转换为LED索引
int getIndex(int x, int y) {
  // 根据点阵屏的实际排列方式调整索引计算
  // 这里假设是从左上角开始，蛇形排列
  
  // 如果是偶数行，从左到右；奇数行，从右到左
  if(y % 2 == 0) {
    return y * MATRIX_WIDTH + x;
  } else {
    return y * MATRIX_WIDTH + (MATRIX_WIDTH - 1 - x);
  }
}
#endif