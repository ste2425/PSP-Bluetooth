#ifndef LED_H
#define LED_H

#define LED 18
#include <ezLED.h>
#include <Arduino.h>

void LED_loop();
void LED_fadeSlow();
void LED_off();
void LED_on();

#endif
