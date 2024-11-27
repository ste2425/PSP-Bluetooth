#include "led.h"

uint8_t currentBrightness = 0;
uint8_t targetBrightness = 0;
// how many steps the brighnes should change in
uint8_t stepSize = 0;
// how long to wait before bump next brightness step
uint8_t tickStepSize = 0;
// used to track time
uint32_t previousMillis = 0;

enum LEDMode {
  FadeSlow,
  Off
};

uint8_t ledMode = LEDMode::Off;

void LED_fadeSlow() {
  targetBrightness = 255;
  stepSize = 10;
  tickStepSize = 500;
}

void LED_off() {
  targetBrightness = 0;
  stepSize = 0;
  tickStepSize = 0;

  LED_loop();
}

void LED_on() {
  targetBrightness = 255;
  stepSize = 0;
  tickStepSize = 0;

  LED_loop();
}

void LED_loop() {  
  uint32_t currentMillis = millis();

  if (tickStepSize == 0 || currentMillis - previousMillis >= tickStepSize) {

    previousMillis = currentMillis;
    
    if (currentBrightness != targetBrightness) {

      // zero used to insidcate instant change
      if (stepSize == 0) {
        currentBrightness = targetBrightness;
      } else {
        // 8bit has max value of 255. Dont blindly add next step. otherwise couldnt end up wrapping back round to zero
        uint8_t diff = targetBrightness - currentBrightness;

        if (diff < stepSize)
          currentBrightness = targetBrightness;
        else 
          currentBrightness += stepSize;
      }
      
      analogWrite(LED, currentBrightness);
    }
  }
}
