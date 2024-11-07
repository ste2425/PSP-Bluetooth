// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Ricardo Quesada
// http://retro.moe/unijoysticle2

#include "bt_service.h"

#include <btstack.h>

typedef enum {
    // Order should not be changed.
    // Unused entries should not be removed.
    CONTROLLER_SUBTYPE_NONE = 0,
    CONTROLLER_SUBTYPE_WIIMOTE_HORIZONTAL,
    CONTROLLER_SUBTYPE_WIIMOTE_VERTICAL,
    CONTROLLER_SUBTYPE_WIIMOTE_ACCEL,
    CONTROLLER_SUBTYPE_WIIMOTE_NUNCHUK,
    CONTROLLER_SUBTYPE_UNUSED_00,
    CONTROLLER_SUBTYPE_WIIMOTE_NUNCHUK_ACCEL,
    CONTROLLER_SUBTYPE_WII_CLASSIC,
    CONTROLLER_SUBTYPE_WIIUPRO,
    CONTROLLER_SUBTYPE_WII_BALANCE_BOARD,
    CONTROLLER_SUBTYPE_WIIMOTE_UDRAW_TABLET,

    // Each gamepad should have its own range; this is to have binary compatibility
    // E.g: NINA users might depend on some hardcoded values
    CONTROLLER_SUBTYPE_EXAMPLE_OF_NEW_GAMEPAD = 20,

} uni_controller_subtype_t;

// General Discoverable = 0x02
// BR/EDR Not supported = 0x04
#define APP_AD_FLAGS 0x06

// Max number of clients that can connect to the service at the same time.
#define MAX_NR_CLIENT_CONNECTIONS 1

// Don't know how to increate MTU for notification, so use the minimum which is 20 (23 - 3)
#define NOTIFICATION_MTU 20

// Struct sent to the BLE client
// A compact version of uni_hid_device_t.
typedef struct __attribute((packed)) {
    uint8_t idx;          // device index number: 0...CONFIG_BLUEPAD32_MAX_DEVICES-1
    bd_addr_t addr;       // 6 bytes
    uint16_t vendor_id;   // 2 bytes
    uint16_t product_id;  // 2 bytes
    uint8_t state;
    uint8_t incoming;
    uint16_t controller_type;
    uni_controller_subtype_t controller_subtype;
} compact_device_t;
_Static_assert(sizeof(compact_device_t) <= NOTIFICATION_MTU, "compact_device_t too big");

// client connection
typedef struct {
    bool notification_enabled;
    uint16_t value_handle;
    hci_con_handle_t connection_handle;
} client_connection_t;
static client_connection_t client_connections[MAX_NR_CLIENT_CONNECTIONS];

// Iterate all over the connected clients, but only one is supported. Hardcoded to 0, don't change.
static int notification_connection_idx;
// Iterate all over the connected controllers
static int notification_device_idx;

static compact_device_t compact_devices[CONFIG_BLUEPAD32_MAX_DEVICES];
static bool service_enabled;

// clang-format off
static const uint8_t adv_data[] = {
    // Flags general discoverable
    2, BLUETOOTH_DATA_TYPE_FLAGS, APP_AD_FLAGS,
    // Name
    5, BLUETOOTH_DATA_TYPE_COMPLETE_LOCAL_NAME,'B', 'P', '3', '2',
    // 4627C4A4-AC00-46B9-B688-AFC5C1BF7F63
    17, BLUETOOTH_DATA_TYPE_COMPLETE_LIST_OF_128_BIT_SERVICE_CLASS_UUIDS,
    0x63, 0x7F, 0xBF, 0xC1, 0xC5, 0xAF, 0x88, 0xB6, 0xB9, 0x46, 0x00, 0xAC, 0xA4, 0xC4, 0x27, 0x46,
};
_Static_assert(sizeof(adv_data) <= 31, "adv_data too big");
// clang-format on
static const int adv_data_len = sizeof(adv_data);

static void att_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t* packet, uint16_t size);
static int att_write_callback(hci_con_handle_t con_handle,
                              uint16_t att_handle,
                              uint16_t transaction_mode,
                              uint16_t offset,
                              uint8_t* buffer,
                              uint16_t buffer_size);
static uint16_t att_read_callback(hci_con_handle_t conn_handle,
                                  uint16_t att_handle,
                                  uint16_t offset,
                                  uint8_t* buffer,
                                  uint16_t buffer_size);
