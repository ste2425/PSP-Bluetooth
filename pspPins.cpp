#include "pspPins.h"

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

const uint8_t pinCount = 14;
bool pspPoweredOn = false;

bool XAxisToBeReset = false;

uint8_t pins[pinCount] = {
  14, // home
  17, // start
  16, // select
  15, // display
  19, //l1
  9, // dup
  7, //ddown
  18, // dright
  8, //dleft
  5, //r1
  2, //cross
  3, // circle
  6, // square
  4, // triangle
 // 21 // power
};

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
 // false,
};

void PSP_mark_all_for_release() {
  for(int i = 0; i < pinCount; i++) {
    if (i != PSP_POWER)
      pinPressed[i] = false;
  }

  XAxisToBeReset = true;
}

void PSP_release_unused() {
  for(int i = 0; i < pinCount; i++) {
    if (pinPressed[i] == false) {            
      pinMode(pins[i], INPUT);
    }
  }

  if (XAxisToBeReset) {
    POT_write_X(127);
    POT_write_Y(127);
  }
}

bool isButtonLS(uint8_t pspButton) {
  return pspButton == PSP_LS_LEFT ||
    pspButton == PSP_LS_RIGHT ||
    pspButton == PSP_LS_UP ||
    pspButton == PSP_LS_DOWN;
}

void moveLS(uint8_t pspButton) {
  XAxisToBeReset = false;
  
  switch(pspButton) {
    case PSP_LS_LEFT:
      Serial.println("Moving LS LEFT");
      POT_write_X(50);
    break;
    case PSP_LS_RIGHT:
      Serial.println("Moving LS RIGHT");
      POT_write_X(200);
    break;
    case PSP_LS_UP:
      Serial.println("Moving LS LEFT");
      POT_write_Y(50);
    break;
    case PSP_LS_DOWN:
      Serial.println("Moving LS RIGHT");
      POT_write_Y(200);
    break;
  }
}

void PSP_press_button (uint8_t pspButton) {
  if (isButtonLS(pspButton)) {
    moveLS(pspButton);
  } else {
    pinPressed[pspButton] = true;
  
    Serial.print("Pressing pin ");
    Serial.print(pins[pspButton]);
    Serial.println("");
    
    pinMode(pins[pspButton], OUTPUT);
    digitalWrite(pins[pspButton], LOW);
  }
}

void toggle_power() {
  Serial.println("Toggle Power");
  
  pinMode(21, OUTPUT);
  digitalWrite(21, LOW);
  delay(500);
  pinMode(21, INPUT);
}

void PSP_power_on() {
  if (!pspPoweredOn) {
    Serial.println("Powering on");
    pspPoweredOn = true;

    toggle_power();
  }
}

void PSP_power_off() {
  if (pspPoweredOn) {
    Serial.println("Powering off");
    pspPoweredOn = false;

    toggle_power();
  }
}

void PSP_toggle_screen(){
  Serial.println("Toggle screen");
  
  pinMode(pins[PSP_DISPLAY], OUTPUT);
  digitalWrite(pins[PSP_DISPLAY], LOW);
  delay(5000);
  pinMode(pins[PSP_DISPLAY], INPUT);
}
