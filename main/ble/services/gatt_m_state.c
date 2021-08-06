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

#include "esp_gatt_common_api.h"
#include "driver/gpio.h"
#include "driver/uart.h"

#include "gatt_m_state.h"

#include "ble/ble.h"
#include "air_ref/air_ref.h"
#include "stdbool.h"

#include "air_ref/serial_logger/logger_frame.h"
#include "air_ref/serial_logger/logger_air_ref.h"
#include "air_ref/air_ref.h"

#define GATTS_M_STATE_TAG "GATTS_M_STATE"

extern machine_state_t m_state;
extern air_ref_conf_t ar_conf;
extern air_ref_state_t ar_state;

extern uint8_t raw_scan_rsp_data[10];
extern uint8_t raw_adv_data[26];
extern struct gatts_profile_inst machine_state_profile_tab[PROFILE_NUM];
const uint16_t GATT_M_STATE_UUID_SERVICE = (0xFF00 | GATT_M_STATE_IDX_SERVICE) & (PROFILE_M_STATE_IDX << 8);
const uint16_t GATT_M_STATE_UUID_VALUE = (0xFF00 | GATT_M_STATE_IDX_VALUE) & (PROFILE_M_STATE_IDX << 8);

static const uint16_t primary_service_uuid = ESP_GATT_UUID_PRI_SERVICE;
static const uint16_t character_declaration_uuid = ESP_GATT_UUID_CHAR_DECLARE;
static const uint8_t char_prop_read_notify = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY;

static prepare_type_env_t b_prepare_write_env;
static esp_gatt_char_prop_t b_property = 0;

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