static client_connection_t* connection_for_conn_handle(hci_con_handle_t conn_handle);
static bool next_notify_device(void);
static void notify_client(void);
static void maybe_notify_client();

static bool is_notify_client_valid(void) {
    return ((client_connections[notification_connection_idx].connection_handle != HCI_CON_HANDLE_INVALID) &&
            (client_connections[notification_connection_idx].notification_enabled));
}

static bool next_notify_device(void) {
    // TODO: For simplicity, we send all 4 devices.
    // But we should only send the connected ones.
    notification_device_idx++;
    if (notification_device_idx == CONFIG_BLUEPAD32_MAX_DEVICES) {
        notification_device_idx = 0;
        return true;
    }
    return false;
}

static void notify_client(void) {
    Serial.println("Notifying client");
    uint8_t status;
    client_connection_t* ctx;
    bool finish_round;

    if (!is_notify_client_valid())
        return;

    ctx = &client_connections[notification_connection_idx];

    // send
    status = att_server_notify(ctx->connection_handle, ctx->value_handle,
                               (const uint8_t*)&compact_devices[notification_device_idx], sizeof(compact_devices[0]));
    if (status != ERROR_CODE_SUCCESS) {
        Serial.println("BLE Service: Failed to notify client");
    }

    finish_round = next_notify_device();
    if (!finish_round)
        att_server_request_can_send_now_event(ctx->connection_handle);
}

static void maybe_notify_client(void) {
    client_connection_t* ctx = NULL;

    for (int i = 0; i < MAX_NR_CLIENT_CONNECTIONS; i++) {
        if (client_connections[i].connection_handle != HCI_CON_HANDLE_INVALID &&
            client_connections[i].notification_enabled) {
            ctx = &client_connections[i];
            break;
        }
    }
    if (ctx)
        att_server_request_can_send_now_event(ctx->connection_handle);
}

static int att_write_callback(hci_con_handle_t con_handle,
                              uint16_t att_handle,
                              uint16_t transaction_mode,
                              uint16_t offset,
                              uint8_t* buffer,
                              uint16_t buffer_size) {
    UNUSED(transaction_mode);

    Serial.println("att_write_callback");

    client_connection_t* ctx;

    switch (att_handle) {
        case ATT_CHARACTERISTIC_4627C4A4_AC03_46B9_B688_AFC5C1BF7F63_01_VALUE_HANDLE: {
            // Whether to enable BLE connections
           // if (buffer_size != 1 || offset != 0)
            //    return ATT_ERROR_REQUEST_NOT_SUPPORTED;

            Serial.println("Enabled: ");
            Serial.println((const char*)buffer);

            FileUtility::writeFile(LittleFS, "/mapping.json", (const char*)buffer);

            //reload_mappings_safe();
            //uni_bt_le_set_enabled(enabled);
            return ATT_ERROR_SUCCESS;
        }
        case ATT_CHARACTERISTIC_4627C4A4_AC04_46B9_B688_AFC5C1BF7F63_01_VALUE_HANDLE: {

            Serial.println("OTA Command: ");
            Serial.println(*buffer);

            //uni_bt_le_set_enabled(enabled);
            return ATT_ERROR_SUCCESS;
        }
        default:
            Serial.println("BLE Service: Unsupported write to");
            break;
    }
    return ATT_ERROR_SUCCESS;
}

static uint16_t att_read_callback(hci_con_handle_t conn_handle,
                                  uint16_t att_handle,
                                  uint16_t offset,
                                  uint8_t* buffer,
                                  uint16_t buffer_size) {
    UNUSED(conn_handle);

    switch (att_handle) {
        case ATT_CHARACTERISTIC_4627C4A4_AC03_46B9_B688_AFC5C1BF7F63_01_VALUE_HANDLE: {
            String lol = FileUtility::readFile(LittleFS, "/mapping.json");
            auto len = lol.length() + 1;
            char dataArr[len];
            lol.toCharArray(dataArr, len);
            return att_read_callback_handle_blob((const uint8_t *)dataArr, len, offset, buffer, buffer_size);
        }
        case ATT_CHARACTERISTIC_4627C4A4_AC01_46B9_B688_AFC5C1BF7F63_01_VALUE_HANDLE: {
            // Version
            return att_read_callback_handle_blob((const uint8_t *)PSP_BLUETOOTH_VERSION, strlen(PSP_BLUETOOTH_VERSION), offset, buffer, buffer_size);
        }
        default:
            break;
    }
    return 0;
}

