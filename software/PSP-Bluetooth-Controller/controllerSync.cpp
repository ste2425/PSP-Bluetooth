#include "controllerSync.h"

Timer syncTimer;
boolean syncActive;

void CTRSYNC_setup() {
  syncTimer.setCallback(CTRSYNC_stop);

  syncTimer.setTimeout(9000);
}

void CTRSYNC_start() {
  if (syncActive)
    return;
    
  syncActive = true;

  BP32.forgetBluetoothKeys();
  BP32.enableNewBluetoothConnections(true);
  LED_blinkFast();

  syncTimer.start();
}

void CTRSYNC_stop() {
  syncTimer.stop();
  LED_solid();
  BP32.enableNewBluetoothConnections(false); 
  syncActive = false;
}

boolean CTRSYNC_isActive() {
  return syncActive;
}
