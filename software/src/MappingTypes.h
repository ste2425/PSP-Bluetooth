#ifndef MAPPINGTypes_H
#define MAPPINGTypes_H

#include <Arduino.h>

enum ControllerTypes {
  DPad,
  Misc,
  Button,
  Analog
};

struct ControllerMapping { // This structure is named "myDataType"
  int PSPButton;
  uint8_t ControllerBit;
  uint8_t ControllerType;
};

struct MappingMeta {
  bool isSet;
  ControllerMapping mappings[30];
  uint8_t mappingNumer;
  uint8_t colour[3];
};

#endif
