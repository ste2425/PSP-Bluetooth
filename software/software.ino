// #include "pspBluetoothApp.h"
#include "src/bleApp.h"

#include "Arduino.h"
#include <ezButton.h>
#include "src/generalCfg.h"
#include "src/gpioCfg.h"

GeneralConfig gCfg;
ezButton *modeChangeBtn;

void setup()
{
  Serial.begin(115200);

  auto gpioCfg = gpioCfg_getConfig();

  modeChangeBtn = new ezButton(gpioCfg.ModeChange);
  modeChangeBtn->setDebounceTime(50);

  gCfg = generalCfg_getConfig();

  if (gCfg.BLEEnabled)
    bleApp_setup();
  // else
  //   pspBluetooth_setup();
}

void loop()
{
  modeChangeBtn->loop();

  if (modeChangeBtn->isPressed())
  {
    gCfg.BLEEnabled = !gCfg.BLEEnabled;
    generalCfg_setConfig(gCfg);

    Serial.println("Toggled BLE Mode, Rebooting");
    ESP.restart();
  }

  if (gCfg.BLEEnabled)
    bleApp_loop();
  // else
  //    pspBluetooth_loop();
  delay(150);
}