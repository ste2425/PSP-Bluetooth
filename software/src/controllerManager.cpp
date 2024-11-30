#include "controllerManager.h"

Timeout pspBootTimeout;
Timeout pspScreenTimeout;

ControllerPtr myControllers[BP32_MAX_GAMEPADS];

void onPspBootTimeout() {
  PSPState_pressScreen();
  pspScreenTimeout.start();
}

void onPSPScreenTimeout() {
  PSPState_releaseScreen();
  LED_on();
}

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
              CTRMANAGER_enableConnections(false);
              PSPState_togglePower();
              pspBootTimeout.start();

              if (CTRMANAGER_newConnectionEnabled()) {
                LED_syncPattern();
              } else if (INTEROP_bleServiceEnabled()) {
                LED_blePattern();
              } else {
                LED_bootPattern();
              }
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
      if (CTRMANAGER_newConnectionEnabled()) {
        LED_syncPattern();
      } else if (INTEROP_bleServiceEnabled()) {
        LED_blePattern();
      } else {
        LED_off();
      }

      pspBootTimeout.stop();
      pspScreenTimeout.stop();
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

bool CTRMANAGER_applyColours() {
    MappingMeta *meta = MAPPINGS_current;

    for (auto ctl : myControllers) {
      if (ctl && ctl->isConnected() && ctl->isGamepad()) {
        ctl->setColorLED(meta->colour[0], meta->colour[1], meta->colour[2]);
        ctl->setPlayerLEDs(meta->mappingNumer & 0x0f);
      }
    }
}

void processGamepad(ControllerPtr ctl) {
    MappingMeta *meta = MAPPINGS_current;
    if (bitRead(ctl->buttons(), 8)) {
      MAPPINGS_next();
      delay(200);
      
      ctl->setColorLED(meta->colour[0], meta->colour[1], meta->colour[2]);
      ctl->setPlayerLEDs(meta->mappingNumer & 0x0f);
    } else {
      for (uint8_t i = 0; i < 30; i++) {
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

void CTRMANAGER_loop() {  
    bool dataUpdated = BP32.update();
    if (dataUpdated) {
        PSPState_markAllReleased();
        processControllers();
    }
    
    PSPState_loop();
    pspBootTimeout.loop();
    pspScreenTimeout.loop();
}

void CTRMANAGER_setup() {
    BP32.setup(&onConnectedController, &onDisconnectedController);

    BP32.enableVirtualDevice(false);

    BP32.forgetBluetoothKeys();
    pspBootTimeout.setCallback(onPspBootTimeout);
    pspBootTimeout.setTimeout(12000);
    pspScreenTimeout.setCallback(onPSPScreenTimeout);
    pspScreenTimeout.setTimeout(6000);
}

bool newConnectionsEnabled = false;

bool CTRMANAGER_newConnectionEnabled() {
  return newConnectionsEnabled;
}

void CTRMANAGER_enableConnections(bool enabled) {
  if (enabled)
    BP32.forgetBluetoothKeys();
  //delay(100);
  newConnectionsEnabled = enabled;
  
  Serial.println("toggle ");
  Serial.println(enabled);
  BP32.enableNewBluetoothConnections(enabled);
}

void CTRMANAGER_toggleConnections() {
  if (newConnectionsEnabled) {
    CTRMANAGER_enableConnections(false);
    Serial.println("Disabling controller connections");
  } else {
    CTRMANAGER_enableConnections(true);
    Serial.println("Enabling controller connections");
  }
}
