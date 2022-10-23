#ifndef MAPPINGS_H
#define MAPPINGS_H

#include <Arduino.h>
#include "pspPins.h"
#include "controller.h"

struct pinMap {
  const byte pin;
  const int controllerMask;
  const CTR_accessorType type;
};

extern pinMap *MAPPINGS_pins;
extern uint8_t MAPPINGS_pin_size;
extern uint8_t MAPPINGS_colour[3];
extern uint8_t MAPPINGS_INDEX;

void MAPPINGS_next_mapping();

#endif
