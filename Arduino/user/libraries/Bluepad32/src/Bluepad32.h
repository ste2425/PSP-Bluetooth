// Copyright 2021 - 2022, Ricardo Quesada, http://retro.moe
// SPDX-License-Identifier: Apache 2.0 or LGPL-2.1-or-later

#ifndef BP32_BLUEPAD32_H
#define BP32_BLUEPAD32_H

#include <inttypes.h>

#include "Gamepad.h"
#include "GamepadProperties.h"
#include "constants.h"

// Using C callbacks since AVR-GCC (needed for Arduino UNO WiFi) doesn't support
// STL
typedef void (*GamepadCallback)(GamepadPtr gamepad);

class Bluepad32 {
  // This is what the user receives
  Gamepad _gamepads[BP32_MAX_GAMEPADS];

  // This is used internally by SPI, and then copied into the Gamepad::State of
  // each gamepad
  int _prevConnectedGamepads;

  GamepadCallback _onConnect;
  GamepadCallback _onDisconnect;

 public:
  Bluepad32();
  /*
   * Get the firmware version
   * result: version as string with this format a.b.c
   */
  const char* firmwareVersion();
  void setDebug(uint8_t on);

  // GPIOs: This is from NINA fw, and might be needed on some boards.
  // E.g.: Arduino Nano RP2040 Connect requires them to turn on/off the RGB LED,
  // that is controlled from the ESP32.
  void pinMode(uint8_t pin, uint8_t mode);
  int digitalRead(uint8_t pin);
  void digitalWrite(uint8_t pin, uint8_t value);

  // Gamepad
  void update();

  // When a gamepad connects to the ESP32, the ESP32 stores keys to make it
  // easier the reconnection.
  // If you want to "forget" (delete) the keys from ESP32, you should call this
  // function.
  void forgetBluetoothKeys();

  // Enable / Disable new Bluetooth connections.
  // When enabled, the device is put in Discovery mode, and new pairs are accepted.
  // When disabled, only devices that have paired before can connect.
  // Established connections are not affected.
  void enableNewBluetoothConnections(bool enabled);

  void setup(const GamepadCallback& onConnect,
             const GamepadCallback& onDisconnect);

 private:
  void checkProtocol();

  uint8_t _protocolVersionHi;
  uint8_t _protocolVersionLow;

  friend class Gamepad;
};

extern Bluepad32 BP32;

#endif  // BP32_BLUEPAD32_H
