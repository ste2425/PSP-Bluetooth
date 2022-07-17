#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <Arduino.h>

enum CTR_accessorType {
  CTR_TYPE_DPAD = 1,
  CTR_TYPE_BUTTONS = 2,
  CTR_TYPE_SYSTEM = 3,
  CTR_TYPE_TRIGGER = 4,
  CTR_TYPE_RS = 5,
  CTR_TYPE_LS = 6,
};

enum CTR_dpad {
  CTR_DPAD_UP = 1 << 0,
  CTR_DPAD_DOWN = 1 << 1,
  CTR_DPAD_RIGHT = 1 << 2,
  CTR_DPAD_LEFT = 1 << 3,
};

enum CTR_buttons {
  CTR_BUTTON_A = 1 << 0,
  CTR_BUTTON_B = 1 << 1,
  CTR_BUTTON_X = 1 << 2,
  CTR_BUTTON_Y = 1 << 3,
  CTR_BUTTON_SHOULDER_L = 1 << 4,
  CTR_BUTTON_SHOULDER_R = 1 << 5,
  CTR_BUTTON_TRIGGER_L = 1 << 6,
  CTR_BUTTON_TRIGGER_R = 1 << 7,
  CTR_BUTTON_THUMB_L = 1 << 8,
  CTR_BUTTON_THUMB_R = 1 << 9,
};

enum CTR_thumbStick {
  CTR_BUTTON_TS,
  CTR_BUTTON_UP,
  CTR_BUTTON_DOWN,
  CTR_BUTTON_LEFT,
  CTR_BUTTON_RIGHT,
};

enum CTR_variableTriggers {
  CTR_BUTTON_THROTTLE,
  CTR_BUTTON_BRAKE,
};

enum CTR_miscButtons{
  CTR_MISC_BUTTON_SYSTEM = 1 << 0,  // xbox/ps4 button
  CTR_MISC_BUTTON_BACK = 1 << 1,    // select/share
  CTR_MISC_BUTTON_HOME = 1 << 2,    // start/options
};

#endif
