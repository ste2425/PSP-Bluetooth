#pragma once
#include <Arduino.h>
#include "bt_service.h"
#include "Mappings.h"
#include <Bluepad32.h>
#include "controllerManager.h"
#include "timeout.h"
#include "led.h"
#include "Settings.h"

bool INTEROP_bleServiceEnabled();
void INTEROP_enableBLEService(bool enabled);
void INTEROP_toggleBLEService();
void INTEROP_reloadControllerMappings();
void INTEROP_enableBLEInactivityWatcher();
void INTEROP_disableBLEInactivityWatcher();
void INTEROP_prepareOTAApply();
void INTEROP_reloadSettings();
void INTEROP_setup();
void INTEROP_loop();