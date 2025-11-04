#include <FastLED.h>
#include <wave.h>

WaveBreathEffect waveEffect(MATRIX_WIDTH, MATRIX_HEIGHT, leds);
void CurrentState();
enum State {
    STATE_START,
    STATE_SMILE,
    STATE_SAD,
    STATE_CRY,
    STATE_SUN,
    STATE_BOW
};

State currentState = STATE_START;


void CurrentState() {
   
    
    switch(currentState) 
    {
        case STATE_START:

            waveEffect.update();
            FastLED.show();
    
            break;
            
        case STATE_SMILE:
            FastLED.setBrightness(5);
            FastLED.show();
            Smile();
        
            break;
            
        case STATE_SAD:
            sad();
            FastLED.show();
            break;
            
        case STATE_CRY:
            FastLED.setBrightness(5);
            Crying();
            FastLED.show();
            break;
            
        case STATE_SUN:
            FastLED.setBrightness(5);
            Sunglass();
            FastLED.show();
            break;
            
        case STATE_BOW:
            FastLED.setBrightness(5);
            Bow();
            FastLED.show();
            break;
            
        default:
            break;
    }
}
