#ifndef FORM_H  
#define FORM_H
#include <FastLED.h>
#define MATRIX_WIDTH 16
#define MATRIX_HEIGHT 16
#define NUM_LEDS 256       // 16×16点阵 = 256个LED
#define DATA_PIN 2         // 更换为GPIO 2，这是ESP32上的有效引脚
int getIndex(int x, int y);

extern const CRGB Purple = CRGB(128, 0, 128);    // 紫色轮廓
const CRGB Yellow = CRGB(255, 255, 0);    // 黄色填充
const CRGB Blue = CRGB(0, 0, 255);        // 蓝色元素
const CRGB Red = CRGB(255, 0, 0);         // 红色装饰
const CRGB Black = CRGB(0, 0, 0);         // 黑色细节
const CRGB White = CRGB(255, 255, 255);
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
  {
     Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow,
     Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow},
    {Yellow, Yellow, Yellow, Yellow, Yellow, Purple, Purple, Purple,
     Purple, Purple, Purple, Yellow, Yellow, Yellow, Yellow, Yellow},
     
    {Yellow, Yellow, Yellow, Yellow, Purple, Yellow, Yellow, Yellow,
     Yellow, Yellow, Yellow, Purple, Yellow, Yellow, Yellow, Yellow},
   
    {Yellow, Yellow, Yellow, Purple, Yellow, Yellow, Yellow, Yellow,
     Yellow, Yellow, Yellow, Yellow, Purple, Yellow, Yellow, Yellow},

    {Yellow, Yellow, Purple, Yellow, Yellow, Yellow, Yellow, Yellow,
     Yellow, Yellow, Yellow, Yellow, Yellow, Purple, Yellow, Yellow},

    {Yellow, Purple, Yellow, Yellow, Yellow, Purple, Yellow, Yellow,
     Yellow, Yellow, Purple, Yellow, Yellow, Yellow, Purple, Yellow},

    {Yellow, Purple, Yellow, Yellow, Yellow, Purple, Yellow, Yellow,
     Yellow, Yellow, Purple, Yellow, Yellow, Yellow, Purple, Yellow},

    {Yellow, Purple, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow,
     Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Purple, Yellow},

    {Yellow, Purple, Yellow, Yellow, Purple, Yellow, Yellow, Yellow,
     Yellow, Yellow, Yellow, Yellow, Purple, Yellow, Yellow, Purple},

    {Yellow, Purple, Yellow, Yellow, Yellow, Purple, Yellow, Yellow,
     Yellow, Yellow, Purple, Yellow, Yellow, Yellow, Purple, Yellow},

    {Yellow, Purple, Yellow, Yellow, Yellow, Yellow, Purple, Purple,
     Purple, Purple, Yellow, Yellow, Yellow, Yellow, Purple, Yellow},

    {Yellow, Yellow, Purple, Yellow, Yellow, Yellow, Yellow, Yellow,
     Yellow, Yellow, Yellow, Yellow, Yellow, Purple, Yellow, Yellow},

    {Yellow, Yellow, Yellow, Purple, Yellow, Yellow, Yellow, Yellow,
     Yellow, Yellow, Yellow, Yellow, Purple, Yellow, Yellow, Yellow},

    {Yellow, Yellow, Yellow, Yellow, Purple, Yellow, Yellow, Yellow,
     Yellow, Yellow, Yellow, Purple, Yellow, Yellow, Yellow, Yellow},

    {Yellow, Yellow, Yellow, Yellow, Yellow, Purple, Purple, Purple,
     Purple, Purple, Purple, Yellow, Yellow, Yellow, Yellow, Yellow},

    {Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow,
     Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow}
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
    // 第0行：全白
   {White, White, White, White, White, White, White, White, White, White, White, White, White, White, White, White},
   // 第 1 行：白、白、白、白、白、蓝、蓝、蓝、蓝、蓝、蓝、白、白、白、白、白
   {White, White, White, White, White, Blue, Blue, Blue, Blue, Blue, Blue, White, White, White, White, White},// 第 2 行：白、白、白、白、蓝、黄、黄、黄、黄、黄、黄、蓝、白、白、白、白{White, White, White, White, Blue, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Blue, White, White, White, White},
   // 第 3 行：白、白、白、蓝、黄、黄、黄、黄、黄、黄、黄、黄、蓝、白、白、白
   {White, White, White, Blue, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Blue, White, White, White},
   // 第 4 行：白、白、蓝、黄、黄、黄、黄、黄、黄、黄、黄、黄、黄、蓝、白、白
   {White, White, Blue, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Blue, White, White},// 第 5 行：白、蓝、黄、黄、蓝、黄、黄、黄、黄、蓝、黄、黄、黄、蓝、白、白
   {White, Blue, Yellow, Yellow, Blue, Yellow, Yellow, Yellow, Yellow, Blue, Yellow, Yellow, Yellow, Blue, White, White},
   // 第 6 行：白、蓝、黄、黄、蓝、黄、黄、黄、黄、蓝、黄、黄、黄、蓝、白、白
   {White, Blue, Yellow, Yellow, Blue, Yellow, Yellow, Yellow, Yellow, Blue, Yellow, Yellow, Yellow, Blue, White, White},
   // 第 7 行：白、蓝、黄、黄、黄、黄、黄、黄、黄、黄、黄、黄、黄、蓝、白、白
   {White, Blue, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Blue, White, White},
   // 第 8 行：白、蓝、黄、黄、黄、黄、蓝、蓝、蓝、蓝、黄、黄、黄、蓝、白、白
   {White, Blue, Yellow, Yellow, Yellow, Yellow, Blue, Blue, Blue, Blue, Yellow, Yellow, Yellow, Blue, White, White},// 第 9 行：白、蓝、黄、黄、蓝、黄、黄、黄、黄、蓝、黄、黄、黄、蓝、白、白{White, Blue, Yellow, Yellow, Blue, Yellow, Yellow, Yellow, Yellow, Yellow, Blue, Yellow, Yellow, Blue, White, White},
   // 第 10 行：白、蓝、黄、黄、黄、黄、黄、黄、黄、黄、黄、黄、黄、蓝、白、白
   {White, Blue, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Blue, White, White},
   // 第 11 行：白、白、蓝、黄、黄、黄、黄、黄、黄、黄、黄、黄、蓝、白、白、白
   {White, White, Blue, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Blue, White, White, White},// 第 12 行：白、白、白、蓝、黄、黄、黄、黄、黄、黄、黄、蓝、白、白、白、白
   {White, White, White, Blue, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Blue, White, White, White, White},
   // 第 13 行：白、白、白、白、蓝、黄、黄、黄、黄、黄、黄、蓝、白、白、白、白
   {White, White, White, White, Blue, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Blue, White, White, White, White},
   // 第 14 行：白、白、白、白、白、蓝、蓝、蓝、蓝、蓝、蓝、白、白、白、白、白
   {White, White, White, White, White, Blue, Blue, Blue, Blue, Blue, Blue, White, White, White, White, White},
   // 第 15 行：全白
   {White, White, White, White, White, White, White, White, White, White, White, White, White, White, White, White}
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
   // 第0行：白白白白白白黑黑黑黑黑白白白白白
{White, White, White, White, White, White, Black, Black, Black, Black, Black, White, White, White, White, White},
// 第1行：白白白白黑黑黄黄黄黄黄黑黑白白白
{White, White, White, White, Black, Black, Yellow, Yellow, Yellow, Yellow, Yellow, Black, Black, White, White, White},
// 第2行：白白白黑黄黄黄黄黄黄黄黄黄黑白白
{White, White, White, Black, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Black, White, White},
// 第3行：白白黑黄黄黄黄黄黄黄黄黄黄黄黑白
{White, White, Black, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Black, White},
// 第4行：白白黑黄黄黄黄黄黄黄黄黄黄黄黑白
{White, White, Black, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Black, White},
// 第5行：白黑黄黄黑黑黑黑黄黑黑黑黑黄黄黑
{White, Black, Yellow, Yellow, Black, Black, Black, Black, Yellow, Black, Black, Black, Black, Yellow, Yellow, Black},
// 第6行：白黑黄黄黑黑白黑黄黑黑白黑黄黄黑
{White, Black, Yellow, Yellow, Black, Black, White, Black, Yellow, Black, Black, White, Black, Yellow, Yellow, Black},
// 第7行：白黑黄黄黑白黑黑黄黑白黑黑黄黄黑
{White, Black, Yellow, Yellow, Black, White, Black, Black, Yellow, Black, White, Black, Black, Yellow, Yellow, Black},
// 第8行：白黑黄黄蓝蓝黑黑黄黑黑蓝蓝黄黄黑
{White, Black, Yellow, Yellow, Blue, Blue, Black, Black, Yellow, Black, Black, Blue, Blue, Yellow, Yellow, Black},
// 第9行：白黑黄蓝蓝黄黄黄黄黄黄黄蓝蓝黄黑
{White, Black, Yellow, Blue, Blue, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Blue, Blue, Yellow, Black},
// 第10行：白白黑黄黄黄黄黄黄黄黄黄黄黄黑白
{White, White, Black, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Black, White},
// 第11行：白白黑黄黄黄黄黑黑黑黄黄黄黄黑白
{White, White, Black, Yellow, Yellow, Yellow, Yellow, Black, Black, Black, Yellow, Yellow, Yellow, Yellow, Black, White},
// 第12行：白白白黑黄黄黄黄黄黄黄黄黄黑白白
{White, White, White, Black, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Black, White, White},
// 第13行：白白白白黑黑黄黄黄黄黄黑黑白白白
{White, White, White, White, Black, Black, Yellow, Yellow, Yellow, Yellow, Yellow, Black, Black, White, White, White},
// 第14行：白白白白白白黑黑黑黑黑白白白白白
{White, White, White, White, White, White, Black, Black, Black, Black, Black, White, White, White, White, White},
// 第15行：白白白白白白白白白白白白白白白白
{White, White, White, White, White, White, White, White, White, White, White, White, White, White, White, White}
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
    // 第1行: 白白白白白白黑黑黑黑黑白白白白白
    {White, White, White, White, White, White, Black, Black, Black, Black, Black, White, White, White, White, White},
    // 第2行: 白白白白黑黑黄黄黄黄黄黑黑白白白
    {White, White, White, White, Black, Black, Yellow, Yellow, Yellow, Yellow, Yellow, Black, Black, White, White, White},
    // 第3行: 白白白黑黄黄黄黄黄黄黄黄黄黑白白
    {White, White, White, Black, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Black, White, White},
    // 第4行: 白白黑黄黄黄黄黄黄黄黄黄黄黄黑白
    {White, White, Black, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Black, White},
    // 第5行: 白白黑黄黄黄黄黄黄黄黄黄黄黄黑白
    {White, White, Black, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Black, White},
    // 第6行: 白黑黑黑黑黑黑黑黑黑黑黑黑黑黑黑
    {White, Black, Black, Black, Black, Black, Black, Black, Black, Black, Black, Black, Black, Black, Black, Black},
    // 第7行: 白黑黄黑白白黑黑黑黑白白黑黑黄黑
    {White, Black, Yellow, Black, White, White, Black, Black, Black, Black, White, White, Black, Black, Yellow, Black},
    // 第8行: 白黑黄黑白黑黑黑黄黑白黑黑黑黄黑
    {White, Black, Yellow, Black, White, Black, Black, Black, Yellow, Black, White, Black, Black, Black, Yellow, Black},
    // 第9行: 白黑黄黄黑黑黑黄黄黄黑黑黑黄黄黑
    {White, Black, Yellow, Yellow, Black, Black, Black, Yellow, Yellow, Yellow, Black, Black, Black, Yellow, Yellow, Black},
    // 第10行: 白黑黄黄黄黄黄黄黄黄黄黄黄黄黄黑
    {White, Black, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Black},
    // 第11行: 白白黑黄黄黄黄黄黄黄黄黑黄黄黑白
    {White, White, Black, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Black, Yellow, Yellow, Black, White},
    // 第12行: 白白黑黄黄黄黄黑黑黑黑黄黄黄黑白
    {White, White, Black, Yellow, Yellow, Yellow, Yellow, Black, Black, Black, Black, Yellow, Yellow, Yellow, Black, White},
    // 第13行: 白白白黑黄黄黄黄黄黄黄黄黄黑白白
    {White, White, White, Black, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Black, White, White},
    // 第14行: 白白白白黑黑黄黄黄黄黄黑黑白白白
    {White, White, White, White, Black, Black, Yellow, Yellow, Yellow, Yellow, Yellow, Black, Black, White, White, White},
    // 第15行: 白白白白白白黑黑黑黑黑白白白白白
    {White, White, White, White, White, White, Black, Black, Black, Black, Black, White, White, White, White, White},
    // 第16行: 白白白白白白白白白白白白白白白白
    {White, White, White, White, White, White, White, White, White, White, White, White, White, White, White, White}
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
// 第0行：白白白白白白黑黑黑黑黑白白白白白
    {White, White, White, White, White, White, Black, Black, Black, Black, Black, White, White, White, White, White},        
