#include <FastLED.h>
#include <emoji.h>
#include <wave.h>

WaveBreathEffect waveEffect(MATRIX_WIDTH, MATRIX_HEIGHT, leds);

void executeCurrentState();
enum State {
    INIT,
    STATE_START,
    STATE_SMILE,
    STATE_SAD,
    STATE_CRY,
    STATE_SUN,
    STATE_BOW
};
enum SubState {
    SUB_FIRST,    // 执行第一个函数
    SUB_SECOND    // 执行并保持第二个函数
};
State currentState = INIT;
SubState currentSubState = SUB_FIRST;
unsigned long subStateStartTime = 0;  // 记录子状态开始时间
void executeCurrentState() {
    unsigned long currentTime = millis();
    
    switch(currentState) {
        case STATE_START:
           
            if(currentSubState == SUB_FIRST) {
               FastLED.setBrightness(5);
                colorWipe(CRGB::DarkBlue, 100);
                // 检查第一个函数是否完成（假设1000ms后完成）
                if(currentTime - subStateStartTime >= 5000) {
                    currentSubState = SUB_SECOND;
                    subStateStartTime = currentTime;
                    FastLED.clear();
                }
            } else {
                // 保持在第二个函数
               waveEffect.update();
               FastLED.show();
            }
            break;
            
        case STATE_SMILE:
            FastLED.setBrightness(5);
            if(currentSubState == SUB_FIRST) {
                blinkAll(CRGB::Orange, 100);
                // 第一个函数执行2秒后切换
                if(currentTime - subStateStartTime >= 2000) {
                    currentSubState = SUB_SECOND;
                    subStateStartTime = currentTime;
                    FastLED.clear();
                }
            } else {
                // 保持笑脸图案
                Smile();
            }
            break;
            
        case STATE_SAD:
          FastLED.setBrightness(5);
            if(currentSubState == SUB_FIRST) {
                blinkAll(CRGB::Magenta, 100);
                if(currentTime - subStateStartTime >= 2000) {
                    currentSubState = SUB_SECOND;
                    subStateStartTime = currentTime;
                    FastLED.clear();
                }
            } else {
                sad();
            }
            break;
            
        case STATE_CRY:
             FastLED.setBrightness(5);
            if(currentSubState == SUB_FIRST) {
                drawCross(CRGB::DarkRed, 100);
                if(currentTime - subStateStartTime >= 1500) {
                    currentSubState = SUB_SECOND;
                    subStateStartTime = currentTime;
                    FastLED.clear();
                }
            } else {
                Crying();
            }
            break;
            
        case STATE_SUN:
            FastLED.setBrightness(5);
            if(currentSubState == SUB_FIRST) {
                drawCheckerboard(CRGB::Yellow, CRGB::Purple, 100);
                if(currentTime - subStateStartTime >= 2000) {
                    currentSubState = SUB_SECOND;
                    subStateStartTime = currentTime;
                    FastLED.clear();
                }
            } else {
                Sunglass();
            }
            break;
            
        case STATE_BOW:
            FastLED.setBrightness(5);
            if(currentSubState == SUB_FIRST) {
                drawSpiral();
                if(currentTime - subStateStartTime >= 3000) {
                    currentSubState = SUB_SECOND;
                    subStateStartTime = currentTime;
                    FastLED.clear();
                }
            } else {
                Bow();
            }
            break;
            
        default:
            break;
    }
}
