// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Ricardo Quesada
// http://retro.moe/unijoysticle2

#include "bt_service.h"

#include <btstack.h>

// General Discoverable = 0x02
// BR/EDR Not supported = 0x04
#define APP_AD_FLAGS 0x06
// Max number of clients that can connect to the service at the same time.
#define MAX_NR_CLIENT_CONNECTIONS 1

// client connection
typedef struct {
    bool notification_enabled;
    uint16_t value_handle;
    hci_con_handle_t connection_handle;
    bool is_webapp;
} client_connection_t;
static client_connection_t client_connections[MAX_NR_CLIENT_CONNECTIONS];

static bool service_enabled;
esp_ota_handle_t otaHandler = 0;

bool uploadingConfig = false;
uint16_t configReadOfset = 0; 
bool uploadingSettings = false;
uint16_t settingsReadOfset = 0; 

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

static client_connection_t* connection_for_conn_handle(hci_con_handle_t conn_handle) {
    int i;
    for (i = 0; i < MAX_NR_CLIENT_CONNECTIONS; i++) {
        if (client_connections[i].connection_handle == conn_handle)
            return &client_connections[i];
    }
    return NULL;
}

static int att_write_callback(hci_con_handle_t con_handle,
                              uint16_t att_handle,
                              uint16_t transaction_mode,
                              uint16_t offset,
                              uint8_t* buffer,
                              uint16_t buffer_size) {
    UNUSED(transaction_mode);

    Serial.println("att_write_callback");

    switch (att_handle) {
        // save controller mappings chunk
        case ATT_CHARACTERISTIC_4627C4A4_AC03_46B9_B688_AFC5C1BF7F63_01_VALUE_HANDLE: {
            FileUtility::appendFile(LittleFS, "/mappingHolding.json", (const char*)buffer);
            
            return ATT_ERROR_SUCCESS;
        }
        // save settings chunk
        case ATT_CHARACTERISTIC_4627C4A4_AC02_46B9_B688_AFC5C1BF7F63_01_VALUE_HANDLE: {
            FileUtility::appendFile(LittleFS, "/settingsHolding.json", (const char*)buffer);
            
            return ATT_ERROR_SUCCESS;
        }
        // handle OTA command
        case ATT_CHARACTERISTIC_4627C4A4_AC04_46B9_B688_AFC5C1BF7F63_01_VALUE_HANDLE: {
            switch((uint8_t) *buffer) {
                case 0: //upload begin
                    Serial.println("Beginning OTA update");
                    
                    if (ESP_OK != esp_ota_begin(esp_ota_get_next_update_partition(NULL), OTA_SIZE_UNKNOWN, &otaHandler)) {
                        Serial.println("Error starting OTA");
                        esp_ota_abort(otaHandler);

                        return ATT_ERROR_UNLIKELY_ERROR;
                    }
                break;
                case 1: //upload complete
                    Serial.println("Ending OTA Upload");
                    
                    if (ESP_OK != esp_ota_end(otaHandler)) {
                        Serial.println("Unable to end OTA");
                        esp_ota_abort(otaHandler);

                        return ATT_ERROR_UNLIKELY_ERROR;
                    }
                break;
                case 2: //apply OTA
                    Serial.println("Applying OTA update");

                    if (ESP_OK == esp_ota_set_boot_partition(esp_ota_get_next_update_partition(NULL)))
                    {
                        Serial.println("Done rebooting shortly...");
                        
                        INTEROP_prepareOTAApply();
                    }
                    else
                    {
                        Serial.println("Error applying OTA");
                        esp_ota_abort(otaHandler);

                        return ATT_ERROR_UNLIKELY_ERROR;
                    }
                break;
                case 3: //reset mappings to default            
                    FileUtility::deleteFile(LittleFS, "/mappingHolding.json");        
                    FileUtility::deleteFile(LittleFS, "/mapping.json");

                    INTEROP_reloadControllerMappings();
                    return ATT_ERROR_SUCCESS;
                break;
                case 4: //config upload start
                    FileUtility::deleteFile(LittleFS, "/mappingHolding.json");
                    uploadingConfig = true;

                    return ATT_ERROR_SUCCESS;
                case 5: //config upload complete
                    uploadingConfig = false;
                    return ATT_ERROR_SUCCESS;
                case 6: //config upload apply
                    uploadingConfig = false;
                    FileUtility::renameFile(LittleFS, "/mappingHolding.json", "/mapping.json");
                    FileUtility::deleteFile(LittleFS, "/mappingHolding.json");

                    INTEROP_reloadControllerMappings();

                    return ATT_ERROR_SUCCESS;
                case 7: //begin config read
                    configReadOfset = 0;

                    return ATT_ERROR_SUCCESS;
                case 8: // settngs upload start
                    FileUtility::deleteFile(LittleFS, "/settingsHolding.json");
                    uploadingSettings = true;

                    return ATT_ERROR_SUCCESS;
                break;
                case 9: //settings upload complete
                    uploadingSettings = false;
                break;
                case 10: // setting spply
                    uploadingSettings = false;
                    FileUtility::renameFile(LittleFS, "/settingsHolding.json", "/settings.json");
                    FileUtility::deleteFile(LittleFS, "/settingsHolding.json");

                    INTEROP_reloadSettings();

                    return ATT_ERROR_SUCCESS;
                break;
                case 11: //begin settings read
                    settingsReadOfset = 0;

                    return ATT_ERROR_SUCCESS;
                break;
                default: 
                    Serial.println("Unknown OTA Command");

                    return ATT_ERROR_UNLIKELY_ERROR;
                break;
            }

            return ATT_ERROR_SUCCESS;
        }
        // Recieve OTA Data
        case ATT_CHARACTERISTIC_4627C4A4_AC05_46B9_B688_AFC5C1BF7F63_01_VALUE_HANDLE: {

            if (buffer_size  > 0) {
                Serial.println("Writing OTA data");
                if (ESP_OK != esp_ota_write(otaHandler, buffer, buffer_size)) {
                    Serial.println("Error Writing OTA data");
                    esp_ota_abort(otaHandler);
                    return ATT_ERROR_UNLIKELY_ERROR;
                }
            }
            
            return ATT_ERROR_SUCCESS;
        }
        default:
            Serial.println("BLE Service: Unsupported write to");
            break;
    }
    return ATT_ERROR_SUCCESS;
}

