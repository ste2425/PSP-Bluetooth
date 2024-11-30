#pragma once

#include <Arduino.h>
#include <Bluepad32.h>
#include "led.h"
#include "PSPState.h"
#include "Mappings.h"
#include "timeout.h"
#include "interop.h"

void CTRMANAGER_loop();
void CTRMANAGER_setup();
void CTRMANAGER_enableConnections(bool enabled);
void CTRMANAGER_toggleConnections();
bool CTRMANAGER_newConnectionEnabled();
bool CTRMANAGER_applyColours();