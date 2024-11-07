#include <Bluepad32.h>
#include "src/digipot.h"
#include "src/led.h"
#include "src/PSPState.h"
#include "src/Button.h"
#include "src/Mappings.h"
#include <LittleFS.h>
#include "src/fileUtility.h"
#include "src/bt_service.h"

enum {
    CMD_BLE_SERVICE_ENABLE,
    CMD_BLE_SERVICE_DISABLE
};

static btstack_context_callback_registration_t cmd_callback_registration;

static void cmd_callback(void* context) {
    unsigned long ctx = (unsigned long)context;
    uint16_t cmd = ctx & 0xffff;
    switch (cmd) {
        case CMD_BLE_SERVICE_ENABLE:
            uni_bt_service_set_enabled(true);
            break;
        case CMD_BLE_SERVICE_DISABLE:
            uni_bt_service_set_enabled(false);
            break;
    }
}

void uni_bt_enable_service_safe(bool enabled) {
    cmd_callback_registration.callback = &cmd_callback;
    cmd_callback_registration.context =
        (void*)(enabled ? (intptr_t)CMD_BLE_SERVICE_ENABLE : (intptr_t)CMD_BLE_SERVICE_DISABLE);
    btstack_run_loop_execute_on_main_thread(&cmd_callback_registration);
}


ControllerPtr myControllers[BP32_MAX_GAMEPADS];

// This callback gets called any time a new gamepad is connected.
// Up to 4 gamepads can be connected at the same time.
void onConnectedController(ControllerPtr ctl) {
    
    bool foundEmptySlot = false;
    
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == nullptr) {            
            myControllers[i] = ctl;
            foundEmptySlot = true;

            MappingMeta *meta = MAPPINGS_current;
            ctl->setColorLED(meta->colour[0], meta->colour[1], meta->colour[2]);
            ctl->setPlayerLEDs(meta->mappingNumer & 0x0f);
            
            // if first controller
            if (i == 0) {
              LED_fadeSlow();
              PSPState_togglePower();

              delay(12000);

              PSPState_toggleScreen();
            }
            
            break;
        }
    }
    
    if (!foundEmptySlot) {
        Serial.println("CALLBACK: Controller connected, but could not found empty slot");
    }
}

void onDisconnectedController(ControllerPtr ctl) {
  
    bool foundController = false;

    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == ctl) {
            Serial.printf("CALLBACK: Controller disconnected from index=%d\n", i);
            myControllers[i] = nullptr;
            foundController = true;
            break;
        }
    }

    bool allDisconnected = true;

    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] != nullptr) {
            allDisconnected = false;
            break;
        }
    }
    
    if (allDisconnected) {
      LED_off();
      PSPState_togglePower();
    }

    if (!foundController) {
        Serial.println("CALLBACK: Controller disconnected, but not found in myControllers");
    }
}

void handleButtonMapping(ControllerMapping *mapping) {
  // value over 100 means button to analog stick
  if (mapping->PSPButton > 100) {
    if (mapping->PSPButton == 101) { // right
      PSPState_analogX(510);
    } else if(mapping->PSPButton == 102) { // left
      PSPState_analogX(-510);
    } else if (mapping->PSPButton == 103) { // up
      PSPState_analogY(-510);
    } else if(mapping->PSPButton == 104) { // down
      PSPState_analogY(510);
    }
  } else {
    PSPState_markButtonAsPressed(mapping->PSPButton);
  }
}

