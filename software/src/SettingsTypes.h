#ifndef SETTINGSTYPES_H
#define SETTINGSTYPES_H

#include <Arduino.h>

struct Settings {
  uint16_t ledBrightness;
  uint16_t throttleThreshold;
  uint16_t analogThreshold;
  uint16_t diagonalThreshold;
  uint16_t screenDelay;
  uint16_t coldBootDelay;
  uint16_t warmBootDelay;
  bool useExtraCombo;
};

#endif
