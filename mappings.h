#ifndef MAPPINGS_H
#define MAPPINGS_H

#include <Arduino.h>
#include "pspPins.h"

enum controllerType {
  TYPE_DPAD = 1,
  TYPE_BUTTONS = 2,
  TYPE_SYSTEM = 3,
  TYPE_TRIGGER = 4,
  TYPE_RS = 5,
  TYPE_LS = 6
};

struct pinMap {
  const byte pin;
  const int controllerMask;
  const controllerType type;
};

enum {
  DPAD_UP = 1 << 0,
  DPAD_DOWN = 1 << 1,
  DPAD_RIGHT = 1 << 2,
  DPAD_LEFT = 1 << 3,
};

enum {
  BUTTON_A = 1 << 0,
  BUTTON_B = 1 << 1,
  BUTTON_X = 1 << 2,
  BUTTON_Y = 1 << 3,
  BUTTON_SHOULDER_L = 1 << 4,
  BUTTON_SHOULDER_R = 1 << 5,
  BUTTON_TRIGGER_L = 1 << 6,
  BUTTON_TRIGGER_R = 1 << 7,
  BUTTON_THUMB_L = 1 << 8,
  BUTTON_THUMB_R = 1 << 9
};

enum {
  BUTTON_THROTTLE,
  BUTTON_BRAKE,
  BUTTON_RS_UP,
  BUTTON_RS_RIGHT,
  BUTTON_RS_LEFT,
  BUTTON_RS_DOWN,
  BUTTON_RS,
  BUTTON_LS
};

enum {
  MISC_BUTTON_SYSTEM = 1 << 0,  // AKA: PS, Xbox, etc.
  MISC_BUTTON_BACK = 1 << 1,    // AKA: Select, Share, -
  MISC_BUTTON_HOME = 1 << 2,    // AKA: Start, Options, +
};

extern const int defaultMappingsLength;
extern pinMap defaultMappings[];

extern const int FPSMappingsLength;
extern pinMap FPSMappings[];

extern const int PS1MappingsLength;
extern pinMap PS1Mappings[];

#endif