void processGamepad(ControllerPtr ctl) {
    if (bitRead(ctl->buttons(), 8)) {
      MAPPINGS_next();
      delay(200);
      
      MappingMeta *meta = MAPPINGS_current;
      ctl->setColorLED(meta->colour[0], meta->colour[1], meta->colour[2]);
      ctl->setPlayerLEDs(meta->mappingNumer & 0x0f);
    } else {
      for (uint8_t i = 0; i < 30; i++) {
        MappingMeta *meta = MAPPINGS_current;
        ControllerMapping *mapping = &meta->mappings[i];
  
        if (mapping->PSPButton == 99)
          continue;
        
        switch (mapping->ControllerType) {
          case ControllerTypes::DPad:
            // > 100 means controlling analog stick
            if (bitRead(ctl->dpad(), mapping->ControllerBit))
              handleButtonMapping(mapping);
          break;
          case ControllerTypes::Misc:
            if (bitRead(ctl->miscButtons(), mapping->ControllerBit))
              handleButtonMapping(mapping);
          break;
          case ControllerTypes::Button:
            if (bitRead(ctl->buttons(), mapping->ControllerBit))
              handleButtonMapping(mapping);
          break;
          case ControllerTypes::Analog:       
            int32_t  xAxis = ctl->axisX();
            int32_t yAxis = ctl->axisY();       
            int32_t  xRAxis = ctl->axisRX();
            int32_t yRAxis = ctl->axisRY();
          
            switch(mapping->ControllerBit) {
              case 1:
                PSPState_analog(xAxis, yAxis);
              break;
              case 2:
                PSPState_analog(xRAxis, yRAxis);
              break;
              case 3: //lu
                if (yAxis < -400) {
                  PSPState_markButtonAsPressed(mapping->PSPButton);
                }
              break;
              case 4: //ld
                if (yAxis > 400) {
                  PSPState_markButtonAsPressed(mapping->PSPButton);
                }
              break;
              case 5: //ll
                if (xAxis < -400) {
                  PSPState_markButtonAsPressed(mapping->PSPButton);
                }
              break;
              case 6: //lr
                if (xAxis > 400) {
                  PSPState_markButtonAsPressed(mapping->PSPButton);
                }
              break;
              case 7: //ru
                if (yRAxis < -400) {
                  PSPState_markButtonAsPressed(mapping->PSPButton);
                }
              break;
              case 8: //rd
                if (yRAxis > 400) {
                  PSPState_markButtonAsPressed(mapping->PSPButton);
                }
              break;
              case 9: //rd
                if (xRAxis < -400) {
                  PSPState_markButtonAsPressed(mapping->PSPButton);
                }
              break;
              case 10: //rr
                if (xRAxis > 400) {
                  PSPState_markButtonAsPressed(mapping->PSPButton);
                }
              break;
            }
          break;
        }
      }
    }
}

void processControllers() {
    for (auto myController : myControllers) {
        if (myController && myController->isConnected() && myController->hasData()) {
            if (myController->isGamepad()) {
                processGamepad(myController);
            } else {
                Serial.println("Unsupported controller");
            }
        }
    }
}

// Arduino setup function. Runs in CPU 1
void setup() {  
    Serial.begin(115200);

    // Setup the Bluepad32 callbacks
    BP32.setup(&onConnectedController, &onDisconnectedController);

    // "forgetBluetoothKeys()" should be called when the user performs
    // a "device factory reset", or similar.
    // Calling "forgetBluetoothKeys" in setup() just as an example.
    // Forgetting Bluetooth keys prevents "paired" gamepads to reconnect.
    // But it might also fix some connection / re-connection issues.
    BP32.forgetBluetoothKeys();

    // Enables mouse / touchpad support for gamepads that support them.
    // When enabled, controllers like DualSense and DualShock4 generate two connected devices:
    // - First one: the gamepad
    // - Second one, which is a "virtual device", is a mouse.
    // By default, it is disabled.
    BP32.enableVirtualDevice(false);

    LittleFS.begin(true);

    DIGIPOT_setup();
    BUTTON_setup();
    PSPState_setup();
    MAPPINGS_setup();

    //uni_bt_enable_service_safe(true);
}

// Arduino loop function. Runs in CPU 1.
void loop() {  
  //must be run first
  LED_loop();
  BUTTON_loop();
  
    // This call fetches all the controllers' data.
    // Call this function in your main loop.
    bool dataUpdated = BP32.update();
    if (dataUpdated) {
        PSPState_markAllReleased();
        processControllers();
    }
    
    PSPState_loop();

    // must be run last, after controller processing

    // The main loop must have some kind of "yield to lower priority task" event.
    // Otherwise, the watchdog will get triggered.
    // If your main loop doesn't have one, just add a simple `vTaskDelay(1)`.
    // Detailed info here:
    // https://stackoverflow.com/questions/66278271/task-watchdog-got-triggered-the-tasks-did-not-reset-the-watchdog-in-time

    //     vTaskDelay(1);
    delay(1);
}
