
#include <string.h>

#include "gel/abstract_state_machine/common.h"

#include "esp_bt.h"
#include "esp_log.h"

#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_gatt_common_api.h"
#include "esp_gatts_api.h"

#include "ble_utils.h"

#define TAG "BLE_CONF"
#define SRV_IDX 0

static const uint16_t primary_service_uuid = ESP_GATT_UUID_PRI_SERVICE;
static const uint16_t GATTS_SERVICE_UUID_CONF = 0x00FE;

static uint16_t *handle_table;
static esp_gatts_attr_db_t *gatt_db;
static uint16_t *UUIDs_conf;
static int32_t *min_values;
static int32_t *max_values;
static uint8_t srv_inst_id;
static uint16_t CONF_ENTRY_SIZE;
static prepare_type_env_t prepare_write_env;

validated_field_t *conf;

void gatts_profile_conf_event_handler(esp_gatts_cb_event_t event,
                                      esp_gatt_if_t gatts_if,
                                      esp_ble_gatts_cb_param_t *param) {
  switch (event) {
  case ESP_GATTS_REG_EVT: {
    esp_err_t create_attr_ret = esp_ble_gatts_create_attr_tab(
        gatt_db, gatts_if, CONF_ENTRY_SIZE, srv_inst_id);
    if (create_attr_ret) {
      ESP_LOGE(TAG, "create attr table failed, error code = %x",
               create_attr_ret);
    }
  } break;
  case ESP_GATTS_READ_EVT:
    ESP_LOGI(TAG, "ESP_GATTS_READ_EVT handle:%d, need_rsp:%d",
             param->read.handle, param->read.need_rsp);
    break;
  case ESP_GATTS_WRITE_EVT:
    // posso voler scrivere solo value, comandare cambio di stato
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
    } else if (param->add_attr_tab.num_handle != CONF_ENTRY_SIZE) {
      ESP_LOGE(TAG, "create attribute table abnormally, num_handle (%d) \
                        doesn't equal to DYNAMIC_SIZE(%d)",
               param->add_attr_tab.num_handle, CONF_ENTRY_SIZE);
    } else {
      //(size1=%d, size2=%d)
      ESP_LOGI(TAG,
               "create attribute table successfully, the number handle = %d \n",
               param->add_attr_tab.num_handle);

      memcpy(handle_table, param->add_attr_tab.handles,
             CONF_ENTRY_SIZE * sizeof(uint16_t));
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
// static const uint8_t char_prop_read = ESP_GATT_CHAR_PROP_BIT_READ;
static const uint8_t char_prop_read_write =
    ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_READ;
static const uint16_t descriptor_delaration = ESP_GATT_UUID_CHAR_DESCRIPTION;
static const uint16_t character_client_config_uuid =
    ESP_GATT_UUID_CHAR_CLIENT_CONFIG;

uint8_t disabled_notification[2] = {0x00, 0x00};

void allocare_una_caratteristica_conf(esp_gatts_attr_db_t *input,
                                      uint16_t index, uint8_t *GATT_CHAR_UUID,
                                      int32_t *value, char *name) {

  // todo la write deve avere un ack.

  ASSEGNA_COSE(
      input[CALC_CONF_SIZE(index) + allocation_conf_characteristic_declaration],
      ESP_GATT_AUTO_RSP, ESP_UUID_LEN_16,
      (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
      CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE,
      (uint8_t *)&char_prop_read_write);

  ASSEGNA_COSE(
      input[CALC_CONF_SIZE(index) + allocation_conf_characteristic_value],
      ESP_GATT_AUTO_RSP, ESP_UUID_LEN_16, GATT_CHAR_UUID,
      ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, sizeof(int32_t),
      sizeof(int32_t), (uint8_t *)value);

  ASSEGNA_COSE(input[CALC_CONF_SIZE(index) +
                     allocation_conf_characteristic_configuration],
               ESP_GATT_AUTO_RSP, ESP_UUID_LEN_16,
               (uint8_t *)&character_client_config_uuid, ESP_GATT_PERM_READ,
               sizeof(uint16_t), sizeof(disabled_notification),
               (uint8_t *)&disabled_notification);

  ASSEGNA_COSE(input[CALC_CONF_SIZE(index) + allocation_conf_descriptor_name],
               ESP_GATT_AUTO_RSP, ESP_UUID_LEN_16,
               (uint8_t *)&descriptor_delaration, ESP_GATT_PERM_READ,
               MAX_STR_LEN, strlen(name), (uint8_t *)name);

  ASSEGNA_COSE(input[CALC_CONF_SIZE(index) + allocation_conf_descriptor_min],
               ESP_GATT_AUTO_RSP, ESP_UUID_LEN_16,
               (uint8_t *)&descriptor_delaration, ESP_GATT_PERM_READ,
               sizeof(int32_t), sizeof(int32_t), (uint8_t *)&min_values[index]);

  ASSEGNA_COSE(input[CALC_CONF_SIZE(index) + allocation_conf_descriptor_max],
               ESP_GATT_AUTO_RSP, ESP_UUID_LEN_16,
               (uint8_t *)&descriptor_delaration, ESP_GATT_PERM_READ,
               sizeof(int32_t), sizeof(int32_t), (uint8_t *)&max_values[index]);
}

void allocate_conf_dynamic(machine_parameters_t *mp, char *names[MAX_STR_LEN],
                           uint8_t p_srvc_inst_id, uint16_t *uuid_ptr) {
  conf = mp->routine_conf;
  CONF_ENTRY_SIZE = CALC_CONF_SIZE(mp->routine_conf_size);
  srv_inst_id = p_srvc_inst_id;

  handle_table = (uint16_t *)malloc(sizeof(uint16_t) * CONF_ENTRY_SIZE);
  if (!handle_table) {
    ESP_LOGI(TAG, "MO ESPLODE TUTTO 0");
  }
  gatt_db = (esp_gatts_attr_db_t *)malloc(sizeof(esp_gatts_attr_db_t) *
                                          CONF_ENTRY_SIZE);
  if (!gatt_db) {
    ESP_LOGI(TAG, "MO ESPLODE TUTTO 1");
  }

  if (mp->routine_conf_size > 0) {
    UUIDs_conf = malloc(sizeof(uint16_t) * CONF_ENTRY_SIZE);
    if (!UUIDs_conf) {
      ESP_LOGI(TAG, "CONF MALLOC ESPLOSA");
      return;
    }
    min_values = malloc(sizeof(int32_t) * mp->routine_conf_size);
    if (!min_values) {
      ESP_LOGI(TAG, "CONF MALLOC ESPLOSA");
      return;
    }
    max_values = malloc(sizeof(int32_t) * mp->routine_conf_size);
    if (!max_values) {
      ESP_LOGI(TAG, "CONF MALLOC ESPLOSA");
      return;
    }
  }
  for (int i = 0; i < mp->routine_conf_size; i++) {
    UUIDs_conf[i] = generate_uuid(UUID_CONF_BASE, i);
    ESP_LOGI(TAG, "callin configure index %d", i);
    min_values[i] = get_limit_value(&mp->routine_conf[i].min_value);
    max_values[i] = get_limit_value(&mp->routine_conf[i].max_value);
    allocare_una_caratteristica_conf(gatt_db, i, (uint8_t *)&(UUIDs_conf[i]),
                                     &mp->routine_conf[i].value, names[i]);
  }
  ASSEGNA_COSE(gatt_db[SRV_IDX], // SERVICE
               ESP_GATT_AUTO_RSP, ESP_UUID_LEN_16,
               (uint8_t *)&primary_service_uuid, ESP_GATT_PERM_READ,
               sizeof(uint16_t), sizeof(GATTS_SERVICE_UUID_CONF),
               (uint8_t *)&GATTS_SERVICE_UUID_CONF);
  *uuid_ptr = GATTS_SERVICE_UUID_CONF;
}

void configure_descriptors(uint16_t index, int32_t min_val, int32_t max_val) {
  esp_ble_gatts_set_attr_value(
      handle_table[CALC_CONF_SIZE(index) + allocation_conf_descriptor_min],
      sizeof(int32_t), (uint8_t *)&min_val);
  esp_ble_gatts_set_attr_value(
      handle_table[CALC_CONF_SIZE(index) + allocation_conf_descriptor_max],
      sizeof(int32_t), (uint8_t *)&max_val);
}

void configure_value(uint16_t index, int32_t value) {
  esp_ble_gatts_set_attr_value(
      handle_table[CALC_CONF_SIZE(index) +
                   allocation_conf_characteristic_value],
      sizeof(int32_t), (uint8_t *)&value);
}
