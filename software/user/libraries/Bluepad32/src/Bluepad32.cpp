// Copyright 2021 - 2022, Ricardo Quesada, http://retro.moe
// SPDX-License-Identifier: Apache-2.0 or LGPL-2.1-or-later

#include "Bluepad32.h"

#include <Arduino.h>
#include <inttypes.h>

#include "constants.h"
#include "utility/debug.h"
#include "utility/spi_drv.h"
#include "utility/wl_types.h"

Bluepad32::Bluepad32()
    : _gamepads(), _prevConnectedGamepads(0), _onConnect(), _onDisconnect() {}

const char* Bluepad32::firmwareVersion() {
  // Max size of fwversion. Might not be possible to have an overflow since the
  // len is stored in a char. But fragile nonetheless.
  static char fwVersion[256];

  WAIT_FOR_SLAVE_SELECT();
  // Send Command
  SpiDrv::sendCmd(GET_FW_VERSION_CMD, PARAM_NUMS_0);

  SpiDrv::spiSlaveDeselect();
  // Wait the reply elaboration
  SpiDrv::waitForSlaveReady();
  SpiDrv::spiSlaveSelect();

  // Wait for reply
  uint8_t dataLen;
  if (!SpiDrv::waitResponseCmd(GET_FW_VERSION_CMD, PARAM_NUMS_1,
                               (uint8_t*)fwVersion, &dataLen)) {
    WARN("error waitResponse");
  }
  SpiDrv::spiSlaveDeselect();
  return fwVersion;
}

void Bluepad32::pinMode(uint8_t pin, uint8_t mode) {
  WAIT_FOR_SLAVE_SELECT();
  // Send Command
  SpiDrv::sendCmd(SET_PIN_MODE, PARAM_NUMS_2);
  SpiDrv::sendParam((uint8_t*)&pin, 1, NO_LAST_PARAM);
  SpiDrv::sendParam((uint8_t*)&mode, 1, LAST_PARAM);

  // pad to multiple of 4
  SpiDrv::readChar();

  SpiDrv::spiSlaveDeselect();
  // Wait the reply elaboration
  SpiDrv::waitForSlaveReady();
  SpiDrv::spiSlaveSelect();

  // Wait for reply
  uint8_t data, dataLen;
  if (!SpiDrv::waitResponseCmd(SET_PIN_MODE, PARAM_NUMS_1, &data, &dataLen)) {
    WARN("error waitResponse");
  }
  SpiDrv::spiSlaveDeselect();
}

int Bluepad32::digitalRead(uint8_t pin) {
  WAIT_FOR_SLAVE_SELECT();
  // Send Command
  SpiDrv::sendCmd(GET_DIGITAL_READ, PARAM_NUMS_1);
  SpiDrv::sendParam((uint8_t*)&pin, 1, LAST_PARAM);

  // pad to multiple of 4
  SpiDrv::readChar();
  SpiDrv::readChar();

  SpiDrv::spiSlaveDeselect();
  // Wait the reply elaboration
  SpiDrv::waitForSlaveReady();
  SpiDrv::spiSlaveSelect();

  // Wait for reply
  uint8_t data, dataLen;
  if (!SpiDrv::waitResponseCmd(GET_DIGITAL_READ, PARAM_NUMS_1, &data,
                               &dataLen)) {
    WARN("error waitResponse");
  }
  SpiDrv::spiSlaveDeselect();

  return !!data;
}

void Bluepad32::setDebug(uint8_t on) {
  WAIT_FOR_SLAVE_SELECT();

  // Send Command
  SpiDrv::sendCmd(SET_DEBUG_CMD, PARAM_NUMS_1);

  SpiDrv::sendParam(&on, 1, LAST_PARAM);

  // pad to multiple of 4
  SpiDrv::readChar();
  SpiDrv::readChar();

  SpiDrv::spiSlaveDeselect();
  // Wait the reply elaboration
  SpiDrv::waitForSlaveReady();
  SpiDrv::spiSlaveSelect();

  // Wait for reply
  uint8_t data, dataLen;
  SpiDrv::waitResponseCmd(SET_DEBUG_CMD, PARAM_NUMS_1, &data, &dataLen);

  SpiDrv::spiSlaveDeselect();
}

void Bluepad32::digitalWrite(uint8_t pin, uint8_t value) {
  WAIT_FOR_SLAVE_SELECT();
  // Send Command
  SpiDrv::sendCmd(SET_DIGITAL_WRITE, PARAM_NUMS_2);
  SpiDrv::sendParam((uint8_t*)&pin, 1, NO_LAST_PARAM);
  SpiDrv::sendParam((uint8_t*)&value, 1, LAST_PARAM);

  // pad to multiple of 4
  SpiDrv::readChar();

  SpiDrv::spiSlaveDeselect();
  // Wait the reply elaboration
  SpiDrv::waitForSlaveReady();
  SpiDrv::spiSlaveSelect();

  // Wait for reply
  uint8_t data, dataLen;
  if (!SpiDrv::waitResponseCmd(SET_DIGITAL_WRITE, PARAM_NUMS_1, &data,
                               &dataLen)) {
    WARN("error waitResponse");
  }
  SpiDrv::spiSlaveDeselect();
}

