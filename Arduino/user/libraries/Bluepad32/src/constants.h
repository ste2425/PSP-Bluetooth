// Copyright 2021 - 2022, Ricardo Quesada, http://retro.moe
// SPDX-License-Identifier: Apache-2.0 or LGPL-2.1-or-later

#ifndef BP32_CONSTANTS_H
#define BP32_CONSTANTS_H

enum {
  BP32_GET_PROTOCOL_VERSION = 0x00,
  BP32_GET_GAMEPADS_DATA = 0x01,
  BP32_SET_GAMEPAD_PLAYERS_LED = 0x02,
  BP32_SET_GAMEPAD_COLOR_LED = 0x03,
  BP32_SET_GAMEPAD_RUMBLE = 0x04,
  BP32_FORGET_BLUETOOTH_KEYS = 0x05,
  BP32_GET_GAMEPAD_PROPERTIES = 0x06,
  BP32_ENABLE_BLUETOOTH_CONNECTIONS = 0x07,
};

// Possible answers when the request doesn't need an answer, like in "set_xxx"
enum {
  BP32_RESPONSE_ERROR = 0,
  BP32_RESPONSE_OK = 1,
};

enum {
  BP32_PROTOCOL_VERSION_HI = 0x01,
  BP32_PROTOCOL_VERSION_LO = 0x02,
};

#define BP32_MAX_GAMEPADS 4

#endif  // BP32_CONSTANTS_H