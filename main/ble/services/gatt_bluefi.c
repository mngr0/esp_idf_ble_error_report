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

#include "gatt_bluefi.h"

#include "ble/ble.h"
#include "air_ref/air_ref.h"
#include "air_ref/serial_logger.h"

#define GATTS_BLUEFI_TAG "GATTS_BLUEFI"

#define INDEX_BLUEFI 4

extern machine_state_t m_state;
extern air_ref_conf_t ar_conf;
extern air_ref_state_t ar_state;

//static uint8_t gatt_ar_state_adv_config_done = 0; //TODO consider if it mus be extern
extern uint8_t raw_scan_rsp_data[10];
extern uint8_t raw_adv_data[26];
extern struct gatts_profile_inst heart_rate_profile_tab[PROFILE_NUM];
const uint16_t GATT_BLUEFI_UUID_SERVICE = (0xFF00 | GATT_BLUEFI_IDX_SERVICE) & (INDEX_BLUEFI << 8);

const uint16_t primary_service_uuid = ESP_GATT_UUID_PRI_SERVICE;
//static const uint16_t character_declaration_uuid = ESP_GATT_UUID_CHAR_DECLARE;
//static const uint8_t char_prop_read_notify =  ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY;

//TODO check that I can pass char_prop_read_notify to multiple paramters
/* Full Database Description - Used to add attributes into the database */
const esp_gatts_attr_db_t gatt_bluefi_db[GATT_BLUEFI_NB] =
    {
        [GATT_BLUEFI_IDX_SERVICE] =
            {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&primary_service_uuid, ESP_GATT_PERM_READ, sizeof(uint16_t), sizeof(GATT_BLUEFI_UUID_SERVICE), (uint8_t *)&GATT_BLUEFI_UUID_SERVICE}},

};