// 第1行：白白白白黑黑黄黄黄黄黄黑红白红白
    {White, White, White, White, Black, Black, Yellow, Yellow, Yellow, Yellow, Yellow, Black, Red, White, Red, White},        
// 第2行：白红白红黄黄黄黄黄黄黄红红红红红
    {White, Red, White, Red, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Red, Red, Red, Red, Red},        
// 第3行：红红红红红黄黄黄黄黄黄黄红红红白
    {Red, Red, Red, Red, Red, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Red, Red, Red, White},        
// 第4行：白红红红黄黄黄黄黄黄黄黄黄红黑白
    {White, Red, Red, Red, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Red, Black, White},        
// 第5行：白黑红黄黑黑黑黄黄黄黑黑黑黄黄黑
    {White, Black, Red, Yellow, Black, Black, Black, Yellow, Yellow, Yellow, Black, Black, Black, Yellow, Yellow, Black},        
// 第6行：白黑黄黄黑黄黑黄黄黄黑黄黑黄黄黑
    {White, Black, Yellow, Yellow, Black, Yellow, Black, Yellow, Yellow, Yellow, Black, Yellow, Black, Yellow, Yellow, Black},        
// 第7行：白黑黄黑黄黄黄黑黄黑黄黄黄黑黄黑
    {White, Black, Yellow, Black, Yellow, Yellow, Yellow, Black, Yellow, Black, Yellow, Yellow, Yellow, Black, Yellow, Black},        
