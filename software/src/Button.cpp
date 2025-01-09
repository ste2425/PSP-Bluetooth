#include "Button.h"

OneButton button;

void doubleClick()
{}

void click() {
  INTEROP_enableBLEService(false);
  CTRMANAGER_toggleConnections();  

  LED_autoSet();
}

void longClick() {
  CTRMANAGER_enableConnections(false);
  INTEROP_toggleBLEService();

  LED_autoSet();
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
