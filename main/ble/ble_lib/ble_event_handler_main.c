/*
 * SPDX-FileCopyrightText: 2021 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

/****************************************************************************
 *
 * This demo showcases creating a GATT database using a predefined attribute
 *table. It acts as a GATT server and can send adv data, be connected by client.
 * Run the gatt_client demo, the client demo will automatically connect to the
 *gatt_server_service_table demo. Client demo will enable GATT server's notify
 *after connection. The two devices will then exchange data.
 *
 ****************************************************************************/

#include <string.h>

#include "esp_bt.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "nvs_flash.h"

#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_gatt_common_api.h"
#include "esp_gatts_api.h"

#include "ble_utils.h"

#define GATTS_TABLE_TAG "BLE_MAIN"
#define TAG GATTS_TABLE_TAG

#define SRV_IDX 0

static const uint16_t primary_service_uuid         = ESP_GATT_UUID_PRI_SERVICE;
static const uint16_t GATTS_SERVICE_UUID_MAIN        = 0x00FF;

static uint16_t *handle_table;
static esp_gatts_attr_db_t *gatt_db;
static uint16_t *UUIDs_main;
static uint8_t srv_inst_id;
static uint16_t MAIN_ENTRY_SIZE;


#define SAMPLE_DEVICE_NAME "ESP_GATTS_DEMO"

/* The max length of characteristic value. When the GATT client performs a write
 * or prepare write operation, the data length must be less than
 * GATTS_DEMO_CHAR_VAL_LEN_MAX.
 */
#define ADV_CONFIG_FLAG (1 << 0)
#define SCAN_RSP_CONFIG_FLAG (1 << 1)

static uint8_t adv_config_done = 0;

static prepare_type_env_t prepare_write_env;

static uint8_t service_uuid[16] = {
    /* LSB
       <-------------------------------------------------------------------------------->
       MSB */
    // first uuid, 16bit, [12],[13] is the value
    0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80,
    0x00, 0x10, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
};

