#include "led.h"

Timeout patternTimeout;

unsigned long bootPattern[2] = { 500, 2000 };
unsigned long syncPattern[4] = { 500, 500, 500, 2000 };
unsigned long blePattern[6] = { 500, 500, 500, 500, 500, 2000 };

uint8_t currentTick = 0;
bool repeat = false;

unsigned long *currentPattern = bootPattern;
uint8_t patternLength = 2;

bool lit = false;
bool run = false;

void onPatternTick() {
  if (currentTick >= patternLength && !repeat) {
    patternTimeout.stop();
    return;
  }

  currentTick++;

  if (currentTick >= patternLength) {
    currentTick = 0;
  }

  lit = !lit;

  if (lit) {
    analogWrite(LED, SETTINGS_current.ledBrightness);
  } else {
    analogWrite(LED, 0);
  }

  patternTimeout.setTimeout(currentPattern[currentTick]);
  patternTimeout.start();
}

void setPattern(unsigned long *pattern, uint8_t length, bool loop) {
  currentTick = 0;
  currentPattern = pattern;
  patternLength = length;
  lit = true;
  repeat = loop;
  run = true;

  analogWrite(LED, SETTINGS_current.ledBrightness);
}

void LED_bootPattern() {
  setPattern(bootPattern, 2, true);
    
  patternTimeout.setTimeout(currentPattern[currentTick]);
  patternTimeout.start();
}

void LED_syncPattern() {
  setPattern(syncPattern, 4, true);
  
  analogWrite(LED, SETTINGS_current.ledBrightness);

  patternTimeout.setTimeout(currentPattern[currentTick]);
  patternTimeout.start();
}

void LED_blePattern() {
  setPattern(blePattern, 6, true);
  
  analogWrite(LED, SETTINGS_current.ledBrightness);

  patternTimeout.setTimeout(currentPattern[currentTick]);
  patternTimeout.start();
}

void LED_off() {
  patternTimeout.stop();
  currentTick = 0;

  run = false;

  analogWrite(LED, 0);
}

void LED_on() {
  patternTimeout.stop();
  currentTick = 0;

  run = false;

  analogWrite(LED, SETTINGS_current.ledBrightness);
}

void LED_loop() {
  patternTimeout.loop();
}

void LED_autoSet() {  
  if(CTRMANAGER_pspBooting()) {
    LED_bootPattern();
  } else if (CTRMANAGER_newConnectionEnabled()) {
    LED_syncPattern();
  } else if (INTEROP_bleServiceEnabled()) {
    LED_blePattern();
  } else if (PSPstate_poweredOn()) {
    LED_on();
  } else {
    LED_off();
  }
}

void LED_setup() {
    pinMode(LED, OUTPUT);
    analogWrite(LED, 0);
    patternTimeout.setCallback(onPatternTick);
}
