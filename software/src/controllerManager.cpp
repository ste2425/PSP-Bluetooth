#include "controllerManager.h"

Timeout pspBootTimeout;
Timeout pspScreenTimeout;
Timeout controllerTurnOffTimeout;

bool pspBooting = false;

#define maxGamepads 4

ControllerPtr myControllers[maxGamepads];

void onPspBootTimeout() {
  PSPState_pressScreen();
  pspScreenTimeout.start();
  pspBootTimeout.setTimeout(1500); // After first boot PSP is 'warm' so dont need to wait as long
}

void onPSPScreenTimeout() {
  PSPState_releaseScreen();
  pspBooting = false;
  LED_autoSet();
}

void disconnectAll() {    
      Serial.println("disconnect");
    for (int i = 0; i < maxGamepads; i++) {
        if (myControllers[i] != nullptr) {
      Serial.println("cont");
          myControllers[i]->disconnect();
        }
    }
}

void onConnectedController(ControllerPtr ctl) {
    
    bool foundEmptySlot = false;
    
    for (int i = 0; i < maxGamepads; i++) {
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

              pspBooting = true;

              LED_autoSet();
            }
            
            break;
        }
    }
    
    if (!foundEmptySlot) {
        Serial.println("CALLBACK: Controller connected, but could not found empty slot");
        ctl->disconnect();
    }
}

void onDisconnectedController(ControllerPtr ctl) {
  
    bool foundController = false;

    for (int i = 0; i < maxGamepads; i++) {
        if (myControllers[i] == ctl) {
            Serial.printf("CALLBACK: Controller disconnected from index=%d\n", i);
            myControllers[i] = nullptr;
            foundController = true;
            break;
        }
    }

    bool allDisconnected = true;

    for (int i = 0; i < maxGamepads; i++) {
        if (myControllers[i] != nullptr) {
            allDisconnected = false;
            break;
        }
    }
    
    if (allDisconnected) {  

      pspBootTimeout.stop();
      pspScreenTimeout.stop();
      PSPState_togglePower();
      INTEROP_enableBLEService(false);

      LED_autoSet();
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

bool CTRMANAGER_pspBooting() {
  return pspBooting;
}

void CTRMANAGER_applyColours() { 
  Serial.println("Applying colours");
    MappingMeta *hi = MAPPINGS_current;
        for (uint8_t i = 0; i < 4; i++) {
          Serial.print("Updating controller for index: ");
          Serial.println(i);

          if (myControllers[i] && myControllers[i]->isConnected()) {
              if (myControllers[i]->isGamepad()) {
                Serial.print("Applying color RGB (");
                Serial.print(hi->colour[0]);
                Serial.print(", ");
                Serial.print(hi->colour[1]);
                Serial.print(", ");
                Serial.print(hi->colour[2]);
                Serial.println(")");

                myControllers[i]->setColorLED(hi->colour[0], hi->colour[1], hi->colour[2]);

                auto indicatorNumber = hi->mappingNumer & 0x0f;
                Serial.print("Setting player indicator: ");
                Serial.println(indicatorNumber);
                myControllers[i]->setPlayerLEDs(indicatorNumber);
                
              } else {
                  Serial.println("Unsupported controller");
              }
          }
        }
}

bool changingMapping = false;

void proccessControllerButton(ControllerPtr ctl, ControllerMapping *mapping) {
  auto isL2 = mapping->ControllerBit == 6;
  auto isR2 = mapping->ControllerBit == 7;
  auto brakeVal = ctl->brake();
  auto throttleVal = ctl->throttle();
  auto pressed = false;

  if (!isL2 && !isR2) {
    if (bitRead(ctl->buttons(), mapping->ControllerBit))
      handleButtonMapping(mapping);
  } else {
    if (brakeVal == 0 && throttleVal == 0)
      pressed = bitRead(ctl->buttons(), mapping->ControllerBit);
    else if ((isL2 && ctl->brake() > 800) || (isR2 && ctl->throttle() > 800))
      pressed = true;

    if (pressed)
      handleButtonMapping(mapping);
  }
}

bool pressingHome = false;

void processGamepad(ControllerPtr ctl) {
    MappingMeta *meta = MAPPINGS_current;
    // next mapping y pressing R1 L2 and Dpad right
    if (bitRead(ctl->buttons(), 4) && bitRead(ctl->buttons(), 5) && bitRead(ctl->dpad(), 2)) {
      if (!changingMapping) {
        MAPPINGS_next();
        delay(200);
        
        ctl->setColorLED(meta->colour[0], meta->colour[1], meta->colour[2]);
        ctl->setPlayerLEDs(meta->mappingNumer & 0x0f);

        changingMapping = true;
      }

      return;
    }

    // controller home button
    bool homePressed = bitRead(ctl->miscButtons(), 0);
    if (homePressed && !pressingHome) {
      Serial.println("Press");
      pressingHome = true;
      controllerTurnOffTimeout.start();
    } else if (!homePressed && pressingHome) {
      Serial.println("Release");
      pressingHome = false;
      controllerTurnOffTimeout.stop();
    }

    changingMapping = false;

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
          proccessControllerButton(ctl, mapping);
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
    controllerTurnOffTimeout.loop();
}

void CTRMANAGER_setup() {
    BP32.setup(&onConnectedController, &onDisconnectedController);

    BP32.enableVirtualDevice(false);

    BP32.forgetBluetoothKeys();
    pspBootTimeout.setCallback(onPspBootTimeout);
    pspBootTimeout.setTimeout(12000);
    pspScreenTimeout.setCallback(onPSPScreenTimeout);
    pspScreenTimeout.setTimeout(6000);

    controllerTurnOffTimeout.setTimeout(3000);
    controllerTurnOffTimeout.setCallback(disconnectAll);
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
