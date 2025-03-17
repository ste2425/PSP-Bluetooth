#include "interop.h"

#define ms_per_min 60000

enum {
    BTSTACKCMD_BLE_SERVICE_ENABLE,
    BTSTACKCMD_BLE_SERVICE_DISABLE
};

static btstack_context_callback_registration_t cmd_callback_registration;

Timeout disableBLETimeout;

static void cmd_callback(void* context) {
    unsigned long ctx = (unsigned long)context;
    uint16_t cmd = ctx & 0xffff;
    switch (cmd) {
        case BTSTACKCMD_BLE_SERVICE_ENABLE:
            uni_bt_service_set_enabled(true);
            break;
        case BTSTACKCMD_BLE_SERVICE_DISABLE:
            uni_bt_service_set_enabled(false);
            break;
    }
}

bool bleEnabled = false;

bool INTEROP_bleServiceEnabled() {
    return bleEnabled;
}

void INTEROP_toggleBLEService() {
    if (bleEnabled) {
        INTEROP_enableBLEService(false);
    } else {
        INTEROP_enableBLEService(true);
    }
}

void INTEROP_enableBLEService(bool enabled) {
    if (enabled)
        disableBLETimeout.start();
    else
        disableBLETimeout.stop();

    bleEnabled = enabled;
    cmd_callback_registration.callback = &cmd_callback;
    cmd_callback_registration.context =
        (void*)(enabled ? (intptr_t)BTSTACKCMD_BLE_SERVICE_ENABLE : (intptr_t)BTSTACKCMD_BLE_SERVICE_DISABLE);

    btstack_run_loop_execute_on_main_thread(&cmd_callback_registration);
}

TaskHandle_t core1CommandTaskHandle;

enum {
	ARDUINOCMD_RELOAD_MAPPINGS,
	ARDUINOCMD_RELOAD_SETTINGS,
    ARDUINOCMD_START_BLE_TIMEOUT,
    ARDUINOCMD_STOP_BLE_TIMEOUT,
    ARDUINOCMD_PREPARE_OTA_APPLY
};

static void arduinocmd_callback(void* arg) {
    unsigned long ctx = (unsigned long)arg;
    uint16_t cmd = ctx & 0xffff;
    switch (cmd) {
        case ARDUINOCMD_RELOAD_MAPPINGS:
            Serial.println("Reloading mappings");
            MAPPINGS_setup();
            CTRMANAGER_applyColours();
            break;
        case ARDUINOCMD_RELOAD_SETTINGS:
            SETTINGS_setup();
            LED_autoSet();
            CTRMANAGER_updateSettings();
            break;
        case ARDUINOCMD_START_BLE_TIMEOUT:
            disableBLETimeout.start();
            break;
        case ARDUINOCMD_STOP_BLE_TIMEOUT:
            disableBLETimeout.stop();
            break;
        case ARDUINOCMD_PREPARE_OTA_APPLY:
            CTRMANAGER_disconnectAll();

            if (PSPstate_poweredOn())
                PSPState_togglePower();
                
            delay(2000);
            esp_restart();
            
            break;
    }

  vTaskDelete(core1CommandTaskHandle);
}

void INTEROP_prepareOTAApply() {
    xTaskCreatePinnedToCore(arduinocmd_callback, "arduinocmd_callback", 4096, (void*)ARDUINOCMD_PREPARE_OTA_APPLY, 5, &core1CommandTaskHandle, 1);
}

void INTEROP_reloadControllerMappings() {
    xTaskCreatePinnedToCore(arduinocmd_callback, "arduinocmd_callback", 4096, (void*)ARDUINOCMD_RELOAD_MAPPINGS, 5, &core1CommandTaskHandle, 1);
}

void INTEROP_reloadSettings() {
    xTaskCreatePinnedToCore(arduinocmd_callback, "arduinocmd_callback", 4096, (void*)ARDUINOCMD_RELOAD_SETTINGS, 5, &core1CommandTaskHandle, 1);
}

void INTEROP_enableBLEInactivityWatcher() {
    Serial.println("ENABLE BLE WATCHER");
    xTaskCreatePinnedToCore(arduinocmd_callback, "arduinocmd_callback", 4096, (void*)ARDUINOCMD_START_BLE_TIMEOUT, 5, &core1CommandTaskHandle, 1);
}

void INTEROP_disableBLEInactivityWatcher() {
    Serial.println("DISABLE BLE WATCHER");
    xTaskCreatePinnedToCore(arduinocmd_callback, "arduinocmd_callback", 4096, (void*)ARDUINOCMD_STOP_BLE_TIMEOUT, 5, &core1CommandTaskHandle, 1);
}

void onBLETimeout() {
    Serial.println("DISANLE BRO");
    INTEROP_enableBLEService(false);
    // TODO - This probably shouldnt be here.
    LED_autoSet();
}

void INTEROP_setup() {
    disableBLETimeout.setCallback(onBLETimeout);
    // 5 min 300000 . 2.5 min 150000
    disableBLETimeout.setTimeout(150000);
}

void INTEROP_loop() {
    disableBLETimeout.loop();
}