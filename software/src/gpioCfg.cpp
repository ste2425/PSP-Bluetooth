#include "gpioCfg.h"

static Preferences preferences;

// Note not all ESP GPIO is safe to use, some pins have specific uses
// Others output a PWM signal on boot. These pins could be used with extra hardware
// to protect the PSP however, but for now they are avoided.
uint8_t defaultGPIO[15] = {
    4, //dpadup
    18, //dpaddown
    16, //dpadleft
    17, //dpadright
    13, //cross
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

    config.DPadUp = preferences.getChar("dpadup", defaultGPIO[PSPDPadUp]);
    config.DPadDown = preferences.getChar("dpaddown", defaultGPIO[PSPDPadDown]);
    config.DpadLeft = preferences.getChar("dpadleft", defaultGPIO[PSPDPadLeft]);
    config.DpadRight = preferences.getChar("dpadright", defaultGPIO[PSPDPadRight]);
    config.Cross = preferences.getChar("crossbtn", defaultGPIO[PSPCross]);
    config.Triangle = preferences.getChar("trianglebtn", defaultGPIO[PSPTriangle]);
    config.Square = preferences.getChar("squarebtn", defaultGPIO[PSPSquare]);
    config.circle = preferences.getChar("circlebtn", defaultGPIO[PSPCircle]);
    config.L1 = preferences.getChar("l1btn", defaultGPIO[PSPL1]);
    config.R1 = preferences.getChar("r1btn", defaultGPIO[PSPR1]);
    config.Start = preferences.getChar("startbtn", defaultGPIO[PSPStart]);
    config.Select = preferences.getChar("selectbtn", defaultGPIO[PSPSelect]);
    config.Home = preferences.getChar("homebtn", defaultGPIO[PSPHome]);
    config.Screen = preferences.getChar("screenbtn", defaultGPIO[PSPScreen]);
    config.ModeChange = preferences.getChar("modechangebtn", defaultGPIO[BTNModeChange]);

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