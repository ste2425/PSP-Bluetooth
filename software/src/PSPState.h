#ifndef PSPSTATE_H
#define PSPSTATE_H

#include "Adafruit_MCP23008.h"
#include <Arduino.h>
#include "digipot.h"

// TODO REMOVE THIS CURRENT CONSOLIZER VERSION DOES NOT USE THIS
#define MCPGND 16
#define ANALOGTOGGLE 4

// Enum used when want to trigger button press
enum GPIOPins {
  Right,
  Circle,
  Cross,
  Power,
  Triangle,
  R1,
  Square,
  L1,
  Up,
  Left,
  Down,
  Sound,
  Display,
  Volup,
  Voldown,
  Home,
  Start,
  Select
};

void PSPState_loop();
void PSPState_setup();
void PSPState_markAllReleased();
void PSPState_markButtonAsPressed(uint8_t button);
void PSPState_markButtonAsReleased(uint8_t button);
void PSPState_analog(int valueX, int valueY);
void PSPState_analogX(int value);
void PSPState_analogY(int value);
void PSPState_togglePower();
void PSPState_pressScreen();
void PSPState_releaseScreen();
bool PSPstate_poweredOn();

#endif
