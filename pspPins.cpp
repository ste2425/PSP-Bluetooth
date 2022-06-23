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
const uint8_t PSP_LS = 19;


const uint8_t pinCount = 14;
bool pspPoweredOn = false;

bool XAxisToBeReset = false;
bool YAxisToBeReset = false;


//Proto
/*
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
*/

// PCB V2
uint8_t pins[pinCount] = {
  17, // home
  14, // start
  15, // select
  16, // display
  9, //l1
  7, // dup
  6, //ddown
  8, // dright
  18, //dleft
  5, //r1
  2, //cross
  3, // circle
  19, // square
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

void PSP_set_ls(uint8_t x, uint8_t y) {
      uint8_t xToSet = constrain(x, 50, 200);
      XAxisToBeReset = false;
      DIGIPOT_write_x(xToSet);
      
      uint8_t yToSet = constrain(y, 50, 200);
      YAxisToBeReset = false;
      DIGIPOT_write_y(yToSet);
}

void moveLS(uint8_t pspButton) {  
  switch(pspButton) {
    case PSP_LS_LEFT:
      Serial.println("Moving LS LEFT");
      XAxisToBeReset = false;
      DIGIPOT_write_x(50);
    break;
    case PSP_LS_RIGHT:
      Serial.println("Moving LS RIGHT");
      XAxisToBeReset = false;
      DIGIPOT_write_x(200);
    break;
    case PSP_LS_UP:
      Serial.println("Moving LS LEFT");
      YAxisToBeReset = false;
      DIGIPOT_write_y(50);
    break;
    case PSP_LS_DOWN:
      Serial.println("Moving LS RIGHT");
      YAxisToBeReset = false;
      DIGIPOT_write_y(200);
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

uint8_t PSP_map_controller(int controllerVal) {  
  /*
   * Controllers report range of -511 - 512
   * bring to 0 - 1023 then map to Digital pots acceptable range.
   * Digi pot accepts 0 - 257 however
   * for some reason PSP reports full range from a value of 50 - 200.
   */
   
  return map(controllerVal + 511, 0, 1023, 50, 200);
}
