#include "interop.h"

enum {
    BTSTACKCMD_BLE_SERVICE_ENABLE,
    BTSTACKCMD_BLE_SERVICE_DISABLE
};

static btstack_context_callback_registration_t cmd_callback_registration;

static void cmd_callback(void* context) {
    Serial.println("CALLBACK");
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

void enableBLEService(bool enabled) {
    cmd_callback_registration.callback = &cmd_callback;
    cmd_callback_registration.context =
        (void*)(enabled ? (intptr_t)BTSTACKCMD_BLE_SERVICE_ENABLE : (intptr_t)BTSTACKCMD_BLE_SERVICE_DISABLE);

    btstack_run_loop_execute_on_main_thread(&cmd_callback_registration);
}

TaskHandle_t reloadMappingsTaskHandle;

enum {
	ARDUINOCMD_RELOAD_MAPPINGS
};

static void arduinocmd_callback(void* arg) {
	unsigned long ctx = (unsigned long)arg;
	uint16_t cmd = ctx & 0xffff;
	switch (cmd) {
		case ARDUINOCMD_RELOAD_MAPPINGS:
			MAPPINGS_setup();
		break;
	}

  vTaskDelete(reloadMappingsTaskHandle);
}

void reloadControllerMappings() {
    xTaskCreatePinnedToCore(arduinocmd_callback, "arduinocmd_callback", 8192, (void*)ARDUINOCMD_RELOAD_MAPPINGS, 1, &reloadMappingsTaskHandle, 1);
}