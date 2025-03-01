#include "Button.h"

OneButton button;

void handleClick(uint8_t mode) {
  switch(mode) {
    case 1: // controller pair
      INTEROP_enableBLEService(false);
      CTRMANAGER_toggleConnections();  
      break;
    case 2: // BLE mode
      CTRMANAGER_enableConnections(false);
      INTEROP_toggleBLEService();
      break;
    case 3: // power toggle
      CTRMANAGER_enableConnections(false);
      INTEROP_enableBLEService(false);
      
      if (CTRMANAGER_isBooted()) {
        CTRMANAGER_bootConsole(false);
        CTRMANAGER_disconnectAll();
      } else {
        CTRMANAGER_bootConsole(true);
      }
      break;
  }

  LED_autoSet();
}

void doubleClick() { 
  handleClick(SETTINGS_current.dblClickMode);
}

void click() {
  handleClick(SETTINGS_current.clickMode);
}

void longClick() {
  handleClick(SETTINGS_current.lngClickMode);
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
