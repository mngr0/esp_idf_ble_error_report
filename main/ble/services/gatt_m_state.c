/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

/****************************************************************************
*
* This demo showcases creating a GATT database using a predefined attribute table.
* It acts as a GATT server and can send adv data, be connected by client.
* Run the gatt_client demo, the client demo will automatically connect to the gatt_server_service_table demo.
* Client demo will enable GATT server's notify after connection. The two devices will then exchange
* data.
*
****************************************************************************/

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"

#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_main.h"
//#include "gatts_table_creat_demo.h"
#include "esp_gatt_common_api.h"
#include "driver/gpio.h"
#include "driver/uart.h"

#include "gatt_m_state.h"

#include "ble/ble.h"
#include "air_ref/air_ref.h"
#include "air_ref/serial_logger.h"

#define INDEX_M_STATE 3
#define GATTS_M_STATE_TAG "GATTS_M_STATE"

extern machine_state_t m_state;
extern air_ref_conf_t ar_conf;
extern air_ref_state_t ar_state;

extern uint8_t raw_scan_rsp_data[10];
extern uint8_t raw_adv_data[26];
extern struct gatts_profile_inst heart_rate_profile_tab[PROFILE_NUM];
const uint16_t GATT_M_STATE_UUID_SERVICE = (0xFF00 | GATT_M_STATE_IDX_SERVICE) & (INDEX_M_STATE << 8);
const uint16_t GATT_M_STATE_UUID_VALUE = (0xFF00 | GATT_M_STATE_IDX_VALUE) & (INDEX_M_STATE << 8);

static const uint16_t primary_service_uuid = ESP_GATT_UUID_PRI_SERVICE;
static const uint16_t character_declaration_uuid = ESP_GATT_UUID_CHAR_DECLARE;
static const uint8_t char_prop_read_notify = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY;

/* Full Database Description - Used to add attributes into the database */
const esp_gatts_attr_db_t gatt_m_state_db[GATT_M_STATE_NB] =
    {
        [GATT_M_STATE_IDX_SERVICE] =
            {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&primary_service_uuid, ESP_GATT_PERM_READ, sizeof(uint16_t), sizeof(GATT_M_STATE_UUID_SERVICE), (uint8_t *)&GATT_M_STATE_UUID_SERVICE}},

        [GATT_M_STATE_IDX_CHAR] =
            {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ, CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read_notify}},

        [GATT_M_STATE_IDX_VALUE] =
            {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&GATT_M_STATE_UUID_VALUE, ESP_GATT_PERM_READ, GATTS_DEMO_CHAR_VAL_LEN_MAX, sizeof(machine_state_t), (uint8_t *)&m_state}},

};
