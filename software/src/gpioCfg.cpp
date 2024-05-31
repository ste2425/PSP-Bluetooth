#include "gpioCfg.h"

static Preferences preferences;

// Note not all ESP GPIO is safe to use, some pins have specific uses
// Others output a PWM signal on boot. These pins could be used with extra hardware
// to protect the PSP however, but for now they are avoided.
uint8_t defaultGPIO[15] = {
    4, //dpadup
    13, //dpaddown
    16, //dpadleft
    17, //dpadright
    18, //cross
    19, //triangle
    21, //square
    22, //circle
    23, //l1
    25, //r1
    26, //start
    27, //select
    32, //home
    33, //screen
    34 //Mode Change Btn - INPUT ONLY
};

GPIOPinConfig gpioCfg_getConfig()
{
    preferences.begin(GPIONamespace, false);

    GPIOPinConfig config;

    config.DPadUp = preferences.getChar((const char*)PSPDPadUp, defaultGPIO[PSPDPadUp]);
    config.DPadDown = preferences.getChar((const char*)PSPDPadDown, defaultGPIO[PSPDPadDown]);
    config.DpadLeft = preferences.getChar((const char*)PSPDPadLeft, defaultGPIO[PSPDPadLeft]);
    config.DpadRight = preferences.getChar((const char*)PSPDPadRight, defaultGPIO[PSPDPadRight]);
    config.Cross = preferences.getChar((const char*)PSPCross, defaultGPIO[PSPCross]);
    config.Triangle = preferences.getChar((const char*)PSPTriangle, defaultGPIO[PSPTriangle]);
    config.Square = preferences.getChar((const char*)PSPSquare, defaultGPIO[PSPSquare]);
    config.circle = preferences.getChar((const char*)PSPCircle, defaultGPIO[PSPCircle]);
    config.L1 = preferences.getChar((const char*)PSPL1, defaultGPIO[PSPL1]);
    config.R1 = preferences.getChar((const char*)PSPR1, defaultGPIO[PSPR1]);
    config.Start = preferences.getChar((const char*)PSPStart, defaultGPIO[PSPStart]);
    config.Select = preferences.getChar((const char*)PSPSelect, defaultGPIO[PSPSelect]);
    config.Home = preferences.getChar((const char*)PSPHome, defaultGPIO[PSPHome]);
    config.Screen = preferences.getChar((const char*)PSPScreen, defaultGPIO[PSPScreen]);
    config.ModeChange = preferences.getChar((const char*)BTNModeChange, defaultGPIO[BTNModeChange]);

    preferences.end();

    return config;
}

uint8_t gpioCfg_getPin(GPIOTypes type)
{
    preferences.begin(GPIONamespace, false);
    uint8_t value = preferences.getChar((const char*)type, defaultGPIO[type]);
    preferences.end();
    return value;
}

void gpioCfg_setPin(GPIOTypes type, uint8_t value)
{
    preferences.begin(GPIONamespace, false);
    preferences.putChar((const char*)type, value);
    preferences.end();
}