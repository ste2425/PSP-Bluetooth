// Copyright 2021 - 2021, Ricardo Quesada, http://retro.moe
// SPDX-License-Identifier: Apache-2.0 or LGPL-2.1-or-later

#include "Gamepad.h"

#include <inttypes.h>

#include "Bluepad32.h"
#include "constants.h"
#include "utility/debug.h"
#include "utility/spi_drv.h"
#include "utility/wl_types.h"

Gamepad::Gamepad() : _connected(false), _state(), _properties() {}

bool Gamepad::isConnected() const { return _connected; }

void Gamepad::setPlayerLEDs(uint8_t led) const {
  if (!isConnected()) {
    WARN("gamepad not connected");
    return;
  }

  WAIT_FOR_SLAVE_SELECT();
  // Send Command
  SpiDrv::sendCmd(BP32_SET_GAMEPAD_PLAYERS_LED, PARAM_NUMS_2);
  SpiDrv::sendParam((uint8_t*)&_state.idx, 1, NO_LAST_PARAM);
  SpiDrv::sendParam((uint8_t*)&led, 1, LAST_PARAM);

  // pad to multiple of 4
  SpiDrv::readChar();

  SpiDrv::spiSlaveDeselect();
  // Wait the reply elaboration
  SpiDrv::waitForSlaveReady();
  SpiDrv::spiSlaveSelect();

  // Wait for reply
  uint8_t data, dataLen;
  if (!SpiDrv::waitResponseCmd(BP32_SET_GAMEPAD_PLAYERS_LED, PARAM_NUMS_1,
                               &data, &dataLen)) {
    WARN("error waitResponse");
  }
  SpiDrv::spiSlaveDeselect();

  if (data != BP32_RESPONSE_OK || dataLen != 1) {
    WARN("Failed to set players led");
  }
}

void Gamepad::setColorLED(uint8_t red, uint8_t green, uint8_t blue) const {
  if (!isConnected()) {
    WARN("gamepad not connected");
    return;
  }

  uint8_t colors[3] = {red, green, blue};

  WAIT_FOR_SLAVE_SELECT();
  // Send Command
  SpiDrv::sendCmd(BP32_SET_GAMEPAD_COLOR_LED, PARAM_NUMS_4);
  SpiDrv::sendParam((uint8_t*)&_state.idx, 1, NO_LAST_PARAM);
  SpiDrv::sendParam(colors, 3, LAST_PARAM);

  // pad to multiple of 4
  SpiDrv::readChar();
  SpiDrv::readChar();
  SpiDrv::readChar();

  SpiDrv::spiSlaveDeselect();
  // Wait the reply elaboration
  SpiDrv::waitForSlaveReady();
  SpiDrv::spiSlaveSelect();

  // Wait for reply
  uint8_t data, dataLen;
  if (!SpiDrv::waitResponseCmd(BP32_SET_GAMEPAD_COLOR_LED, PARAM_NUMS_1, &data,
                               &dataLen)) {
    WARN("error waitResponse");
  }
  SpiDrv::spiSlaveDeselect();

  if (data != BP32_RESPONSE_OK || dataLen != 1) {
    WARN("Failed to set color led");
  }
}

void Gamepad::setRumble(uint8_t force, uint8_t duration) const {
  if (!isConnected()) {
    WARN("gamepad not connected");
    return;
  }

  uint8_t rumble[2] = {force, duration};

  WAIT_FOR_SLAVE_SELECT();
  // Send Command
  SpiDrv::sendCmd(BP32_SET_GAMEPAD_RUMBLE, PARAM_NUMS_3);
  SpiDrv::sendParam((uint8_t*)&_state.idx, 1, NO_LAST_PARAM);
  SpiDrv::sendParam(rumble, 2, LAST_PARAM);

  // already padded to multiple of 4

  SpiDrv::spiSlaveDeselect();
  // Wait the reply elaboration
  SpiDrv::waitForSlaveReady();
  SpiDrv::spiSlaveSelect();

  // Wait for reply
  uint8_t data, dataLen;
  if (!SpiDrv::waitResponseCmd(BP32_SET_GAMEPAD_RUMBLE, PARAM_NUMS_1, &data,
                               &dataLen)) {
    WARN("error waitResponse");
  }

  if (data != BP32_RESPONSE_OK || dataLen != 1) {
    WARN("Failed to set rumble");
  }
  SpiDrv::spiSlaveDeselect();
}

void Gamepad::disconnect() {
  if (!isConnected()) {
    WARN("gamepad not connected");
    return;
  }

  WAIT_FOR_SLAVE_SELECT();
  // Send Command
  SpiDrv::sendCmd(BP32_DISCONNECT_GAMEPAD, PARAM_NUMS_1);
  SpiDrv::sendParam((uint8_t*)&_state.idx, 1, LAST_PARAM);

  // pad to multiple of 4
  SpiDrv::readChar();
  SpiDrv::readChar();

  SpiDrv::spiSlaveDeselect();
  // Wait the reply elaboration
  SpiDrv::waitForSlaveReady();
  SpiDrv::spiSlaveSelect();

  // Wait for reply
  uint8_t data, dataLen;
  if (!SpiDrv::waitResponseCmd(BP32_DISCONNECT_GAMEPAD, PARAM_NUMS_1, &data,
                               &dataLen)) {
    WARN("error waitResponse");
  }
  SpiDrv::spiSlaveDeselect();

  if (data != BP32_RESPONSE_OK || dataLen != 1) {
    WARN("Failed to disconnect");
  }
}

// Private functions

void Gamepad::onConnected() {
  _connected = true;

  // Properties are static, they don't change during the "life of the
  // connection". Request them once, at connect time and cache the result.
  // FIXME: _properties.idx contains the error code in case it fails.
  uint8_t errorCode;
  tParam params[] = {
      {sizeof(errorCode), (char*)&errorCode},
      {sizeof(_properties), (char*)&_properties},
  };

  // Requires protocol version 1.1 at least.
  if (BP32._protocolVersionHi <= 1 && BP32._protocolVersionLow < 1) {
    WARN("Requires protocol version 1.1. Upgrade ESP32 firmware");
    _properties.idx = -1;
    return;
  }

  WAIT_FOR_SLAVE_SELECT();
  // Send Command
  SpiDrv::sendCmd(BP32_GET_GAMEPAD_PROPERTIES, PARAM_NUMS_1);
  SpiDrv::sendParam((uint8_t*)&_state.idx, 1, LAST_PARAM);

  SpiDrv::spiSlaveDeselect();
  // Wait the reply elaboration
  SpiDrv::waitForSlaveReady();
  SpiDrv::spiSlaveSelect();

  // Wait for reply
  if (!SpiDrv::waitResponseParams(BP32_GET_GAMEPAD_PROPERTIES, PARAM_NUMS_2,
                                  params)) {
    WARN("error waitResponseParams");
    _properties.idx = -2;
    return;
  }

  SpiDrv::spiSlaveDeselect();

  if (errorCode != BP32_RESPONSE_OK) {
    WARN("Failed to get gamepad properties");
    _properties.idx = -3;
  }
}

void Gamepad::onDisconnected() {
  _connected = false;
  _properties = {};
}