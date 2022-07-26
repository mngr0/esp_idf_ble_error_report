#include <string.h>

#include "esp_bt.h"
#include "esp_log.h"

#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_gatt_common_api.h"
#include "esp_gatts_api.h"

#include "ble_utils.h"

#define TAG "BLE_STATUS"
#define SRV_IDX 0

static const uint16_t primary_service_uuid = ESP_GATT_UUID_PRI_SERVICE;
static const uint16_t GATTS_SERVICE_UUID_STATUS = 0x00FD;

static uint16_t *handle_table;
static esp_gatts_attr_db_t *gatt_db;
static uint16_t *UUIDs_status;
static uint8_t srv_inst_id;
static uint16_t STATUS_ENTRY_SIZE;
static prepare_type_env_t prepare_write_env;

int32_t *status;

void gatts_profile_status_event_handler(esp_gatts_cb_event_t event,
                                        esp_gatt_if_t gatts_if,
                                        esp_ble_gatts_cb_param_t *param) {
  switch (event) {
  case ESP_GATTS_REG_EVT: {
    esp_err_t create_attr_ret = esp_ble_gatts_create_attr_tab(
        gatt_db, gatts_if, STATUS_ENTRY_SIZE, srv_inst_id);
    if (create_attr_ret) {
      ESP_LOGE(TAG, "create attr table failed, error code = %x",
               create_attr_ret);
    }
  } break;
  case ESP_GATTS_READ_EVT:
    ESP_LOGI(TAG, "ESP_GATTS_READ_EVT");
    break;
  case ESP_GATTS_WRITE_EVT:
    if (!param->write.is_prep) {
      // the data length of gattc write  must be less than
      // GATTS_DEMO_CHAR_VAL_LEN_MAX.
      ESP_LOGI(TAG, "GATT_WRITE_EVT, handle = %d, value len = %d, value :",
               param->write.handle, param->write.len);
      esp_log_buffer_hex(TAG, param->write.value, param->write.len);

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
    ESP_LOGI(TAG, "ESP_GATTS_EXEC_WRITE_EVT");
    example_exec_write_event_env(&prepare_write_env, param);
    break;
  case ESP_GATTS_MTU_EVT:
    ESP_LOGI(TAG, "ESP_GATTS_MTU_EVT, MTU %d", param->mtu.mtu);
    break;
  case ESP_GATTS_CONF_EVT:
    ESP_LOGI(TAG, "ESP_GATTS_CONF_EVT, status = %d, attr_handle %d",
             param->conf.status, param->conf.handle);
    break;
  case ESP_GATTS_START_EVT:
    ESP_LOGI(TAG, "SERVICE_START_EVT, status %d, service_handle %d",
             param->start.status, param->start.service_handle);
    break;
  case ESP_GATTS_CONNECT_EVT:
    ESP_LOGI(TAG, "ESP_GATTS_CONNECT_EVT, conn_id = %d",
             param->connect.conn_id);
    esp_log_buffer_hex(TAG, param->connect.remote_bda, 6);
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
    ESP_LOGI(TAG, "ESP_GATTS_DISCONNECT_EVT, reason = 0x%x",
             param->disconnect.reason);
    break;
  case ESP_GATTS_CREAT_ATTR_TAB_EVT: {
    if (param->add_attr_tab.status != ESP_GATT_OK) {
      ESP_LOGE(TAG, "create attribute table failed, error code=0x%x",
               param->add_attr_tab.status);
    } else if (param->add_attr_tab.num_handle != STATUS_ENTRY_SIZE) {
      ESP_LOGE(TAG, "create attribute table abnormally, num_handle (%d) \
                        doesn't equal to DYNAMIC_SIZE(%d)",
               param->add_attr_tab.num_handle, STATUS_ENTRY_SIZE);
    } else {
      //(size1=%d, size2=%d)
      ESP_LOGI(TAG,
               "create attribute table successfully, the number handle = %d \n",
               param->add_attr_tab.num_handle);

      memcpy(handle_table, param->add_attr_tab.handles,
             STATUS_ENTRY_SIZE * sizeof(uint16_t));
      ESP_LOGI(TAG, "memcpy done\n");

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
static const uint8_t char_prop_read_notify =
    ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY;
static const uint16_t descriptor_delaration = ESP_GATT_UUID_CHAR_DESCRIPTION;
static const uint16_t character_client_config_uuid =
    ESP_GATT_UUID_CHAR_CLIENT_CONFIG;

uint16_t *notifications_enabled;

void allocare_una_caratteristica_status(esp_gatts_attr_db_t *input,
                                        uint16_t index, uint8_t *GATT_CHAR_UUID,
                                        int32_t *value, char *name) {

  ASSEGNA_COSE(input[CALC_STATUS_SIZE(index) +
                     allocation_status_characteristic_declaration],
               ESP_GATT_AUTO_RSP, ESP_UUID_LEN_16,
               (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
               CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE,
               (uint8_t *)&char_prop_read_notify);

  ASSEGNA_COSE(
      input[CALC_STATUS_SIZE(index) + allocation_status_characteristic_value],
      ESP_GATT_AUTO_RSP, ESP_UUID_LEN_16, GATT_CHAR_UUID, ESP_GATT_PERM_READ,
      sizeof(int32_t), sizeof(int32_t), (uint8_t *)value);

  ASSEGNA_COSE(input[CALC_STATUS_SIZE(index) +
                     allocation_status_characteristic_configuration],
               ESP_GATT_AUTO_RSP, ESP_UUID_LEN_16,
               (uint8_t *)&character_client_config_uuid,
               ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, sizeof(uint16_t),
               sizeof(uint16_t),
               (uint8_t *)&notifications_enabled[index]);

  ASSEGNA_COSE(
      input[CALC_STATUS_SIZE(index) + allocation_status_descriptor_name],
      ESP_GATT_AUTO_RSP, ESP_UUID_LEN_16, (uint8_t *)&descriptor_delaration,
      ESP_GATT_PERM_READ, MAX_STR_LEN, strlen(name), (uint8_t *)name);
}

static int32_t tmp;

void allocate_status_dynamic (uint16_t size, char names[][MAX_STR_LEN],
                             uint8_t p_srvc_inst_id, uint16_t *uuid_ptr) {

  STATUS_ENTRY_SIZE = CALC_STATUS_SIZE(size);
  srv_inst_id = p_srvc_inst_id;
  handle_table = (uint16_t *)malloc(sizeof(uint16_t) * STATUS_ENTRY_SIZE);
  if (!handle_table) {
    ESP_LOGI(TAG, "malloc handle table failed");
  }
  gatt_db = (esp_gatts_attr_db_t *)malloc(sizeof(esp_gatts_attr_db_t) *
                                          STATUS_ENTRY_SIZE);
  if (!gatt_db) {
    ESP_LOGI(TAG, "malloc gatt db failed");
  }

  if (size > 0) {
    UUIDs_status = malloc(sizeof(uint16_t) * STATUS_ENTRY_SIZE);
    if (!UUIDs_status) {
      ESP_LOGI(TAG, "malloc uuids failed");
      return;
    }
    notifications_enabled = malloc(sizeof(uint16_t) * size);
    if (!notifications_enabled) {
      ESP_LOGI(TAG, "malloc notifications failed");
      return;
    }
  }
  for (int i = 0; i < size; i++) {
    UUIDs_status[i] = generate_uuid(UUID_STATUS_BASE, i);
    ESP_LOGI(TAG, "callin configure index %d", i);
    allocare_una_caratteristica_status(gatt_db, i,
                                       (uint8_t *)&(UUIDs_status[i]),
                                       &tmp, names[i]);
  }
  ASSEGNA_COSE(gatt_db[SRV_IDX], // SERVICE
               ESP_GATT_AUTO_RSP, ESP_UUID_LEN_16,
               (uint8_t *)&primary_service_uuid, ESP_GATT_PERM_READ,
               sizeof(uint16_t), sizeof(GATTS_SERVICE_UUID_STATUS),
               (uint8_t *)&GATTS_SERVICE_UUID_STATUS);

  *uuid_ptr = GATTS_SERVICE_UUID_STATUS;
}
