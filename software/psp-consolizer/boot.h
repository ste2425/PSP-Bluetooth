#ifndef BOOT_H
#define BOOT_H

#include <Arduino.h>
#include <Bluepad32.h>
#include <timer.h>
#include "led.h"
#include "pspPins.h"
#include "variables.h"

void BOOT_powerOn();
void BOOT_powerOff();
bool BOOT_isBooting();
void BOOT_setup();

#endif
