// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Ricardo Quesada
// http://retro.moe/unijoysticle2

#ifndef BT_SERVICE_H
#define BT_SERVICE_H

#include <stdbool.h>
#include "bt_service.gatt.h"
#include "Arduino.h"
#include <LittleFS.h>
#include "fileutility.h"
#include "constants.h"

void uni_bt_service_init(void);
void uni_bt_service_deinit(void);
bool uni_bt_service_is_enabled();
void uni_bt_service_set_enabled(bool enabled);

#endif  // BT_SERVICE_H