void Bluepad32::update() {
  WAIT_FOR_SLAVE_SELECT();
  // Send Command
  SpiDrv::sendCmd(BP32_GET_GAMEPADS_DATA, PARAM_NUMS_0);

  SpiDrv::spiSlaveDeselect();
  // Wait the reply elaboration
  SpiDrv::waitForSlaveReady();
  SpiDrv::spiSlaveSelect();

  // Wait for reply
  uint8_t totalParams;

  Gamepad::State currentState[BP32_MAX_GAMEPADS];

  SpiDrv::waitResponse(BP32_GET_GAMEPADS_DATA, &totalParams,
                       (uint8_t**)currentState, BP32_MAX_GAMEPADS);

  SpiDrv::spiSlaveDeselect();

  int connectedGamepads = 0;
  for (int i = 0; i < totalParams; i++) {
    connectedGamepads |= (1 << currentState[i].idx);

    // Update gamepads state
    _gamepads[currentState[i].idx]._state = currentState[i];
  }

  // No changes in connected gamepads. No need to call onConnected or
  // onDisconnected.
  if (connectedGamepads == _prevConnectedGamepads) return;

  // Compare bit by bit, and find which one got connected and which one
  // disconnected.
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    int bit = (1 << i);
    int current = connectedGamepads & bit;
    int prev = _prevConnectedGamepads & bit;

    // No changes in this gamepad, skip
    if (current == prev) continue;

    if (current) {
      _gamepads[i].onConnected();
      _onConnect(&_gamepads[i]);
      INFO("gamepad connected: %d", i);
    } else {
      _onDisconnect(&_gamepads[i]);
      _gamepads[i].onDisconnected();
      INFO("gamepad disconnected: %d", i);
    }
  }

  _prevConnectedGamepads = connectedGamepads;
}

void Bluepad32::forgetBluetoothKeys() {
  WAIT_FOR_SLAVE_SELECT();
  // Send Command
  SpiDrv::sendCmd(BP32_FORGET_BLUETOOTH_KEYS, PARAM_NUMS_0);

  SpiDrv::spiSlaveDeselect();
  // Wait the reply elaboration
  SpiDrv::waitForSlaveReady();
  SpiDrv::spiSlaveSelect();

  // Wait for reply
  uint8_t data, dataLen;
  if (!SpiDrv::waitResponseCmd(BP32_FORGET_BLUETOOTH_KEYS, PARAM_NUMS_1, &data,
                               &dataLen)) {
    WARN("error waitResponse");
  }
  SpiDrv::spiSlaveDeselect();
}

void Bluepad32::enableNewBluetoothConnections(bool enabled) {
  WAIT_FOR_SLAVE_SELECT();
  uint8_t en = enabled;

  // Send Command
  SpiDrv::sendCmd(BP32_ENABLE_BLUETOOTH_CONNECTIONS, PARAM_NUMS_1);

  SpiDrv::sendParam(&en, 1, LAST_PARAM);

  // pad to multiple of 4
  SpiDrv::readChar();
  SpiDrv::readChar();

  SpiDrv::spiSlaveDeselect();
  // Wait the reply elaboration
  SpiDrv::waitForSlaveReady();
  SpiDrv::spiSlaveSelect();

  // Wait for reply
  uint8_t data, dataLen;
  SpiDrv::waitResponseCmd(BP32_ENABLE_BLUETOOTH_CONNECTIONS, PARAM_NUMS_1,
                          &data, &dataLen);

  SpiDrv::spiSlaveDeselect();
}

void Bluepad32::setup(const GamepadCallback& onConnect,
                      const GamepadCallback& onDisconnect) {
  _onConnect = onConnect;
  _onDisconnect = onDisconnect;

  checkProtocol();
}

void Bluepad32::checkProtocol() {
  WAIT_FOR_SLAVE_SELECT();
  // Send Command
  SpiDrv::sendCmd(BP32_GET_PROTOCOL_VERSION, PARAM_NUMS_0);

  SpiDrv::spiSlaveDeselect();
  // Wait the reply elaboration
  SpiDrv::waitForSlaveReady();
  SpiDrv::spiSlaveSelect();

  // Wait for reply
  uint8_t dataLen;
  uint8_t data[2];
  if (!SpiDrv::waitResponseCmd(BP32_GET_PROTOCOL_VERSION, PARAM_NUMS_1, data,
                               &dataLen)) {
    WARN("error waitResponse");
  }

  SpiDrv::spiSlaveDeselect();

  if (dataLen != 2) {
    WARN("Invalid dataLen");
  }
  INFO("Protocol version: %d.%d", data[0], data[1]);
  _protocolVersionHi = data[0];
  _protocolVersionLow = data[1];
}

Bluepad32 BP32;