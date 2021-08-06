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

#include "gatt_ar_conf.h"
#include "ble/ble.h"
#include "air_ref/air_ref.h"

#define GATTS_AR_CONF_TAG "GATTS_AR_CONF"

extern machine_state_t m_state;
extern air_ref_conf_t ar_conf;
extern air_ref_state_t ar_state;

//static uint8_t gatt_ar_state_adv_config_done = 0; //TODO consider if it mus be extern
extern uint8_t raw_scan_rsp_data[10];
extern uint8_t raw_adv_data[26];
extern struct gatts_profile_inst heart_rate_profile_tab[PROFILE_NUM];

const uint16_t GATT_AR_CONF_UUID_SERVICE = (0xFF00 | GATT_AR_CONF_IDX_SERVICE) & (PROFILE_AR_CONF_IDX << 8);
const uint16_t GATT_AR_CONF_UUID_VALUE = (0xFF00 | GATT_AR_CONF_IDX_VALUE) & (PROFILE_AR_CONF_IDX << 8);

static const uint16_t primary_service_uuid = ESP_GATT_UUID_PRI_SERVICE;
static const uint16_t character_declaration_uuid = ESP_GATT_UUID_CHAR_DECLARE;
static const uint8_t char_prop_read_notify = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY;


static prepare_type_env_t b_prepare_write_env;
static esp_gatt_char_prop_t b_property = 0;

//TODO check that I can pass char_prop_read_notify to multiple paramters
/* Full Database Description - Used to add attributes into the database */
const esp_gatts_attr_db_t gatt_ar_conf_db[GATT_AR_CONF_NB] =
    {
        [GATT_AR_CONF_IDX_SERVICE] =
            {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&primary_service_uuid, ESP_GATT_PERM_READ, sizeof(uint16_t), sizeof(GATT_AR_CONF_UUID_SERVICE), (uint8_t *)&GATT_AR_CONF_UUID_SERVICE}},

        [GATT_AR_CONF_IDX_CHAR] =
            {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ, CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read_notify}},

        [GATT_AR_CONF_IDX_VALUE] =
            {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&GATT_AR_CONF_UUID_VALUE, ESP_GATT_PERM_READ, GATTS_DEMO_CHAR_VAL_LEN_MAX, sizeof(air_ref_conf_t), (uint8_t *)&ar_conf}},

};

