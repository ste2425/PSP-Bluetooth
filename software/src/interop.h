#pragma once
#include <Arduino.h>
#include "bt_service.h"
#include "Mappings.h"
#include <Bluepad32.h>
#include "controllerManager.h"

bool INTEROP_bleServiceEnabled();
void INTEROP_enableBLEService(bool enabled);
void INTEROP_toggleBLEService();
void reloadControllerMappings();