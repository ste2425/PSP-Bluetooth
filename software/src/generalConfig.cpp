#include "generalCfg.h"

static Preferences preferences;

GeneralConfig generalCfg_getConfig()
{
    preferences.begin(GeneralNamespace, false);

    GeneralConfig config;

    config.BLEEnabled = preferences.getBool("BLEEnabled", false);

    preferences.end();

    return config;
}

void generalCfg_setConfig(GeneralConfig config)
{
    preferences.begin(GeneralNamespace, false);
    preferences.putBool("BLEEnabled", config.BLEEnabled);
    preferences.end();
}