static uint16_t att_read_callback(hci_con_handle_t con_handle,
                                  uint16_t att_handle,
                                  uint16_t offset,
                                  uint8_t* buffer,
                                  uint16_t buffer_size) {

    client_connection_t* ctx;

    switch (att_handle) {
        // Read Controller Mappings
        case ATT_CHARACTERISTIC_4627C4A4_AC03_46B9_B688_AFC5C1BF7F63_01_VALUE_HANDLE: {
            // TODO change me so its not a String class
            String mappingsAsString;

            if (uploadingConfig) {
                mappingsAsString = FileUtility::readFile(LittleFS, "/mappingHolding.json");
            } else {
                mappingsAsString = FileUtility::readFile(LittleFS, "/mapping.json");

                if (mappingsAsString.length() > 2 && mappingsAsString[0] == '[' && mappingsAsString[1] == ']'){
                    Serial.println("Loading default");
                    mappingsAsString = MAPPINGS_DEFAULT;
                }
            }

            // convert to char array
            auto len = mappingsAsString.length() + 1;
            char dataArr[len];
            mappingsAsString.toCharArray(dataArr, len);
        
            if (!buffer) {
                return len;
            }

            uint8_t resp = att_read_callback_handle_blob((const uint8_t *)dataArr, len, configReadOfset, buffer, 100);

            configReadOfset += resp;

            return resp;
        }
        // Read setting
        case ATT_CHARACTERISTIC_4627C4A4_AC02_46B9_B688_AFC5C1BF7F63_01_VALUE_HANDLE: {
            // TODO change me so its not a String class
            String mappingsAsString;

            if (uploadingSettings) {
                mappingsAsString = FileUtility::readFile(LittleFS, "/settingsHolding.json");
            } else {
                mappingsAsString = FileUtility::readFile(LittleFS, "/settings.json");
            }

            // convert to char array
            auto len = mappingsAsString.length() + 1;
            char dataArr[len];
            mappingsAsString.toCharArray(dataArr, len);
        
            if (!buffer) {
                return len;
            }

            uint8_t resp = att_read_callback_handle_blob((const uint8_t *)dataArr, len, settingsReadOfset, buffer, 100);

            settingsReadOfset += resp;

            return resp;
        }
        // Return Version
        case ATT_CHARACTERISTIC_4627C4A4_AC01_46B9_B688_AFC5C1BF7F63_01_VALUE_HANDLE: {
            ctx = connection_for_conn_handle(con_handle);

            if (!ctx)
                return ATT_ERROR_REQUEST_NOT_SUPPORTED;

            ctx->is_webapp = true;
            INTEROP_disableBLEInactivityWatcher();

            return att_read_callback_handle_blob((const uint8_t *)PSP_BLUETOOTH_VERSION, strlen(PSP_BLUETOOTH_VERSION), offset, buffer, buffer_size);
        }
        default:
            break;
    }
    return 0;
}

static void att_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t* packet, uint16_t size) {
    UNUSED(channel);
    UNUSED(size);

    client_connection_t* ctx;

    if (packet_type != HCI_EVENT_PACKET)
        return;

    switch (hci_event_packet_get_type(packet)) {
        case ATT_EVENT_CONNECTED:
            ctx = connection_for_conn_handle(HCI_CON_HANDLE_INVALID);

            if (!ctx)
                break;

            ctx->connection_handle = att_event_connected_get_handle(packet);
            break;
        case ATT_EVENT_DISCONNECTED:
            ctx = connection_for_conn_handle(att_event_disconnected_get_handle(packet));

            if (!ctx)
                break;

            Serial.print("BLE Service: client disconnected, handle = ");
            Serial.println(ctx->connection_handle);

            if (ctx->is_webapp)
                INTEROP_enableBLEInactivityWatcher();

            memset(ctx, 0, sizeof(*ctx));
            
            ctx->connection_handle = HCI_CON_HANDLE_INVALID;
            
            //this runs when controller disconnects too, maybe should remove it?
            esp_ota_abort(otaHandler);
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