void ar_conf_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param) {
    switch (event) {
    case ESP_GATTS_REG_EVT:
        ESP_LOGI(GATTS_AR_CONF_TAG, "REGISTER_APP_EVT, status %d, app_id %d\n", param->reg.status, param->reg.app_id);
        heart_rate_profile_tab[PROFILE_AR_CONF_IDX].service_id.is_primary = true;
        heart_rate_profile_tab[PROFILE_AR_CONF_IDX].service_id.id.inst_id = 0x00;
        heart_rate_profile_tab[PROFILE_AR_CONF_IDX].service_id.id.uuid.len = ESP_UUID_LEN_16;
        heart_rate_profile_tab[PROFILE_AR_CONF_IDX].service_id.id.uuid.uuid.uuid16 = GATT_AR_CONF_UUID_SERVICE;

        esp_ble_gatts_create_service(gatts_if, &heart_rate_profile_tab[PROFILE_AR_CONF_IDX].service_id, GATT_AR_CONF_NB);
        break;
    case ESP_GATTS_READ_EVT: { // READ MACHINE STATE
        ESP_LOGI(GATTS_AR_CONF_TAG, "GATT_READ_EVT, conn_id %d, trans_id %d, handle %d\n", param->read.conn_id, param->read.trans_id, param->read.handle);
        esp_gatt_rsp_t rsp;
        memset(&rsp, 0, sizeof(esp_gatt_rsp_t));
        rsp.attr_value.handle = param->read.handle;
        rsp.attr_value.len = 4;
        rsp.attr_value.value[0] = 0xde;
        rsp.attr_value.value[1] = 0xad;
        rsp.attr_value.value[2] = 0xbe;
        rsp.attr_value.value[3] = 0xef;
        esp_ble_gatts_send_response(gatts_if, param->read.conn_id, param->read.trans_id,
                                    ESP_GATT_OK, &rsp);
        break;
    }
    case ESP_GATTS_WRITE_EVT: { //EDIT CONF
        ESP_LOGI(GATTS_AR_CONF_TAG, "GATT_WRITE_EVT, conn_id %d, trans_id %d, handle %d\n", param->write.conn_id, param->write.trans_id, param->write.handle);
        if (!param->write.is_prep){
            ESP_LOGI(GATTS_AR_CONF_TAG, "GATT_WRITE_EVT, value len %d, value :", param->write.len);
            esp_log_buffer_hex(GATTS_AR_CONF_TAG, param->write.value, param->write.len);

            //TODO call function to send new conf

            // if (heart_rate_profile_tab[PROFILE_AR_CONF_IDX].descr_handle == param->write.handle && param->write.len == 2){
            //     uint16_t descr_value= param->write.value[1]<<8 | param->write.value[0];
            //     if (descr_value == 0x0001){
            //         if (b_property & ESP_GATT_CHAR_PROP_BIT_NOTIFY){
            //             ESP_LOGI(GATTS_AR_CONF_TAG, "notify enable");
            //             uint8_t notify_data[15];
            //             for (int i = 0; i < sizeof(notify_data); ++i)
            //             {
            //                 notify_data[i] = i%0xff;
            //             }
            //             //the size of notify_data[] need less than MTU size
            //             esp_ble_gatts_send_indicate(gatts_if, param->write.conn_id, heart_rate_profile_tab[PROFILE_AR_CONF_IDX].char_handle,
            //                                     sizeof(notify_data), notify_data, false);
            //         }
            //     }else if (descr_value == 0x0002){
            //         if (b_property & ESP_GATT_CHAR_PROP_BIT_INDICATE){
            //             ESP_LOGI(GATTS_AR_CONF_TAG, "indicate enable");
            //             uint8_t indicate_data[15];
            //             for (int i = 0; i < sizeof(indicate_data); ++i)
            //             {
            //                 indicate_data[i] = i%0xff;
            //             }
            //             //the size of indicate_data[] need less than MTU size
            //             esp_ble_gatts_send_indicate(gatts_if, param->write.conn_id, heart_rate_profile_tab[PROFILE_AR_CONF_IDX].char_handle,
            //                                     sizeof(indicate_data), indicate_data, true);
            //         }
            //     }
            //     else if (descr_value == 0x0000){
            //         ESP_LOGI(GATTS_AR_CONF_TAG, "notify/indicate disable ");
            //     }else{
            //         ESP_LOGE(GATTS_AR_CONF_TAG, "unknown value");
            //     }

            // }
        }
        example_write_event_env(gatts_if, &b_prepare_write_env, param);
        break;
    }
    case ESP_GATTS_EXEC_WRITE_EVT:
        ESP_LOGI(GATTS_AR_CONF_TAG,"ESP_GATTS_EXEC_WRITE_EVT");
        esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, ESP_GATT_OK, NULL);
        example_exec_write_event_env(&b_prepare_write_env, param);
        break;
    case ESP_GATTS_MTU_EVT:
        ESP_LOGI(GATTS_AR_CONF_TAG, "ESP_GATTS_MTU_EVT, MTU %d", param->mtu.mtu);
        break;
    case ESP_GATTS_UNREG_EVT:
        break;
    case ESP_GATTS_CREATE_EVT:
        ESP_LOGI(GATTS_AR_CONF_TAG, "CREATE_SERVICE_EVT, status %d,  service_handle %d\n", param->create.status, param->create.service_handle);
        heart_rate_profile_tab[PROFILE_AR_CONF_IDX].service_handle = param->create.service_handle;
        heart_rate_profile_tab[PROFILE_AR_CONF_IDX].char_uuid.len = ESP_UUID_LEN_16;
        heart_rate_profile_tab[PROFILE_AR_CONF_IDX].char_uuid.uuid.uuid16 = GATT_AR_CONF_UUID_VALUE;

        esp_ble_gatts_start_service(heart_rate_profile_tab[PROFILE_AR_CONF_IDX].service_handle);
        b_property = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_NOTIFY;
        esp_err_t add_char_ret =esp_ble_gatts_add_char( heart_rate_profile_tab[PROFILE_AR_CONF_IDX].service_handle, &heart_rate_profile_tab[PROFILE_AR_CONF_IDX].char_uuid,
                                                        ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                                                        b_property,
                                                        NULL, NULL);
        if (add_char_ret){
            ESP_LOGE(GATTS_AR_CONF_TAG, "add char failed, error code =%x",add_char_ret);
        }
        break;
    case ESP_GATTS_ADD_INCL_SRVC_EVT:
        break;
    case ESP_GATTS_ADD_CHAR_EVT:
        ESP_LOGI(GATTS_AR_CONF_TAG, "ADD_CHAR_EVT, status %d,  attr_handle %d, service_handle %d\n",
                 param->add_char.status, param->add_char.attr_handle, param->add_char.service_handle);

        heart_rate_profile_tab[PROFILE_AR_CONF_IDX].char_handle = param->add_char.attr_handle;
        heart_rate_profile_tab[PROFILE_AR_CONF_IDX].descr_uuid.len = ESP_UUID_LEN_16;
        heart_rate_profile_tab[PROFILE_AR_CONF_IDX].descr_uuid.uuid.uuid16 = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;
        esp_ble_gatts_add_char_descr(heart_rate_profile_tab[PROFILE_AR_CONF_IDX].service_handle, &heart_rate_profile_tab[PROFILE_AR_CONF_IDX].descr_uuid,
                                     ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                                     NULL, NULL);
        break;
    case ESP_GATTS_ADD_CHAR_DESCR_EVT:
        heart_rate_profile_tab[PROFILE_AR_CONF_IDX].descr_handle = param->add_char_descr.attr_handle;
        ESP_LOGI(GATTS_AR_CONF_TAG, "ADD_DESCR_EVT, status %d, attr_handle %d, service_handle %d\n",
                 param->add_char_descr.status, param->add_char_descr.attr_handle, param->add_char_descr.service_handle);
        break;
    case ESP_GATTS_DELETE_EVT:
        break;
    case ESP_GATTS_START_EVT:
        ESP_LOGI(GATTS_AR_CONF_TAG, "SERVICE_START_EVT, status %d, service_handle %d\n",
                 param->start.status, param->start.service_handle);
        break;
    case ESP_GATTS_STOP_EVT:
        break;
    case ESP_GATTS_CONNECT_EVT:
        ESP_LOGI(GATTS_AR_CONF_TAG, "CONNECT_EVT, conn_id %d, remote %02x:%02x:%02x:%02x:%02x:%02x:",
                 param->connect.conn_id,
                 param->connect.remote_bda[0], param->connect.remote_bda[1], param->connect.remote_bda[2],
                 param->connect.remote_bda[3], param->connect.remote_bda[4], param->connect.remote_bda[5]);
        heart_rate_profile_tab[PROFILE_AR_CONF_IDX].conn_id = param->connect.conn_id;
        break;
    case ESP_GATTS_CONF_EVT:
        ESP_LOGI(GATTS_AR_CONF_TAG, "ESP_GATTS_CONF_EVT status %d attr_handle %d", param->conf.status, param->conf.handle);
        if (param->conf.status != ESP_GATT_OK){
            esp_log_buffer_hex(GATTS_AR_CONF_TAG, param->conf.value, param->conf.len);
        }
    break;
    case ESP_GATTS_DISCONNECT_EVT:
    case ESP_GATTS_OPEN_EVT:
    case ESP_GATTS_CANCEL_OPEN_EVT:
    case ESP_GATTS_CLOSE_EVT:
    case ESP_GATTS_LISTEN_EVT:
    case ESP_GATTS_CONGEST_EVT:
    default:
        break;
    }
}