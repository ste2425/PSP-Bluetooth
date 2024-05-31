#ifndef GCFG_H
#define GCFG_H

#include <stdint.h>
#include "constants.h"
#include <Preferences.h>

struct GeneralConfig {
    bool BLEEnabled;
};

extern uint8_t defaultGPIO[15];

GeneralConfig generalCfg_getConfig();
void generalCfg_setConfig(GeneralConfig config);

#endif