#ifndef Button_H
#define Button_H

#include <Arduino.h>
#include <OneButton.h>
#include "controllerManager.h"
#include "interop.h"
#include "led.h"
#include "fileUtility.h"
#include "Settings.h"

#define BUTTON_PIN 19

void BUTTON_loop();
void BUTTON_setup();

#endif
