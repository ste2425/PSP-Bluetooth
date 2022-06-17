#include <Bluepad32.h>
#include "digipot.h"
#include "pspPins.h"
#include "mappings.h"
#include "variables.h"

// Currently only use the first connected gamepad. Bluepad32 supports upto 4
// Would be good to allow multiple, maybe could use different controllers for emulators etc
GamepadPtr myGamepad = nullptr;

pinMap *mappedPins = defaultMappings;
const int *mappedPinsSizePTR = &defaultMappingsLength;

int mode = 0;
bool modeToggled = false;

void setup() {
  Serial.begin(VAR_serial_baud);

  if (VAR_serial_wait) {
    while (!Serial) {
      ;
    }
  }
  
  String fv = BP32.firmwareVersion();
  Serial.print("Firmware version installed: ");
  Serial.println(fv);
  
  BP32.setup(&onConnectedGamepad, &onDisconnectedGamepad);

  // This needs to be behind a reset button later
  BP32.forgetBluetoothKeys();
  
  DIGIPOT_setup();
  
  Serial.println("Setup Complete");
}

void loop() {
  BP32.update();

  PSP_mark_all_for_release();
  
  if (myGamepad && myGamepad->isConnected()) {  
    updateButtons(myGamepad);
    
    if (myGamepad->miscBack()) {
      toggleMode();
    } else {
      modeToggled = false;
    }

    if (myGamepad->thumbL()) {
      toggle_power();
    }          
  }

  PSP_release_unused();
}

void toggleMode() {
  if (modeToggled) return;

  modeToggled = true;
  mode++;

  if (mode > 2)
    mode = 0;

  PSP_mark_all_for_release();
  PSP_release_unused();

  switch (mode) {
    case 0:
      mappedPins = defaultMappings;
      mappedPinsSizePTR = &defaultMappingsLength;
    break;
    case 1:
      mappedPins = FPSMappings;
      mappedPinsSizePTR = &FPSMappingsLength;
    break;
    case 2:
      mappedPins = PS1Mappings;
      mappedPinsSizePTR = &PS1MappingsLength;
  }

  myGamepad->setPlayerLEDs((mode + 1) & 0x0f);
  myGamepad->setRumble(15, 5);

  Serial.print("Changing Mode ");
  Serial.print(mode);
  Serial.println("");
}

//---------------------------------------

/*
 * --------------------------------------
 * Bluepad32 callbacks
 * --------------------------------------
 */
void onConnectedGamepad(GamepadPtr gp) {
  if (myGamepad == nullptr) {
    myGamepad = gp;
    
    Serial.println("CALLBACK: Gamepad is connected");

    GamepadProperties properties = gp->getProperties();
    char buf[80];
    sprintf(buf,
            "BTAddr: %02x:%02x:%02x:%02x:%02x:%02x, VID/PID: %04x:%04x, "
            "flags: 0x%02x",
            properties.btaddr[0], properties.btaddr[1], properties.btaddr[2],
            properties.btaddr[3], properties.btaddr[4], properties.btaddr[5],
            properties.type, properties.subtype, properties.flags);
    Serial.println(buf);

    //PSP_power_on();
    //PSP_toggle_screen();
  }
}

void onDisconnectedGamepad(GamepadPtr gp) {
  if (myGamepad != nullptr && myGamepad == gp) {
    myGamepad = nullptr;
    
    Serial.println("CALLBACK: Gamepad is disconnected");

   // PSP_power_off();
  }
}

void updateButtons(GamepadPtr gamepad) {
  for (byte i = 0; i < *mappedPinsSizePTR; i++){
    pinMap *mapping = &mappedPins[i];

    switch (mapping->type) {
      case TYPE_DPAD:
        if (gamepad->dpad() & mapping->controllerMask) {
            PSP_press_button(mapping->pin);
        }
      break;
      case TYPE_BUTTONS:
        if (gamepad->buttons() & mapping->controllerMask) {
            PSP_press_button(mapping->pin);
        }
      break;
      case TYPE_SYSTEM:
        if (gamepad->miscButtons() & mapping->controllerMask) {
            PSP_press_button(mapping->pin);
        }
      break; 
      case TYPE_TRIGGER:
        if (mapping->controllerMask == BUTTON_THROTTLE ? myGamepad->throttle() : myGamepad->brake() > VAR_analog_trigger_threshold) {
            PSP_press_button(mapping->pin);
        }
      break;
      case TYPE_RS:
        handle_RS(mapping);
      break;
      case TYPE_LS:
        handle_LS(mapping);
      break;
    }
  }
}

void handle_LS(pinMap *mapping) {
  int xVal = myGamepad->axisX();
  int yVal = myGamepad->axisY();

  int activated = 0;

  switch(mapping->controllerMask) {
    case BUTTON_RS_UP:
      activated = yVal < VAR_ts_btn_threshold * -1;
    break;
    case BUTTON_RS_DOWN:
      activated = yVal > VAR_ts_btn_threshold;
    break;
    case BUTTON_RS_RIGHT:
      activated = xVal > VAR_ts_btn_threshold;
    break;
    case BUTTON_RS_LEFT:
      activated = xVal < VAR_ts_btn_threshold * -1;
    break;
    case BUTTON_LS:
      PSP_set_ls(PSP_map_controller(xVal), PSP_map_controller(yVal));
      return;
    break;
  }

  if (activated) {
      PSP_press_button(mapping->pin);
  }
}

void handle_RS(pinMap *mapping) {
  int xVal = myGamepad->axisRX();
  int yVal = myGamepad->axisRY();

  int activated = 0;

  switch(mapping->controllerMask) {
    case BUTTON_RS_UP:
      activated = yVal < VAR_ts_btn_threshold * -1;
    break;
    case BUTTON_RS_DOWN:
      activated = yVal > VAR_ts_btn_threshold;
    break;
    case BUTTON_RS_RIGHT:
      activated = xVal > VAR_ts_btn_threshold;
    break;
    case BUTTON_RS_LEFT:
      activated = xVal < VAR_ts_btn_threshold * -1;
    break;
    case BUTTON_LS:
      PSP_set_ls(PSP_map_controller(xVal), PSP_map_controller(yVal));
      return;
    break;
  }

  if (activated) {
      PSP_press_button(mapping->pin);
  }
}
