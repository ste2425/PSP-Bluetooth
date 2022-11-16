#include "pspPins.h"

// Array indexes for referencing each PSP button
const uint8_t PSP_HOME = 0;
const uint8_t PSP_START = 1;
const uint8_t PSP_SELECT = 2;
const uint8_t PSP_DISPLAY = 3;
const uint8_t PSP_L1 = 4;
const uint8_t PSP_DUP = 5;
const uint8_t PSP_DDOWN = 6;
const uint8_t PSP_DRIGHT = 7;
const uint8_t PSP_DLEFT = 8;
const uint8_t PSP_R1 = 9;
const uint8_t PSP_CROSS = 10;
const uint8_t PSP_CIRCLE = 11;
const uint8_t PSP_SQUARE = 12;
const uint8_t PSP_TRIANGLE = 13;
const uint8_t PSP_POWER = 14;
const uint8_t PSP_LS_LEFT = 15;
const uint8_t PSP_LS_RIGHT = 16;
const uint8_t PSP_LS_UP = 17;
const uint8_t PSP_LS_DOWN = 18;
const uint8_t PSP_LS = 19;
const uint8_t PSP_VOL_DOWN = 20;
const uint8_t PSP_VOL_UP = 21;

const uint8_t pinCount = 16;
bool pspPoweredOn = false;

bool XAxisToBeReset = false;
bool YAxisToBeReset = false;

// Lists the physical Arduino pin for each PSP button
uint8_t pins[pinCount] = {
  17, // home
  14, // start
  15, // select
  16, // display
  6, //l1
  7, // dup
  9, //ddown
  18, // dright
  8, //dleft
  5, //r1
  2, //cross
  3, // circle
  19, // square
  4, // triangle,
  1, // vol down
  0, // vol up
};

// State tracking for each PSP button press
bool pinPressed[pinCount] = {
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  false,
  false,
};

void PSP_mark_all_for_release() {
  for(int i = 0; i < pinCount; i++) {
    if (i != PSP_POWER)
      pinPressed[i] = false;
  }

  XAxisToBeReset = true;
  YAxisToBeReset = true;
}

void PSP_release_unused() {
  for(int i = 0; i < pinCount; i++) {
    if (pinPressed[i] == false) {            
      pinMode(pins[i], INPUT);
    }
  }

  if (XAxisToBeReset) {
    DIGIPOT_write_x(127);
  }
  
  if (YAxisToBeReset) {
    DIGIPOT_write_y(127);
  }
}

bool isButtonLS(uint8_t pspButton) {
  return pspButton == PSP_LS_LEFT ||
    pspButton == PSP_LS_RIGHT ||
    pspButton == PSP_LS_UP ||
    pspButton == PSP_LS_DOWN;
}

void moveLS(uint8_t pspButton) {  
  switch(pspButton) {
    case PSP_LS_LEFT:
      XAxisToBeReset = false;
      DIGIPOT_write_x(50);
    break;
    case PSP_LS_RIGHT:
      XAxisToBeReset = false;
      DIGIPOT_write_x(200);
    break;
    case PSP_LS_UP:
      YAxisToBeReset = false;
      DIGIPOT_write_y(50);
    break;
    case PSP_LS_DOWN:
      YAxisToBeReset = false;
      DIGIPOT_write_y(200);
    break;
  }
}

void PSP_set_ls(uint8_t x, uint8_t y) {
      uint8_t xToSet = constrain(x, 50, 200);
      XAxisToBeReset = false;
      DIGIPOT_write_x(xToSet);
      
      uint8_t yToSet = constrain(y, 50, 200);
      YAxisToBeReset = false;
      DIGIPOT_write_y(yToSet);
}

void PSP_press_button (uint8_t pspButton) {
  if (isButtonLS(pspButton)) {
    moveLS(pspButton);
  } else {
    pinPressed[pspButton] = true;
      
    pinMode(pins[pspButton], OUTPUT);
    digitalWrite(pins[pspButton], LOW);
  }
}

bool coldBoot = true;

void toggle_power() {  
  pinMode(21, OUTPUT);
  digitalWrite(21, LOW);
  delay(500);
  pinMode(21, INPUT);
}

bool PSP_power_on() {
  bool isColdBoot = coldBoot;

  coldBoot = false;
  
  if (!pspPoweredOn) {
    pspPoweredOn = true;

    toggle_power();
  }

  return isColdBoot;
}

void PSP_power_off() {
  if (pspPoweredOn) {
    pspPoweredOn = false;

    toggle_power();
  }
}

void PSP_press_screen() {
  pinMode(pins[PSP_DISPLAY], OUTPUT);
  digitalWrite(pins[PSP_DISPLAY], LOW);
}

void PSP_release_screen() {
  pinMode(pins[PSP_DISPLAY], INPUT);
}

void PSP_toggle_screen(){  
  pinMode(pins[PSP_DISPLAY], OUTPUT);
  digitalWrite(pins[PSP_DISPLAY], LOW);
  delay(5000);
  pinMode(pins[PSP_DISPLAY], INPUT);
}

uint8_t PSP_map_controller(int controllerVal) {  
  /*
   * Controllers report range of -511 - 512
   * bring to 0 - 1023 then map to Digital pots acceptable range.
   * Digi pot accepts 0 - 257 however
   * for some reason PSP reports full range from a value of 50 - 200.
   */
   
  return map(controllerVal + 511, 0, 1023, 200, 50);
}
