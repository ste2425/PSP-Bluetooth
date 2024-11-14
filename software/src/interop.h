#pragma once
#include <Arduino.h>
#include "bt_service.h"
#include "Mappings.h"
#include <Bluepad32.h>

void enableBLEService(bool enabled);
void reloadControllerMappings();