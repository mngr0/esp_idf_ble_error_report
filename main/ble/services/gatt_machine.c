
#include <stdint.h>
#include <string.h>

#include "esp_bt.h"
#include "esp_gatt_defs.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "nvs_flash.h"

#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"

#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_gatt_common_api.h"

#include "gatt_handle.h"

#include "ble/ble.h"
#include "stdbool.h"

#include "air_ref/logger.h"
#include "cJSON.h"

#define GATTS_MACHINE_TAG "GATTS_MACHINE"

static const uint16_t GATT_HANDLE_UUID_SERVICE =
    (GATT_HANDLE_IDX_SERVICE) | (PROFILE_MACHINE_IDX << 4);
static const uint16_t GATT_HANDLE_STATUS_VALUE =
    (GATT_HANDLE_IDX_STATUS_VALUE) | (PROFILE_MACHINE_IDX << 4);
static const uint16_t GATT_HANDLE_CONFIG_VALUE =
    (GATT_HANDLE_IDX_CONFIG_VALUE) | (PROFILE_MACHINE_IDX << 4);
static const uint16_t GATT_HANDLE_COMMAND_VALUE =
    (GATT_HANDLE_IDX_COMMAND_VALUE) | (PROFILE_MACHINE_IDX << 4);
static const uint16_t GATT_HANDLE_HANDLE_STATUS_VALUE =
    (GATT_HANDLE_IDX_HANDLE_STATUS_VALUE) | (PROFILE_MACHINE_IDX << 4);

static const uint16_t primary_service_uuid = ESP_GATT_UUID_PRI_SERVICE;

static const uint16_t character_declaration_uuid = ESP_GATT_UUID_CHAR_DECLARE;
static const uint8_t char_prop_notify = ESP_GATT_CHAR_PROP_BIT_NOTIFY | ESP_GATT_CHAR_PROP_BIT_INDICATE;
static const uint8_t char_prop_write = ESP_GATT_CHAR_PROP_BIT_WRITE;
static const uint8_t char_prop_notify_write = ESP_GATT_CHAR_PROP_BIT_NOTIFY |
                                              ESP_GATT_CHAR_PROP_BIT_INDICATE |
                                              ESP_GATT_CHAR_PROP_BIT_WRITE;

static handle_descriptor_t machine_descriptor;

const esp_gatts_attr_db_t gatt_machine_db[GATT_HANDLE_NB] = {
    [GATT_HANDLE_IDX_SERVICE] =
        {{ESP_GATT_AUTO_RSP},
         {ESP_UUID_LEN_16, (uint8_t *)&primary_service_uuid, ESP_GATT_PERM_READ,
          sizeof(GATT_HANDLE_UUID_SERVICE), sizeof(GATT_HANDLE_UUID_SERVICE),
          (uint8_t *)&GATT_HANDLE_UUID_SERVICE}},

    [GATT_HANDLE_IDX_STATUS_CHAR] = // notify only
    {{ESP_GATT_AUTO_RSP},
     {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid,
      ESP_GATT_PERM_READ |
          ESP_GATT_PERM_WRITE, // write here should enable/disable notification
      CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE,
      (uint8_t *)&char_prop_notify}},

    [GATT_HANDLE_IDX_STATUS_VALUE] = {{ESP_GATT_RSP_BY_APP},
                                      {ESP_UUID_LEN_16,
                                       (uint8_t *)&GATT_HANDLE_STATUS_VALUE,
                                       ESP_GATT_PERM_READ,
                                       GATTS_DEMO_CHAR_VAL_LEN_MAX, 1024,
                                       (uint8_t *)machine_descriptor.status}},

    [GATT_HANDLE_IDX_CONFIG_CHAR] = // notify write
    {{ESP_GATT_AUTO_RSP},
     {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid,
      ESP_GATT_PERM_READ, CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE,
      (uint8_t *)&char_prop_notify_write}},

    [GATT_HANDLE_IDX_CONFIG_VALUE] = {{ESP_GATT_AUTO_RSP},
                                      {ESP_UUID_LEN_16,
                                       (uint8_t *)&GATT_HANDLE_CONFIG_VALUE,
                                       ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                                       GATTS_DEMO_CHAR_VAL_LEN_MAX, 1024,
                                       (uint8_t *)machine_descriptor.config}},

    [GATT_HANDLE_IDX_COMMAND_CHAR] = // write only
    {{ESP_GATT_AUTO_RSP},
     {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid,
      ESP_GATT_PERM_READ, CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE,
      (uint8_t *)&char_prop_write}},

    [GATT_HANDLE_IDX_COMMAND_VALUE] = {{ESP_GATT_RSP_BY_APP},
                                       {ESP_UUID_LEN_16,
                                        (uint8_t *)&GATT_HANDLE_COMMAND_VALUE,
                                        ESP_GATT_PERM_WRITE,
                                        GATTS_DEMO_CHAR_VAL_LEN_MAX, 64,
                                        (uint8_t *)machine_descriptor.command}},

    [GATT_HANDLE_IDX_HANDLE_STATUS_CHAR] = // notify ()
    {{ESP_GATT_AUTO_RSP},
     {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid,
      ESP_GATT_PERM_READ, CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE,
      (uint8_t *)&char_prop_notify}},

    [GATT_HANDLE_IDX_HANDLE_STATUS_VALUE] =
        {{ESP_GATT_RSP_BY_APP},
         {ESP_UUID_LEN_16, (uint8_t *)&GATT_HANDLE_HANDLE_STATUS_VALUE,
          ESP_GATT_PERM_READ, GATTS_DEMO_CHAR_VAL_LEN_MAX, 64,
          (uint8_t *)machine_descriptor.handle_status}},
};

bool gatt_machine_send_status_update_to_client(char *json_status) {
  return gatt_handle_send_status_update_to_client(&machine_descriptor,
                                                  json_status);
}

bool gatt_machine_send_logger_update_to_client(char *json_status) {
  return gatt_handle_send_logger_update_to_client(&machine_descriptor,
                                                  json_status);
}

bool gatt_machine_send_conf_update_to_client(char *json_status) {
  return gatt_handle_send_conf_update_to_client(&machine_descriptor,
                                                json_status);
}

handle_descriptor_t *get_machine_handle_ptr() { return &machine_descriptor; }

void machine_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if,
                           esp_ble_gatts_cb_param_t *param) {
  handle_event_handler(GATTS_MACHINE_TAG, &machine_descriptor, event, gatts_if,
                       param, PROFILE_MACHINE_IDX, gatt_machine_db);
}