static client_connection_t* connection_for_conn_handle(hci_con_handle_t conn_handle) {
    int i;
    for (i = 0; i < MAX_NR_CLIENT_CONNECTIONS; i++) {
        if (client_connections[i].connection_handle == conn_handle)
            return &client_connections[i];
    }
    return NULL;
}

static void att_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t* packet, uint16_t size) {
    UNUSED(channel);
    UNUSED(size);

    client_connection_t* ctx;
    int mtu;

    if (packet_type != HCI_EVENT_PACKET)
        return;

    switch (hci_event_packet_get_type(packet)) {
        case ATT_EVENT_CONNECTED:
            // setup new
            ctx = connection_for_conn_handle(HCI_CON_HANDLE_INVALID);
            if (!ctx)
                break;
            ctx->connection_handle = att_event_connected_get_handle(packet);
            mtu = att_server_get_mtu(ctx->connection_handle);
            Serial.println("BLE Service: New client connected handle");
            break;
        case ATT_EVENT_MTU_EXCHANGE_COMPLETE:
            mtu = att_event_mtu_exchange_complete_get_MTU(packet) - 3;
            ctx = connection_for_conn_handle(att_event_mtu_exchange_complete_get_handle(packet));
            if (!ctx)
                break;
            break;
        case ATT_EVENT_CAN_SEND_NOW:
            notify_client();
            break;
        case ATT_EVENT_DISCONNECTED:
            ctx = connection_for_conn_handle(att_event_disconnected_get_handle(packet));
            if (!ctx)
                break;
            Serial.println("BLE Service: client disconnected");
            memset(ctx, 0, sizeof(*ctx));
            ctx->connection_handle = HCI_CON_HANDLE_INVALID;
            break;
        case HCI_EVENT_DISCONNECTION_COMPLETE:
            // Do something?
            break;
        case HCI_EVENT_LE_META:
            switch (hci_event_le_meta_get_subevent_code(packet)) {
                case HCI_SUBEVENT_LE_CONNECTION_COMPLETE:
                    // Deprecated. Replaced by ATT_EVENT_CONNECTED
                    break;
                default:
                    Serial.println("Unsupported HCI_EVENT_LE_META");
                    break;
            }
            break;
        default:
            Serial.println("BLE Service: Unsupported ATT_EVENT");
            break;
    }
}

void uni_bt_service_deinit(void) {
    att_server_deinit();
    gap_advertisements_enable(false);
}

/*
 * Configures the ATT Server with the pre-compiled ATT Database generated from the .gatt file.
 * Finally, it configures the advertisements.
 */
void uni_bt_service_init(void) {
    Serial.println("Starting Bluepad32 BLE service UUID: 4627C4A4-AC00-46B9-B688-AFC5C1BF7F63\n");

    // Setup ATT server.
    att_server_init(profile_data, att_read_callback, att_write_callback);

    // setup advertisements
    uint16_t adv_int_min = 0x0030;
    uint16_t adv_int_max = 0x0030;
    uint8_t adv_type = 0;
    bd_addr_t null_addr;

    memset(null_addr, 0, 6);
    memset(compact_devices, 0, sizeof(compact_devices));
    memset(&client_connections, 0, sizeof(client_connections));
    for (int i = 0; i < MAX_NR_CLIENT_CONNECTIONS; i++)
        client_connections[i].connection_handle = HCI_CON_HANDLE_INVALID;
    for (int i = 0; i < CONFIG_BLUEPAD32_MAX_DEVICES; i++)
        compact_devices[i].idx = i;

    // register for ATT events
    att_server_register_packet_handler(att_packet_handler);

    gap_advertisements_set_params(adv_int_min, adv_int_max, adv_type, 0, null_addr, 0x07, 0x00);
    gap_advertisements_set_data(adv_data_len, (uint8_t*)adv_data);
    gap_advertisements_enable(true);
}

bool uni_bt_service_is_enabled() {
    return service_enabled;
}

void uni_bt_service_set_enabled(bool enabled) {
    if (enabled == service_enabled)
        return;

    service_enabled = enabled;

    if (service_enabled)
        uni_bt_service_init();
    else
        uni_bt_service_deinit();
}