void m_state_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    switch (event)
    {
    case ESP_GATTS_REG_EVT:
        ESP_LOGI(GATTS_M_STATE_TAG, "REGISTER_APP_EVT, status %d, app_id %d\n", param->reg.status, param->reg.app_id);
        heart_rate_profile_tab[PROFILE_M_STATE_IDX].service_id.is_primary = true;
        heart_rate_profile_tab[PROFILE_M_STATE_IDX].service_id.id.inst_id = 0x00;
        heart_rate_profile_tab[PROFILE_M_STATE_IDX].service_id.id.uuid.len = ESP_UUID_LEN_16;
        heart_rate_profile_tab[PROFILE_M_STATE_IDX].service_id.id.uuid.uuid.uuid16 = GATT_M_STATE_UUID_SERVICE;

        esp_ble_gatts_create_service(gatts_if, &heart_rate_profile_tab[PROFILE_M_STATE_IDX].service_id, GATT_M_STATE_NB);
        break;
    case ESP_GATTS_READ_EVT:
    { // READ MACHINE STATE
        ESP_LOGI(GATTS_M_STATE_TAG, "GATT_READ_EVT, conn_id %d, trans_id %d, handle %d\n", param->read.conn_id, param->read.trans_id, param->read.handle);
        esp_gatt_rsp_t rsp;

        flatcc_builder_t builder;
        size_t size;
        void *buf;

        memset(&rsp, 0, sizeof(esp_gatt_rsp_t));
        rsp.attr_value.handle = param->read.handle;

        flatcc_builder_init(&builder);
        load_m_state(&builder, &m_state);
        buf = flatcc_builder_finalize_buffer(&builder, &size);
        rsp.attr_value.len = size;
        memcpy(rsp.attr_value.value, buf, size);
        ESP_LOGI(GATTS_M_STATE_TAG, "required size: %d\n", size);

        flatcc_builder_aligned_free(buf);
        flatcc_builder_clear(&builder);

        esp_ble_gatts_send_response(gatts_if, param->read.conn_id, param->read.trans_id,
                                    ESP_GATT_OK, &rsp);
        break;
    }
    case ESP_GATTS_WRITE_EVT:
    { //EDIT CONF
        ESP_LOGI(GATTS_M_STATE_TAG, "GATT_WRITE_EVT, conn_id %d, trans_id %d, handle %d\n", param->write.conn_id, param->write.trans_id, param->write.handle);
        if (!param->write.is_prep)
        {
            ESP_LOGI(GATTS_M_STATE_TAG, "GATT_WRITE_EVT, value len %d, value :", param->write.len);
            esp_log_buffer_hex(GATTS_M_STATE_TAG, param->write.value, param->write.len);
        }
        example_write_event_env(gatts_if, &b_prepare_write_env, param);
        break;
    }
    case ESP_GATTS_EXEC_WRITE_EVT:
        ESP_LOGI(GATTS_M_STATE_TAG, "ESP_GATTS_EXEC_WRITE_EVT");
        esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, ESP_GATT_OK, NULL);
        example_exec_write_event_env(&b_prepare_write_env, param);
        break;
    case ESP_GATTS_MTU_EVT:
        ESP_LOGI(GATTS_M_STATE_TAG, "ESP_GATTS_MTU_EVT, MTU %d", param->mtu.mtu);
        break;
    case ESP_GATTS_UNREG_EVT:
        break;
    case ESP_GATTS_CREATE_EVT:
        ESP_LOGI(GATTS_M_STATE_TAG, "CREATE_SERVICE_EVT, status %d,  service_handle %d\n", param->create.status, param->create.service_handle);
        heart_rate_profile_tab[PROFILE_M_STATE_IDX].service_handle = param->create.service_handle;
        heart_rate_profile_tab[PROFILE_M_STATE_IDX].char_uuid.len = ESP_UUID_LEN_16;
        heart_rate_profile_tab[PROFILE_M_STATE_IDX].char_uuid.uuid.uuid16 = GATT_M_STATE_UUID_VALUE;

        esp_ble_gatts_start_service(heart_rate_profile_tab[PROFILE_M_STATE_IDX].service_handle);
        b_property = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_NOTIFY;
        esp_err_t add_char_ret = esp_ble_gatts_add_char(heart_rate_profile_tab[PROFILE_M_STATE_IDX].service_handle, &heart_rate_profile_tab[PROFILE_M_STATE_IDX].char_uuid,
                                                        ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                                                        b_property,
                                                        NULL, NULL);
        if (add_char_ret)
        {
            ESP_LOGE(GATTS_M_STATE_TAG, "add char failed, error code =%x", add_char_ret);
        }
        break;
    case ESP_GATTS_ADD_INCL_SRVC_EVT:
        break;
    case ESP_GATTS_ADD_CHAR_EVT:
        ESP_LOGI(GATTS_M_STATE_TAG, "ADD_CHAR_EVT, status %d,  attr_handle %d, service_handle %d\n",
                 param->add_char.status, param->add_char.attr_handle, param->add_char.service_handle);

        heart_rate_profile_tab[PROFILE_M_STATE_IDX].char_handle = param->add_char.attr_handle;
        heart_rate_profile_tab[PROFILE_M_STATE_IDX].descr_uuid.len = ESP_UUID_LEN_16;
        heart_rate_profile_tab[PROFILE_M_STATE_IDX].descr_uuid.uuid.uuid16 = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;
        esp_ble_gatts_add_char_descr(heart_rate_profile_tab[PROFILE_M_STATE_IDX].service_handle, &heart_rate_profile_tab[PROFILE_M_STATE_IDX].descr_uuid,
                                     ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                                     NULL, NULL);
        break;
    case ESP_GATTS_ADD_CHAR_DESCR_EVT:
        heart_rate_profile_tab[PROFILE_M_STATE_IDX].descr_handle = param->add_char_descr.attr_handle;
        ESP_LOGI(GATTS_M_STATE_TAG, "ADD_DESCR_EVT, status %d, attr_handle %d, service_handle %d\n",
                 param->add_char_descr.status, param->add_char_descr.attr_handle, param->add_char_descr.service_handle);
        break;
    case ESP_GATTS_DELETE_EVT:
        break;
    case ESP_GATTS_START_EVT:
        ESP_LOGI(GATTS_M_STATE_TAG, "SERVICE_START_EVT, status %d, service_handle %d\n",
                 param->start.status, param->start.service_handle);
        break;
    case ESP_GATTS_STOP_EVT:
        break;
    case ESP_GATTS_CONNECT_EVT:
        ESP_LOGI(GATTS_M_STATE_TAG, "CONNECT_EVT, conn_id %d, remote %02x:%02x:%02x:%02x:%02x:%02x:",
                 param->connect.conn_id,
                 param->connect.remote_bda[0], param->connect.remote_bda[1], param->connect.remote_bda[2],
                 param->connect.remote_bda[3], param->connect.remote_bda[4], param->connect.remote_bda[5]);
        heart_rate_profile_tab[PROFILE_M_STATE_IDX].conn_id = param->connect.conn_id;
        break;
    case ESP_GATTS_CONF_EVT:
        ESP_LOGI(GATTS_M_STATE_TAG, "ESP_GATTS_CONF_EVT status %d attr_handle %d", param->conf.status, param->conf.handle);
        if (param->conf.status != ESP_GATT_OK)
        {
            esp_log_buffer_hex(GATTS_M_STATE_TAG, param->conf.value, param->conf.len);
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