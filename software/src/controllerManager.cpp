#include "controllerManager.h"

Timeout pspBootTimeout;
Timeout pspScreenTimeout;
Timeout controllerTurnOffTimeout;
Timeout lowBaterryCheckTimeout;

bool pspBooting = false;
bool pspColdBoot = true;

bool batteryLow = false;

#define maxGamepads 4

ControllerPtr myControllers[maxGamepads];

void onPspBootTimeout() {
  PSPState_pressScreen();
  pspScreenTimeout.start();
}

void onPSPScreenTimeout() {
  Serial.println("Screen has been pressed");
  PSPState_releaseScreen();
  pspBooting = false;
  LED_autoSet();
}

void onLowBatteryCheck() {
 // Serial.println("Battery Timeout triggered");

  if (myControllers[0] != nullptr) {
        
    uint8_t batteryLevel = myControllers[0]->battery();
    //Serial.print("Battery level: ");
    //Serial.println(batteryLevel);

    auto batteryBelow = batteryLevel < SETTINGS_current.batteryWarning;

    if (batteryBelow && !batteryLow) {
      //Serial.println("Below threshold setting light");
      batteryLow = true;
      LED_autoSet();
    } else if(!batteryBelow && batteryLow) {      
      //Serial.println("Battery OK");
      batteryLow = false;
      LED_autoSet();
    }
    
    lowBaterryCheckTimeout.start();
  } else {    
    batteryLow = false;
  }
}

bool CTRMANAGER_isBooted() {
  return PSPstate_poweredOn();
}

void CTRMANAGER_bootConsole(bool bootOrEnd) {
  if (bootOrEnd) {
    CTRMANAGER_enableConnections(false);
    PSPState_togglePower();
    auto delayBootTime = pspColdBoot ? SETTINGS_current.coldBootDelay : SETTINGS_current.warmBootDelay;

    Serial.println("Boot delay");
    Serial.println(delayBootTime);

    pspBootTimeout.setTimeout(delayBootTime);
    pspColdBoot = false;
    pspBootTimeout.start();

    pspBooting = true;

    lowBaterryCheckTimeout.start();
  } else {
    pspBootTimeout.stop();
    pspScreenTimeout.stop();
    lowBaterryCheckTimeout.stop();

    pspBooting = false;

    if (PSPstate_poweredOn())
      PSPState_togglePower();

    INTEROP_enableBLEService(false);
  }
}

bool CTRMANAGER_batteryLow() {
  return batteryLow;
}

void CTRMANAGER_updateSettings() {
  pspScreenTimeout.stop();
  pspScreenTimeout.setTimeout(SETTINGS_current.screenDelay);
  controllerTurnOffTimeout.setTimeout(SETTINGS_current.shutdownDelay);
}

void CTRMANAGER_disconnectAll() {    
    Serial.println("Disconnecting all controllers");

    for (int i = 0; i < maxGamepads; i++) {
        if (myControllers[i] != nullptr) {
          Serial.print("Disconnecting: ");
          Serial.println(i);
          myControllers[i]->disconnect();
        }
    }

  lowBaterryCheckTimeout.stop();
  batteryLow = false;
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
            if (i == 0 && !PSPstate_poweredOn()) {
              CTRMANAGER_bootConsole(true);
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
      CTRMANAGER_bootConsole(false);
      LED_autoSet();
    }

    if (!foundController) {
        Serial.println("CALLBACK: Controller disconnected, but not found in myControllers");
    }

    batteryLow = false;
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
    else if ((isL2 && ctl->brake() > SETTINGS_current.throttleThreshold) || (isR2 && ctl->throttle() > SETTINGS_current.throttleThreshold))
      pressed = true;

    if (pressed)
      handleButtonMapping(mapping);
  }
}

bool pressingHome = false;

void handleAnalogUp(int32_t xAxis, int32_t yAxis, uint8_t pspButton) {
  bool isUp = yAxis < 0;

  if (!isUp)
    return;

  uint32_t value = yAxis * -1; // up is minus. Ensure always positivr
  
  bool upPressed = value > SETTINGS_current.analogThreshold;
  
  if (upPressed){
    PSPState_markButtonAsPressed(pspButton);
  }
}

void handleAnalogDown(int32_t xAxis, int32_t yAxis, uint8_t pspButton) {
  bool isDown = yAxis > 0; // down is positive

  if (!isDown)
    return;

  uint32_t value = yAxis;

  bool pressed = value > SETTINGS_current.analogThreshold;

  if (pressed) {
    PSPState_markButtonAsPressed(pspButton);
  }
}

