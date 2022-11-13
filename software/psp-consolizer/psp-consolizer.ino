#include <Bluepad32.h>
#include <ezButton.h>
#include <timer.h>
#include <timerManager.h>
#include "mappings.h"
#include "led.h"
#include "controllerSync.h"
#include "boot.h"

uint8_t LEDScale = 10;

ezButton syncBtn(20);

Timer controllerLEDTunoff;
Timer shutdownTimer;

// Currently only use the first connected gamepad. Bluepad32 supports upto 4
// Would be good to allow multiple, maybe could use different controllers for emulators etc
GamepadPtr myGamepad = nullptr;

void setScaledControllerColour(uint8_t r, uint8_t g, uint8_t b)
{
  float scale = LEDScale / 10.00;

  myGamepad->setColorLED(MAPPINGS_colour[0] * scale, MAPPINGS_colour[1] * scale, MAPPINGS_colour[2] * scale);
}

void onConnectedGamepad(GamepadPtr gp)
{
  if (myGamepad == nullptr)
  {

    if (CTRSYNC_isActive())
      CTRSYNC_stop();

    myGamepad = gp;

    if (LEDScale != 0)
    {
      setScaledControllerColour(MAPPINGS_colour[0], MAPPINGS_colour[1], MAPPINGS_colour[2]);
    }
    else
    {
      controllerLEDTunoff.start();
      myGamepad->setColorLED(MAPPINGS_colour[0], MAPPINGS_colour[1], MAPPINGS_colour[2]);
    }

    myGamepad->setPlayerLEDs((MAPPINGS_INDEX + 1) & 0x0f);

    if (VAR_standby_mode && !VAR_disable_auto_boot)
    {
      BOOT_powerOn();
    }
  }
}

void onDisconnectedGamepad(GamepadPtr gp)
{
  if (myGamepad != nullptr && myGamepad == gp)
  {
    myGamepad = nullptr;

    if (VAR_standby_mode && !VAR_disable_auto_boot)
    {
      BOOT_powerOff();
    }
  }
}

void updateButtons(GamepadPtr gamepad)
{
  for (byte i = 0; i < MAPPINGS_pin_size; i++)
  {
    pinMap *mapping = &MAPPINGS_pins[i];

    switch (mapping->type)
    {
    case CTR_TYPE_DPAD:
      if (gamepad->dpad() & mapping->controllerMask)
      {
        PSP_press_button(mapping->pin);
      }
      break;
    case CTR_TYPE_BUTTONS:
      if (gamepad->buttons() & mapping->controllerMask)
      {
        PSP_press_button(mapping->pin);
      }
      break;
    case CTR_TYPE_SYSTEM:
      if (gamepad->miscButtons() & mapping->controllerMask)
      {
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

void handle_Trigger(pinMap *mapping)
{
  int triggerValue = mapping->controllerMask == CTR_BUTTON_THROTTLE ? myGamepad->throttle() : myGamepad->brake();

  if (triggerValue > VAR_analog_trigger_threshold)
  {
    PSP_press_button(mapping->pin);
  }
}

void handle_TS(pinMap *mapping, int xAxis, int yAxis)
{
  uint8_t activated = 0;

  switch (mapping->controllerMask)
  {
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

  if (activated)
  {
    PSP_press_button(mapping->pin);
  }
}

void setup()
{
  BP32.setup(&onConnectedGamepad, &onDisconnectedGamepad);

  BP32.enableNewBluetoothConnections(false);

  DIGIPOT_setup();
  LED_setup();
  CTRSYNC_setup();
  BOOT_setup();

  syncBtn.setDebounceTime(50);

  controllerLEDTunoff.setCallback([]() -> void { myGamepad->setColorLED(0, 0, 0); });

  controllerLEDTunoff.setTimeout(4000);

  shutdownTimer.setCallback([]() -> void { 
    PSP_press_start();
    delay(100);
    PSP_release_start();
    
    myGamepad->disconnect(); 
  });
  shutdownTimer.setTimeout(3000);

  if (!VAR_standby_mode && !VAR_disable_auto_boot)
  {
    BOOT_powerOn();
  }
}

void loop()
{
  syncBtn.loop();
  BP32.update();

  TimerManager::instance().update();

  if (syncBtn.isPressed())
  {
    if (!CTRSYNC_isActive())
    {
      CTRSYNC_start();
    }
    else
    {
      CTRSYNC_stop();
    }
  }

  if (BOOT_isBooting())
    return;

  PSP_mark_all_for_release();

  if (myGamepad && myGamepad->isConnected())
  {

    if (myGamepad->miscSystem() && !shutdownTimer.isRunning()) {
      shutdownTimer.start();
    } else if (!myGamepad->miscSystem() && shutdownTimer.isRunning()) {
      shutdownTimer.stop();

      PSP_press_home();
      delay(100);
      PSP_release_home();
    }

    // config mode
    if (myGamepad->thumbR())
    {
      //myGamepad->disconnect();      
      if (myGamepad->dpad() == CTR_DPAD_UP)
      {
        MAPPINGS_next_mapping();
        myGamepad->setRumble(100, 10);

        if (LEDScale == 0)
        {
          controllerLEDTunoff.start();
          myGamepad->setColorLED(MAPPINGS_colour[0], MAPPINGS_colour[1], MAPPINGS_colour[2]);
        }
        else
        {
          setScaledControllerColour(MAPPINGS_colour[0], MAPPINGS_colour[1], MAPPINGS_colour[2]);
        }

        myGamepad->setPlayerLEDs((MAPPINGS_INDEX + 1) & 0x0f);

        delay(500);
      }
      else if (myGamepad->dpad() == CTR_DPAD_DOWN)
      {
        controllerLEDTunoff.stop();

        // meh this is ugly
        if (LEDScale == 10)
          LEDScale = 4;
        else if (LEDScale == 4)
          LEDScale = 1;
        else if (LEDScale == 1)
          LEDScale = 0;
        else if (LEDScale == 0)
          LEDScale = 10;

        setScaledControllerColour(MAPPINGS_colour[0], MAPPINGS_colour[1], MAPPINGS_colour[2]);

        delay(500);
      }
    }
    else
    {
      updateButtons(myGamepad);
    }
  }

  PSP_release_unused();
}