/* The length of adv data must be less than 31 bytes */
static esp_ble_adv_data_t adv_data = {
    .set_scan_rsp = false,
    .include_name = true,
    .include_txpower = true,
    .min_interval = 0x0006, // slave connection min interval, Time =
                            // min_interval * 1.25 msec
    .max_interval = 0x0010, // slave connection max interval, Time =
                            // max_interval * 1.25 msec
    .appearance = 0x00,
    .manufacturer_len = 0,       // TEST_MANUFACTURER_DATA_LEN,
    .p_manufacturer_data = NULL, // test_manufacturer,
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = sizeof(service_uuid),
    .p_service_uuid = service_uuid,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

// scan response data
static esp_ble_adv_data_t scan_rsp_data = {
    .set_scan_rsp = true,
    .include_name = true,
    .include_txpower = true,
    .min_interval = 0x0006,
    .max_interval = 0x0010,
    .appearance = 0x00,
    .manufacturer_len = 0,       // TEST_MANUFACTURER_DATA_LEN,
    .p_manufacturer_data = NULL, //&test_manufacturer[0],
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = sizeof(service_uuid),
    .p_service_uuid = service_uuid,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

static esp_ble_adv_params_t adv_params = {
    .adv_int_min = 0x20,
    .adv_int_max = 0x40,
    .adv_type = ADV_TYPE_IND,
    .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
    .channel_map = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

/* Full Database Description - Used to add attributes into the database */
static esp_gatts_attr_db_t *gatt_db;


void gap_event_handler(esp_gap_ble_cb_event_t event,
                              esp_ble_gap_cb_param_t *param) {
  switch (event) {

  case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
    adv_config_done &= (~ADV_CONFIG_FLAG);
    if (adv_config_done == 0) {
      esp_ble_gap_start_advertising(&adv_params);
    }
    break;
  case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT:
    adv_config_done &= (~SCAN_RSP_CONFIG_FLAG);
    if (adv_config_done == 0) {
      esp_ble_gap_start_advertising(&adv_params);
    }
    break;
  case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
    /* advertising start complete event to indicate advertising start
     * successfully or failed */
    if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
      ESP_LOGE(GATTS_TABLE_TAG, "advertising start failed");
    } else {
      ESP_LOGI(GATTS_TABLE_TAG, "advertising start successfully");
    }
    break;
  case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
    if (param->adv_stop_cmpl.status != ESP_BT_STATUS_SUCCESS) {
      ESP_LOGE(GATTS_TABLE_TAG, "Advertising stop failed");
    } else {
      ESP_LOGI(GATTS_TABLE_TAG, "Stop adv successfully\n");
    }
    break;
  case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
    ESP_LOGI(
        GATTS_TABLE_TAG,
        "update connection params status = %d, min_int = %d, max_int = "
        "%d,conn_int = %d,latency = %d, timeout = %d",
        param->update_conn_params.status, param->update_conn_params.min_int,
        param->update_conn_params.max_int, param->update_conn_params.conn_int,
        param->update_conn_params.latency, param->update_conn_params.timeout);
    break;
  default:
    break;
  }
}


void gatts_profile_main_event_handler(esp_gatts_cb_event_t event,
                                        esp_gatt_if_t gatts_if,
                                        esp_ble_gatts_cb_param_t *param) {
  switch (event) {
  case ESP_GATTS_REG_EVT: {
    esp_err_t set_dev_name_ret =
        esp_ble_gap_set_device_name(SAMPLE_DEVICE_NAME);
    if (set_dev_name_ret) {
      ESP_LOGE(GATTS_TABLE_TAG, "set device name failed, error code = %x",
               set_dev_name_ret);
    }

    // config adv data
    esp_err_t ret = esp_ble_gap_config_adv_data(&adv_data);
    if (ret) {
      ESP_LOGE(GATTS_TABLE_TAG, "config adv data failed, error code = %x", ret);
    }
    adv_config_done |= ADV_CONFIG_FLAG;
    // config scan response data
    ret = esp_ble_gap_config_adv_data(&scan_rsp_data);
    if (ret) {
      ESP_LOGE(GATTS_TABLE_TAG,
               "config scan response data failed, error code = %x", ret);
    }
    adv_config_done |= SCAN_RSP_CONFIG_FLAG;

    esp_err_t create_attr_ret = esp_ble_gatts_create_attr_tab(
        gatt_db, gatts_if, MAIN_ENTRY_SIZE, srv_inst_id);
    if (create_attr_ret) {
      ESP_LOGE(GATTS_TABLE_TAG, "create attr table failed, error code = %x",
               create_attr_ret);
    }
  } break;
  case ESP_GATTS_READ_EVT:
    ESP_LOGI(GATTS_TABLE_TAG, "ESP_GATTS_READ_EVT");
    break;
  case ESP_GATTS_WRITE_EVT:
    if (!param->write.is_prep) {
      // the data length of gattc write  must be less than
      // GATTS_DEMO_CHAR_VAL_LEN_MAX.
      ESP_LOGI(GATTS_TABLE_TAG,
               "GATT_WRITE_EVT, handle = %d, value len = %d, value :",
               param->write.handle, param->write.len);
      esp_log_buffer_hex(GATTS_TABLE_TAG, param->write.value, param->write.len);

      /* send response when param->write.need_rsp is true*/
      if (param->write.need_rsp) {
        esp_ble_gatts_send_response(gatts_if, param->write.conn_id,
                                    param->write.trans_id, ESP_GATT_OK, NULL);
      }
    } else {
      /* handle prepare write */
      example_prepare_write_event_env(gatts_if, &prepare_write_env, param);
    }
    break;
  case ESP_GATTS_EXEC_WRITE_EVT:
    // the length of gattc prepare write data must be less than
    // GATTS_DEMO_CHAR_VAL_LEN_MAX.
    ESP_LOGI(GATTS_TABLE_TAG, "ESP_GATTS_EXEC_WRITE_EVT");
    example_exec_write_event_env(&prepare_write_env, param);
    break;
  case ESP_GATTS_MTU_EVT:
    ESP_LOGI(GATTS_TABLE_TAG, "ESP_GATTS_MTU_EVT, MTU %d", param->mtu.mtu);
    break;
  case ESP_GATTS_CONF_EVT:
    ESP_LOGI(GATTS_TABLE_TAG, "ESP_GATTS_CONF_EVT, status = %d, attr_handle %d",
             param->conf.status, param->conf.handle);
    break;
  case ESP_GATTS_START_EVT:
    ESP_LOGI(GATTS_TABLE_TAG, "SERVICE_START_EVT, status %d, service_handle %d",
             param->start.status, param->start.service_handle);
    break;
  case ESP_GATTS_CONNECT_EVT:
    ESP_LOGI(GATTS_TABLE_TAG, "ESP_GATTS_CONNECT_EVT, conn_id = %d",
             param->connect.conn_id);
    esp_log_buffer_hex(GATTS_TABLE_TAG, param->connect.remote_bda, 6);
    esp_ble_conn_update_params_t conn_params = {0};
    memcpy(conn_params.bda, param->connect.remote_bda, sizeof(esp_bd_addr_t));
    /* For the iOS system, please refer to Apple official documents about the
     * BLE connection parameters restrictions. */
    conn_params.latency = 0;
    conn_params.max_int = 0x20; // max_int = 0x20*1.25ms = 40ms
    conn_params.min_int = 0x10; // min_int = 0x10*1.25ms = 20ms
    conn_params.timeout = 400;  // timeout = 400*10ms = 4000ms
    // start sent the update connection parameters to the peer device.
    esp_ble_gap_update_conn_params(&conn_params);
    break;
  case ESP_GATTS_DISCONNECT_EVT:
    ESP_LOGI(GATTS_TABLE_TAG, "ESP_GATTS_DISCONNECT_EVT, reason = 0x%x",
             param->disconnect.reason);
    esp_ble_gap_start_advertising(&adv_params);
    break;
  case ESP_GATTS_CREAT_ATTR_TAB_EVT: {
    if (param->add_attr_tab.status != ESP_GATT_OK) {
      ESP_LOGE(GATTS_TABLE_TAG,
               "create attribute table failed, error code=0x%x",
               param->add_attr_tab.status);
    } else if (param->add_attr_tab.num_handle != MAIN_ENTRY_SIZE) {
      ESP_LOGE(GATTS_TABLE_TAG,
               "create attribute table abnormally, num_handle (%d) \
                        doesn't equal to DYNAMIC_SIZE(%d)",
               param->add_attr_tab.num_handle, MAIN_ENTRY_SIZE);
    } else {
      //(size1=%d, size2=%d)
      ESP_LOGI(GATTS_TABLE_TAG,
               "create attribute table successfully, the number handle = %d \n",
               param->add_attr_tab.num_handle);

      memcpy(handle_table, param->add_attr_tab.handles,
             MAIN_ENTRY_SIZE *
                 sizeof(uint16_t)); //(1+allocation_conf_size*DYNAMIC_SIZE)*sizeof(uint16_t));//todo
                                    //non sono affatto sicuro
                                    //sizeof(handle_table));
      ESP_LOGI(GATTS_TABLE_TAG, "memcpy done\n");

      esp_ble_gatts_start_service(handle_table[SRV_IDX]);
    }
    break;
  }
  case ESP_GATTS_STOP_EVT:
  case ESP_GATTS_OPEN_EVT:
  case ESP_GATTS_CANCEL_OPEN_EVT:
  case ESP_GATTS_CLOSE_EVT:
  case ESP_GATTS_LISTEN_EVT:
  case ESP_GATTS_CONGEST_EVT:
  case ESP_GATTS_UNREG_EVT:
  case ESP_GATTS_DELETE_EVT:
  default:
    break;
  }
}

static const uint16_t character_declaration_uuid = ESP_GATT_UUID_CHAR_DECLARE;
static const uint8_t char_prop_read_write_notify =
    ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_READ |
    ESP_GATT_CHAR_PROP_BIT_NOTIFY;
static const uint8_t char_prop_read_notify = ESP_GATT_CHAR_PROP_BIT_READ;
static const uint16_t descriptor_delaration = ESP_GATT_UUID_CHAR_DESCRIPTION;
static const uint16_t character_client_config_uuid =
    ESP_GATT_UUID_CHAR_CLIENT_CONFIG;

static const uint8_t char_value[4] = {0x11, 0x22, 0x33, 0x44};

void allocare_una_caratteristica_main(esp_gatts_attr_db_t *input, uint16_t base,
                                      uint8_t *GATT_CHAR_UUID) {

  ASSEGNA_COSE(input[base + allocation_main_characteristic_declaration],
               ESP_GATT_AUTO_RSP, ESP_UUID_LEN_16,
               (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
               GATTS_DEMO_CHAR_VAL_LEN_MAX, sizeof(char_value),
               (uint8_t *)&char_prop_read_write_notify);

  ASSEGNA_COSE(input[base + allocation_main_characteristic_value],
               ESP_GATT_AUTO_RSP, ESP_UUID_LEN_16, GATT_CHAR_UUID,
               ESP_GATT_PERM_READ, CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE,
               (uint8_t *)char_value);

  ASSEGNA_COSE(input[base + allocation_main_characteristic_configuration],
               ESP_GATT_AUTO_RSP, ESP_UUID_LEN_16,
               (uint8_t *)&character_client_config_uuid, ESP_GATT_PERM_READ,
               CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE,
               (uint8_t *)&char_prop_read_write_notify);

  ASSEGNA_COSE(input[base + allocation_main_descriptor_uuid_conf], ESP_GATT_AUTO_RSP,
               ESP_UUID_LEN_16, (uint8_t *)&descriptor_delaration,
               ESP_GATT_PERM_READ, 40, 1, (uint8_t *)&char_prop_read_notify);

  ASSEGNA_COSE(input[base + allocation_main_descriptor_uuid_status], ESP_GATT_AUTO_RSP,
               ESP_UUID_LEN_16, (uint8_t *)&descriptor_delaration,
               ESP_GATT_PERM_READ, sizeof(int32_t), sizeof(int32_t),
               (uint8_t *)&char_prop_read_notify);

}


void allocate_main_dynamic(machine_parameters_t *mp, uint16_t UUIDs[][UUID_INDEX_SIZE], uint16_t UUIDs_len,
                           uint8_t p_srvc_inst_id) {
  MAIN_ENTRY_SIZE = CALC_MAIN_SIZE(UUIDs_len);
  srv_inst_id = p_srvc_inst_id;

  handle_table = (uint16_t *)malloc(sizeof(uint16_t) *
                                          MAIN_ENTRY_SIZE);
  if (!handle_table) {
    ESP_LOGI(TAG, "MO ESPLODE TUTTO 0");
  }
  gatt_db = (esp_gatts_attr_db_t *)malloc(sizeof(esp_gatts_attr_db_t) *
                                          MAIN_ENTRY_SIZE);
  if (!gatt_db) {
    ESP_LOGI(TAG, "MO ESPLODE TUTTO 1");
  }

  if (UUIDs_len > 0) {
    UUIDs_main = malloc(sizeof(uint16_t) * MAIN_ENTRY_SIZE);
    if (!UUIDs_main) {
      ESP_LOGI(TAG, "MAIN MALLOC ESPLOSA");
      return;
    }
  }
  for (int i = 0; i < UUIDs_len; i++) {
    UUIDs_main[i] = generate_uuid(UUID_MAIN_BASE, i);
    ESP_LOGI(TAG, "callin configure index %d", i);
    allocare_una_caratteristica_main(gatt_db,
                                       (1 + i * allocation_main_size),
                                       (uint8_t *)&(UUIDs_main[i]));
  }
  ASSEGNA_COSE(gatt_db[SRV_IDX], // SERVICE
               ESP_GATT_AUTO_RSP, ESP_UUID_LEN_16,
               (uint8_t *)&primary_service_uuid, ESP_GATT_PERM_READ,
               sizeof(uint16_t), sizeof(GATTS_SERVICE_UUID_MAIN),
               (uint8_t *)&GATTS_SERVICE_UUID_MAIN);
}
