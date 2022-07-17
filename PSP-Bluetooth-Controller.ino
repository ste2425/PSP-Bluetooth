#include <Bluepad32.h>
#include <ezButton.h>
#include "digipot.h"
#include "pspPins.h"
#include "mappings.h"
#include "variables.h"
#include <timer.h>

Timer syncTimer;

ezButton syncBtn(20);

// Currently only use the first connected gamepad. Bluepad32 supports upto 4
// Would be good to allow multiple, maybe could use different controllers for emulators etc
GamepadPtr myGamepad = nullptr;

void onSyncEnd() {
  BP32.enableNewBluetoothConnections(false);
  Serial.println("Disabling Sync");
}

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

  BP32.enableNewBluetoothConnections(false);
  
  DIGIPOT_setup();
  syncBtn.setDebounceTime(50);
  syncTimer.setCallback(onSyncEnd);
  syncTimer.setTimeout(6000);

  if (!VAR_standby_mode && !VAR_disable_auto_boot) {
    PSP_power_on();
    delay(VAR_boot_screen_wait);
    PSP_toggle_screen();
  }
  
  Serial.println("Setup Complete");
}

void loop() {
  syncBtn.loop();
  BP32.update();
  syncTimer.update();

  PSP_mark_all_for_release();
  
  if (myGamepad && myGamepad->isConnected()) {  
    updateButtons(myGamepad);   

    if (myGamepad->thumbR()) {
      MAPPINGS_next_mapping();      
      myGamepad->setRumble(15, 5);
      delay(500);
    }
  }

  if (syncBtn.isPressed()) {
    if (syncTimer.isStopped()) {
      Serial.println("Enabling Sync");
      BP32.forgetBluetoothKeys();
      BP32.enableNewBluetoothConnections(true);

      syncTimer.start();
    }
  }

  PSP_release_unused();
}

void onConnectedGamepad(GamepadPtr gp) {
  if (myGamepad == nullptr) {
    myGamepad = gp;
    
    Serial.println("CALLBACK: Gamepad is connected");
    if (VAR_standby_mode && !VAR_disable_auto_boot) {
      myGamepad->setColorLED(166, 17, 17);
      bool coldBoot = PSP_power_on();
      Serial.print("Is Cold Boot: ");
      Serial.println(coldBoot);
      myGamepad->setColorLED(230, 211, 9);
      delay(coldBoot ? VAR_boot_screen_wait : VAR_boot_screen_wait_warm);
      myGamepad->setColorLED(9, 230, 218);
      PSP_toggle_screen();
      myGamepad->setColorLED(128, 235, 14);
    }
  }
}

void onDisconnectedGamepad(GamepadPtr gp) {
  if (myGamepad != nullptr && myGamepad == gp) {
    myGamepad = nullptr;
    
    Serial.println("CALLBACK: Gamepad is disconnected");
    
    if (VAR_standby_mode && !VAR_disable_auto_boot) {
      PSP_power_off();
    }
  }
}

void updateButtons(GamepadPtr gamepad) {
  for (byte i = 0; i < MAPPINGS_pin_size; i++){
    pinMap *mapping = &MAPPINGS_pins[i];

    switch (mapping->type) {
      case CTR_TYPE_DPAD:
        if (gamepad->dpad() & mapping->controllerMask) {
            PSP_press_button(mapping->pin);
        }
      break;
      case CTR_TYPE_BUTTONS:
        if (gamepad->buttons() & mapping->controllerMask) {
            PSP_press_button(mapping->pin);
        }
      break;
      case CTR_TYPE_SYSTEM:
        if (gamepad->miscButtons() & mapping->controllerMask) {
            PSP_press_button(mapping->pin);
        }
      break; 
      case CTR_TYPE_TRIGGER:
        handle_Trigger(mapping);
      break;
      case CTR_TYPE_RS:
        handle_TS(mapping, myGamepad->axisRX(), myGamepad->axisRY());
      break;
      case CTR_TYPE_LS:
        handle_TS(mapping, myGamepad->axisX(), myGamepad->axisY());
      break;
    }
  }
}

void handle_Trigger(pinMap *mapping) {
  int triggerValue = mapping->controllerMask == CTR_BUTTON_THROTTLE ? myGamepad->throttle() : myGamepad->brake();
  
  if (triggerValue > VAR_analog_trigger_threshold) {
      PSP_press_button(mapping->pin);
  }
}

void handle_TS(pinMap *mapping, int xAxis, int yAxis) {
  uint8_t activated = 0;

  switch(mapping->controllerMask) {
    case CTR_BUTTON_UP:
      activated = yAxis < VAR_ts_btn_threshold * -1;
    break;
    case CTR_BUTTON_DOWN:
      activated = yAxis > VAR_ts_btn_threshold;
    break;
    case CTR_BUTTON_RIGHT:
      activated = xAxis > VAR_ts_btn_threshold;
    break;
    case CTR_BUTTON_LEFT:
      activated = xAxis < VAR_ts_btn_threshold * -1;
    break;
    case CTR_BUTTON_TS:
      PSP_set_ls(PSP_map_controller(xAxis), PSP_map_controller(yAxis));
      return;
    break;
  }

  if (activated) {
      PSP_press_button(mapping->pin);
  }
}
