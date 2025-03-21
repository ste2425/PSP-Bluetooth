#ifndef SETTINGS_H
#define SETTINGS_H

#define SETTINGS_DEFAULT "{ \"dblClickMode\": 0, \"lngClickMode\": 2, \"clickMode\": 1, \"shutdownDelay\": 3000, \"useAlternativeShutdown\": false, \"batteryWarning\": 38, \"warmBootDelay\": 5000, \"coldBootDelay\": 13000, \"screenDelay\": 7000, \"ledBrightness\": 255,  \"throttleThreshold\": 800, \"analogThreshold\": 300, \"diagonalThreshold\": 300, \"useExtraCombo\": true }"

#include "SettingsTypes.h"
#include <Arduino.h>
#include <LittleFS.h>
#include "fileUtility.h"

extern Settings SETTINGS_current;

void SETTINGS_setup();

#endif
