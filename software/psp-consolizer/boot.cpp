#include "boot.h"

Timer waitForScreenTimer;
Timer waitForScreenRelease;

bool booting = false;

void BOOT_powerOn() {
  if (booting)
    return;

  booting = true;
  
  bool coldBoot = PSP_power_on();

  LED_blinkSlow();
  
  waitForScreenTimer.setTimeout(coldBoot ? VAR_boot_screen_wait : VAR_boot_screen_wait_warm);

  waitForScreenTimer.start();
}

void BOOT_powerOff() {
    if (booting)
      return;
      
    PSP_power_off();
    LED_off();
    waitForScreenTimer.stop();
}

bool BOOT_isBooting() {
  return booting;
}

void BOOT_setup() {
  waitForScreenRelease.setTimeout(6000);

  waitForScreenTimer.setCallback([]() -> void {
      PSP_press_screen();
      waitForScreenRelease.start();
  });

  waitForScreenRelease.setCallback([]() -> void {
    PSP_release_screen();
    LED_solid();
    
    booting = false;
  });
}
