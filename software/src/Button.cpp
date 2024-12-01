#include "Button.h"

OneButton button;

void doubleClick()
{}

void click() {
  CTRMANAGER_toggleConnections();

  if (CTRMANAGER_newConnectionEnabled()) {
    LED_syncPattern();
  } else if (INTEROP_bleServiceEnabled()) {
    LED_blePattern();
  } else if (PSPstate_poweredOn()) {
    LED_on();
  } else {
    LED_off();
  }
}

void longClick() {
  INTEROP_toggleBLEService();

  if (INTEROP_bleServiceEnabled()) {
    LED_blePattern();
  } else if (CTRMANAGER_newConnectionEnabled()) {
    LED_syncPattern();
  } else if (PSPstate_poweredOn()) {
    LED_on();
  } else {
    LED_off();
  }
}

void BUTTON_loop() {
  button.tick();
}

void BUTTON_setup() {
  button.attachDoubleClick(doubleClick);
  button.attachClick(click);
  button.attachLongPressStart(longClick);
  
  button.setup(BUTTON_PIN, INPUT_PULLUP, true);
}
