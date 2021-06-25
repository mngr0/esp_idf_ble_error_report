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

const uint16_t uuid_GATT_M_STATE = 0xFFFA;

/* The max length of characteristic value. When the GATT client performs a write or prepare write operation,
*  the data length must be less than GATTS_DEMO_CHAR_VAL_LEN_MAX.
*/        // esp_err_t create_attr_ret = esp_ble_gatts_create_service(gatts_if, &heart_rate_profile_tab[PROFILE_APP_IDX].service_id, HRS_IDX_NB);
        // if (create_attr_ret)
        // {
        //     ESP_LOGE(GATTS_TABLE_TAG, "create attr table failed, error code = %x", create_attr_ret);
        // }
static uint8_t gatt_ar_state_adv_config_done = 0; //TODO consider if it mus be extern
extern uint8_t raw_scan_rsp_data[10];
extern uint8_t raw_adv_data[26];
extern struct gatts_profile_inst heart_rate_profile_tab[PROFILE_NUM];
static const uint16_t GATT_M_STATE_UUID_SERVICE = (0xFF00 | GATT_M_STATE_IDX_SERVICE) & (INDEX_M_STATE <<8);

static const uint16_t primary_service_uuid = ESP_GATT_UUID_PRI_SERVICE;
//static const uint16_t character_declaration_uuid = ESP_GATT_UUID_CHAR_DECLARE;
//static const uint8_t char_prop_read_notify =  ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY;

//TODO check that I can pass char_prop_read_notify to multiple paramters
/* Full Database Description - Used to add attributes into the database */
const esp_gatts_attr_db_t gatt_m_state_db[GATT_M_STATE_NB] =
    {
        [GATT_M_STATE_IDX_SERVICE] =
            {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&primary_service_uuid, ESP_GATT_PERM_READ, sizeof(uint16_t), sizeof(GATT_M_STATE_UUID_SERVICE), (uint8_t *)&GATT_M_STATE_UUID_SERVICE}},

    [GATT_M_STATE_IDX_CHAR]=,
    [GATT_M_STATE_IDX_VALUE]=,

};

// void m_state_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
// {
//     switch (event)
//     {
//     case ESP_GATTS_REG_EVT:
//     {
//         esp_err_t set_dev_name_ret = esp_ble_gap_set_device_name(SAMPLE_DEVICE_NAME);
//         if (set_dev_name_ret)
//         {
//             ESP_LOGE(GATTS_M_STATE_TAG, "set device name failed, error code = %x", set_dev_name_ret);
//         }
//         esp_err_t raw_adv_ret = esp_ble_gap_config_adv_data_raw(raw_adv_data, sizeof(raw_adv_data));
//         if (raw_adv_ret)
//         {
//             ESP_LOGE(GATTS_M_STATE_TAG, "config raw adv data failed, error code = %x ", raw_adv_ret);
//         }
//         gatt_ar_state_adv_config_done |= ADV_CONFIG_FLAG;
//         esp_err_t raw_scan_ret = esp_ble_gap_config_scan_rsp_data_raw(raw_scan_rsp_data, sizeof(raw_scan_rsp_data));
//         if (raw_scan_ret)
//         {
//             ESP_LOGE(GATTS_M_STATE_TAG, "config raw scan rsp data failed, error code = %x", raw_scan_ret);
//         }
//         gatt_ar_state_adv_config_done |= SCAN_RSP_CONFIG_FLAG;

//         esp_err_t create_attr_ret = esp_ble_gatts_create_attr_tab(gatt_m_state_db, gatts_if, HRS_IDX_NB, SVC_INST_ID);
//         if (create_attr_ret)
//         {
//             ESP_LOGE(GATTS_M_STATE_TAG, "create attr table failed, error code = %x", create_attr_ret);
//         }
//     }
//     break;
//     case ESP_GATTS_READ_EVT:
//         ESP_LOGI(GATTS_M_STATE_TAG, "ESP_GATTS_READ_EVT");
//         break;
//     case ESP_GATTS_WRITE_EVT:
//     case ESP_GATTS_EXEC_WRITE_EVT:
//     case ESP_GATTS_MTU_EVT:
//     case ESP_GATTS_CONF_EVT:
//     case ESP_GATTS_START_EVT:
//     case ESP_GATTS_CONNECT_EVT:
//     case ESP_GATTS_DISCONNECT_EVT:
//     case ESP_GATTS_CREAT_ATTR_TAB_EVT:
//     case ESP_GATTS_STOP_EVT:
//     case ESP_GATTS_OPEN_EVT:
//     case ESP_GATTS_CANCEL_OPEN_EVT:
//     case ESP_GATTS_CLOSE_EVT:
//     case ESP_GATTS_LISTEN_EVT:
//     case ESP_GATTS_CONGEST_EVT:
//     case ESP_GATTS_UNREG_EVT:
//     case ESP_GATTS_DELETE_EVT:
//     default:
//         ESP_LOGI(GATTS_M_STATE_TAG, "SOME EVENT, MTU %d", event);
//         break;
//     }
// }