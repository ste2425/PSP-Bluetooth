// Copyright 2021 - 2021, Ricardo Quesada, http://retro.moe
// SPDX-License-Identifier: Apache-2.0 or LGPL-2.1-or-later

#ifndef BP32_GAMEPAD_H
#define BP32_GAMEPAD_H

#include <inttypes.h>

#include "GamepadProperties.h"

class Gamepad {
 public:
  // Types of controllers
  // Taken from here, with Bluepad32 additions:
  // http://hg.libsdl.org/SDL/file/28fcb5ef7ff1/src/joystick/controller_type.h
  enum {
    CONTROLLER_TYPE_None = -1,
    CONTROLLER_TYPE_Unknown = 0,

    // Steam Controllers
    CONTROLLER_TYPE_UnknownSteamController = 1,
    CONTROLLER_TYPE_SteamController = 2,
    CONTROLLER_TYPE_SteamControllerV2 = 3,

    // Other Controllers
    CONTROLLER_TYPE_UnknownNonSteamController = 30,
    CONTROLLER_TYPE_XBox360Controller = 31,
    CONTROLLER_TYPE_XBoxOneController = 32,
    CONTROLLER_TYPE_PS3Controller = 33,
    CONTROLLER_TYPE_PS4Controller = 34,
    CONTROLLER_TYPE_WiiController = 35,
    CONTROLLER_TYPE_AppleController = 36,
    CONTROLLER_TYPE_AndroidController = 37,
    CONTROLLER_TYPE_SwitchProController = 38,
    CONTROLLER_TYPE_SwitchJoyConLeft = 39,
    CONTROLLER_TYPE_SwitchJoyConRight = 40,
    CONTROLLER_TYPE_SwitchJoyConPair = 41,
    CONTROLLER_TYPE_SwitchInputOnlyController = 42,
    CONTROLLER_TYPE_MobileTouch = 43,
    CONTROLLER_TYPE_XInputSwitchController =
        44,  // Client-side only, used to mark Switch-compatible controllers as
             // not supporting Switch controller protocol
    CONTROLLER_TYPE_PS5Controller = 45,

    // Bluepad32 own extensions
    CONTROLLER_TYPE_iCadeController = 50,          // (Bluepad32)
    CONTROLLER_TYPE_SmartTVRemoteController = 51,  // (Bluepad32)
    CONTROLLER_TYPE_EightBitdoController = 52,     // (Bluepad32)
    CONTROLLER_TYPE_GenericController = 53,        // (Bluepad32)
    CONTROLLER_TYPE_NimbusController = 54,         // (Bluepad32)
    CONTROLLER_TYPE_OUYAController = 55,           // (Bluepad32)

    CONTROLLER_TYPE_LastController,  // Don't add game controllers below this
                                     // enumeration - this enumeration can
                                     // change value

    // Keyboards and Mice
    CONTROLLER_TYPE_GenericKeyboard = 400,
    CONTROLLER_TYPE_GenericMouse = 800,
  };

  // DPAD
  enum {
    DPAD_UP = 1 << 0,
    DPAD_DOWN = 1 << 1,
    DPAD_RIGHT = 1 << 2,
    DPAD_LEFT = 1 << 3,
  };

  // BUTTON_ are the main gamepad buttons, like X, Y, A, B, etc.
  enum {
    BUTTON_A = 1 << 0,
    BUTTON_B = 1 << 1,
    BUTTON_X = 1 << 2,
    BUTTON_Y = 1 << 3,
    BUTTON_SHOULDER_L = 1 << 4,
    BUTTON_SHOULDER_R = 1 << 5,
    BUTTON_TRIGGER_L = 1 << 6,
    BUTTON_TRIGGER_R = 1 << 7,
    BUTTON_THUMB_L = 1 << 8,
    BUTTON_THUMB_R = 1 << 9,
  };

  // MISC_BUTTONS_ are buttons that are usually not used in the game, but are
  // helpers like "back", "home", etc.
  enum {
    MISC_BUTTON_SYSTEM = 1 << 0,  // AKA: PS, Xbox, etc.
    MISC_BUTTON_BACK = 1 << 1,    // AKA: Select, Share, -
    MISC_BUTTON_HOME = 1 << 2,    // AKA: Start, Options, +
  };

  Gamepad();

  // Must match nina_gamepad_t defined here:
  // https://gitlab.com/ricardoquesada/bluepad32/-/blob/main/src/components/bluepad32/uni_platform_nina.c
  struct State {
    // Used to tell "controller" who is the owner of this data. 4 gamepads can
    // be connected, this value indicates which gamepad it is.
    uint8_t idx;

    uint8_t dpad;
    int32_t axisX;
    int32_t axisY;
    int32_t axisRX;
    int32_t axisRY;

    int32_t brake;
    int32_t throttle;

    uint16_t buttons;
    uint8_t miscButtons;

    // Type of controller: PS4, PS5, Wii, Xbox One, etc...
    uint8_t type;
  } __attribute__((packed));

  uint8_t dpad() const { return _state.dpad; }

  // To test one button at the time.
  bool a() const { return _state.buttons & BUTTON_A; }
  bool b() const { return _state.buttons & BUTTON_B; }
  bool x() const { return _state.buttons & BUTTON_X; }
  bool y() const { return _state.buttons & BUTTON_Y; }
  bool l1() const { return _state.buttons & BUTTON_SHOULDER_L; }
  bool l2() const { return _state.buttons & BUTTON_TRIGGER_L; }
  bool r1() const { return _state.buttons & BUTTON_SHOULDER_R; }
  bool r2() const { return _state.buttons & BUTTON_TRIGGER_R; }
  bool thumbL() const { return _state.buttons & BUTTON_THUMB_L; }
  bool thumbR() const { return _state.buttons & BUTTON_THUMB_R; }

  // Returns the state of all buttons.
  uint16_t buttons() const { return _state.buttons; }

  // Returns the state of all misc buttons.
  uint16_t miscButtons() const { return _state.miscButtons; }

  // Axis
  int32_t axisX() const { return _state.axisX; }
  int32_t axisY() const { return _state.axisY; }
  int32_t axisRX() const { return _state.axisRX; }
  int32_t axisRY() const { return _state.axisRY; }

  // Brake & Throttle
  int32_t brake() const { return _state.brake; }
  int32_t throttle() const { return _state.throttle; }

  // Misc buttons
  bool miscSystem() const { return _state.miscButtons & MISC_BUTTON_SYSTEM; }
  bool miscBack() const { return _state.miscButtons & MISC_BUTTON_BACK; }
  bool miscHome() const { return _state.miscButtons & MISC_BUTTON_HOME; }

  bool isConnected() const;

  // Returns the gamepad model.
  int getModel() const { return _state.type; }
  // Returns gamepad properties. Available since protocol v1.1.
  GamepadProperties getProperties() const { return _properties; }

  // "Output" functions.
  void setPlayerLEDs(uint8_t led) const;
  void setColorLED(uint8_t red, uint8_t green, uint8_t blue) const;
  void setRumble(uint8_t force, uint8_t duration) const;

 private:
  void onConnected();
  void onDisconnected();

  bool _connected;
  State _state;
  GamepadProperties _properties;

  // Delete copy constructor to avoid copying the state by mistake. If so,
  // chances are that the gamepad won't get updated automatically.
  Gamepad(const Gamepad&) = delete;

  friend class Bluepad32;
};

typedef Gamepad* GamepadPtr;

#endif  // BP32_GAMEPAD_H
