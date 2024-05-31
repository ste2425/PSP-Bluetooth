#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>
#include "constants.h"
#include <Preferences.h>

enum GPIOTypes { 
    PSPDPadUp, PSPDPadDown, PSPDPadLeft, PSPDPadRight,
    PSPCross, PSPTriangle, PSPSquare, PSPCircle,
    PSPL1, PSPR1, PSPStart, PSPSelect, PSPHome, PSPScreen,
    BTNModeChange
};

struct GPIOPinConfig {
    uint8_t DPadUp;
    uint8_t DPadDown;
    uint8_t DpadLeft;
    uint8_t DpadRight;
    uint8_t Cross;
    uint8_t Triangle;
    uint8_t Square;
    uint8_t circle;
    uint8_t L1;
    uint8_t R1;
    uint8_t Start;
    uint8_t Select;
    uint8_t Home;
    uint8_t Screen;
    uint8_t ModeChange;
};

extern uint8_t defaultGPIO[15];

GPIOPinConfig gpioCfg_getConfig();
uint8_t gpioCfg_getPin(GPIOTypes type);
void gpioCfg_setPin(GPIOTypes type, uint8_t value);

#endif