#ifndef SETTINGS_H
#define SETTINGS_H

#define SETTINGS_DEFAULT "{ \"shutdownDelay\": 3000, \"useAlternativeShutdown\": false, \"batteryWarning\": 38, \"warmBootDelay\": 2500, \"coldBootDelay\": 13000, \"screenDelay\": 7000, \"ledBrightness\": 255,  \"throttleThreshold\": 800, \"analogThreshold\": 300, \"diagonalThreshold\": 300, \"useExtraCombo\": true }"

#include "SettingsTypes.h"
#include <Arduino.h>
#include <LittleFS.h>
#include "fileUtility.h"

extern Settings SETTINGS_current;

void SETTINGS_setup();

#endif