void handleAnalogLeft(int32_t xAxis, int32_t yAxis, uint8_t pspButton) {
  bool isLeft = xAxis < 0;

  if (!isLeft)
    return;

  uint32_t value = xAxis * -1; // left is minus. Ensure always positivr
  
  bool pressed = value > SETTINGS_current.analogThreshold;
  
  if (pressed) {
    PSPState_markButtonAsPressed(pspButton);
  }
}

void handleAnalogRight(int32_t xAxis, int32_t yAxis, uint8_t pspButton) {
  bool isRight = xAxis > 0; // right is positive

  if (!isRight)
    return;

  uint32_t value = xAxis;

  bool pressed = value > SETTINGS_current.analogThreshold;

  if (pressed) {
    PSPState_markButtonAsPressed(pspButton);
  }
}

void processGamepad(ControllerPtr ctl) {
    MappingMeta *meta = MAPPINGS_current;
    // next mapping y pressing R1 L2 and Dpad right
    auto l1Pressed = bitRead(ctl->buttons(), 4);
    auto r1Pressed = bitRead(ctl->buttons(), 5);
    auto selectPressed = bitRead(ctl->miscButtons(), 1);
    auto startPressed = bitRead(ctl->miscButtons(), 2);
    bool homePressed = bitRead(ctl->miscButtons(), 0);
    
    auto isAltMode = SETTINGS_current.useExtraCombo ?
      l1Pressed && r1Pressed && selectPressed && startPressed : 
      selectPressed && startPressed;

    auto dpadRightPressed = bitRead(ctl->dpad(), 2);
    auto dpadLeftPressed = bitRead(ctl->dpad(), 3);
    
    if (isAltMode && dpadRightPressed) {
      if (!changingMapping) {
        MAPPINGS_next();
        delay(200);
        
        ctl->setColorLED(meta->colour[0], meta->colour[1], meta->colour[2]);
        ctl->setPlayerLEDs(meta->mappingNumer & 0x0f);

        changingMapping = true;
      }

      return;
    } else if (isAltMode && dpadLeftPressed) {
      Serial.println("Pressing left");
     // PSPState_pressScreen();
      // dont forget to mark it as pressed otherwise it will never be released.
      PSPState_markButtonAsPressed(GPIOPins::Display);
      return;
    }

    // controller home button
    bool sutdownPressed = SETTINGS_current.useAlternativeShutdown ? 
      L1 && R1 && startPressed :
      homePressed;
    if (sutdownPressed && !pressingHome) {
      Serial.println("Starting shutdown timer");
      pressingHome = true;
      controllerTurnOffTimeout.start();
    } else if (!sutdownPressed && pressingHome) {
      Serial.println("Stopping shutdown timer");
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
              handleAnalogUp(xAxis, yAxis, mapping->PSPButton);
            break;
            case 4: //ld
              handleAnalogDown(xAxis, yAxis, mapping->PSPButton);
            break;
            case 5: //ll
              handleAnalogLeft(xAxis, yAxis, mapping->PSPButton);
            break;
            case 6: //lr
              handleAnalogRight(xAxis, yAxis, mapping->PSPButton);
            break;
            case 7: //ru
              handleAnalogUp(xRAxis, yRAxis, mapping->PSPButton);
            break;
            case 8: //rd
              handleAnalogDown(xRAxis, yRAxis, mapping->PSPButton);
            break;
            case 9: //rl
              handleAnalogLeft(xRAxis, yRAxis, mapping->PSPButton);
            break;
            case 10: //rr
              handleAnalogRight(xRAxis, yRAxis, mapping->PSPButton);
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
    lowBaterryCheckTimeout.loop();
}

void CTRMANAGER_setup() {
    BP32.setup(&onConnectedController, &onDisconnectedController);

    BP32.enableVirtualDevice(false);

    BP32.forgetBluetoothKeys();
    pspBootTimeout.setCallback(onPspBootTimeout);
    pspScreenTimeout.setCallback(onPSPScreenTimeout);
    pspScreenTimeout.setTimeout(SETTINGS_current.screenDelay);

    controllerTurnOffTimeout.setTimeout(SETTINGS_current.shutdownDelay);
    controllerTurnOffTimeout.setCallback(CTRMANAGER_disconnectAll);

    lowBaterryCheckTimeout.setTimeout(5000);
    lowBaterryCheckTimeout.setCallback(onLowBatteryCheck);
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