// 第8行：白黑黄黄黄黄黄黄黄黄黄黄黄黄黄黑
    {White, Black, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Black},        
// 第9行：白黑黄黄黄黄黄黄黄黄黄黄黄黄黄黑
    {White, Black, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Black},        
// 第10行：白白黑黄黄黄黑黄黄黄黑黄黄黄黑白
    {White, White, Black, Yellow, Yellow, Yellow, Black, Yellow, Yellow, Yellow, Black, Yellow, Yellow, Yellow, Black, White},        
// 第11行：白白黑黄黄黄黄黑黑黑黄黄黄黄黑白
    {White, White, Black, Yellow, Yellow, Yellow, Yellow, Black, Black, Black, Yellow, Yellow, Yellow, Yellow, Black, White},        
// 第12行：白白白黑黄黄黄黄黄黄红黄红黑白白
    {White, White, White, Black, Yellow, Yellow, Yellow, Yellow, Yellow, Yellow, Red, Yellow, Red, Black, White, White},        
// 第13行：白白白白黑黑黄黄黄红红红红红白白
    {White, White, White, White, Black, Black, Yellow, Yellow, Yellow, Red, Red, Red, Red, Red, White, White},        
// 第14行：白白白白白白黑黑黑黑红红红白白白
    {White, White, White, White, White, White, Black, Black, Black, Black, Red, Red, Red, White, White, White},        
// 第15行：白白白白白白白白白白白红白白白白
    {White, White, White, White, White, White, White, White, White, White, White, Red, White, White, White, White}
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