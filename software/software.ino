#include <Bluepad32.h>
#include "src/digipot.h"
#include "src/led.h"
#include "src/PSPState.h"
#include "src/Button.h"
#include "src/Mappings.h"
#include <LittleFS.h>
#include "src/controllerManager.h"
#include "src/interop.h"
#include "src/timeout.h"

Timeout disableNewConnectionTimer;

void disableNewConnections() {
  CTRMANAGER_enableConnections(false);
}

void setup() { 
    disableNewConnectionTimer.setCallback(disableNewConnections);
    disableNewConnectionTimer.setTimeout(1000);
    disableNewConnectionTimer.start();

    Serial.begin(115200);

    LittleFS.begin(true);

    DIGIPOT_setup();
    BUTTON_setup();
    PSPState_setup();
    MAPPINGS_setup();
    CTRMANAGER_setup();
    LED_setup();
    INTEROP_setup();

    INTEROP_enableBLEService(false);
}

void loop() {    
  disableNewConnectionTimer.loop();
  LED_loop();
  
  BUTTON_loop();
  CTRMANAGER_loop();
  INTEROP_loop();

  delay(1);
}
