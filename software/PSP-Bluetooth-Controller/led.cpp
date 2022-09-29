#include "led.h"

const uint8_t LED_PIN = 1;
const uint8_t LED_FAST_BLINK = 250;
const uint16_t LED_SLOW_BLINK = 1000;
Timer LEDTimer;

void LED_setup() {
  pinMode(LED_PIN, OUTPUT);

  LEDTimer.setCallback([]() -> void {
    uint8_t val = digitalRead(LED_PIN);
    digitalWrite(LED_PIN, !val);
  });
}

void LED_blinkFast() {
  LEDTimer.setInterval(LED_FAST_BLINK);
  LEDTimer.start();
}

void LED_blinkSlow() {
  LEDTimer.setInterval(LED_SLOW_BLINK);
  LEDTimer.start();
}

void LED_solid() {
  LEDTimer.stop();
  digitalWrite(LED_PIN, HIGH);
}

void LED_off() {
  LEDTimer.stop();
  digitalWrite(LED_PIN, LOW);
}
