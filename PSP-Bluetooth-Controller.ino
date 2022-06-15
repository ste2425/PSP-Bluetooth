#include <Bluepad32.h>
#include "pot.h"
#include "pspPins.h"
#include "mappings.h"

// Currently only use the first connected gamepad. Bluepad32 supports upto 4
// Would be good to allow multiple, maybe could use different controllers for emulators etc
GamepadPtr myGamepad = nullptr;

pinMap *mappedPins = defaultMappings;
const int *mappedPinsSizePTR = &defaultMappingsLength;

int mode = 0;
bool modeToggled = false;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  String fv = BP32.firmwareVersion();
  Serial.print("Firmware version installed: ");
  Serial.println(fv);
  
  // This call is mandatory. It setups Bluepad32 and creates the callbacks.
  BP32.setup(&onConnectedGamepad, &onDisconnectedGamepad);

  // This needs to be behind a reset button later
  BP32.forgetBluetoothKeys();
  
  POT_setup();
  
  Serial.println("Setup Complete");
}

void loop() {
  BP32.update();

  PSP_mark_all_for_release();
  
  if (myGamepad && myGamepad->isConnected()) {  
    updateButtons(myGamepad);
   // updateLeftStick(myGamepad);
    
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

  if (mode > 1)
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
  }

  myGamepad->setPlayerLEDs((mode + 1) & 0x0f);
  myGamepad->setRumble(15, 5);

  Serial.print("Changing Mode ");
  Serial.print(mode);
  Serial.println("");
}

void updateLeftStick(GamepadPtr gamepad) {
    int axisX = POT_map_controller(gamepad->axisX());
    int axisY = POT_map_controller(gamepad->axisY());

    POT_set(axisX, axisY);
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
            properties.vendor_id, properties.product_id, properties.flags);
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
        if (mapping->controllerMask == BUTTON_THROTTLE ? myGamepad->throttle() : myGamepad->brake() > 300) {
            PSP_press_button(mapping->pin);
        }
      break;
      case TYPE_RS:
        handle_RS(mapping);
      break;
    }
  }
}

bool handle_RS(pinMap *mapping) {
  int xVal = myGamepad->axisRX();
  int yVal = myGamepad->axisRY();

  int activated = 0;

  switch(mapping->controllerMask) {
    case RS_UP:
      activated = yVal < -250;
    break;
    case RS_DOWN:
      activated = yVal > 250;
    break;
    case RS_RIGHT:
      activated = xVal > 250;
    break;
    case RS_LEFT:
      activated = xVal <- 250;
    break;
  }

  if (activated) {
      PSP_press_button(mapping->pin);
  }
}

