#ifndef LED_H
#define LED_H

#define LED 18
#include <Arduino.h>
#include "timeout.h"
#include "controllerManager.h"
#include "interop.h"
#include "PSPState.h"

void LED_bootPattern();
void LED_syncPattern();
void LED_blePattern();
void LED_off();
void LED_on();
void LED_setup();
void LED_loop();
void LED_autoSet();

#endif
