#include <string.h>

#include "esp_bt.h"
#include "esp_log.h"

#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_gatt_common_api.h"
#include "esp_gatts_api.h"

#include "ble_utils.h"

#define TAG "BLE_STATUS"

void example_prepare_write_event_env(esp_gatt_if_t gatts_if,
                                     prepare_type_env_t *prepare_write_env,
                                     esp_ble_gatts_cb_param_t *param) {
  ESP_LOGI(TAG, "prepare write, handle = %d, value len = %d",
           param->write.handle, param->write.len);
  esp_gatt_status_t status = ESP_GATT_OK;
  if (prepare_write_env->prepare_buf == NULL) {
    prepare_write_env->prepare_buf =
        (uint8_t *)malloc(PREPARE_BUF_MAX_SIZE * sizeof(uint8_t));
    prepare_write_env->prepare_len = 0;
    if (prepare_write_env->prepare_buf == NULL) {
      ESP_LOGE(TAG, "%s, Gatt_server prep no mem", __func__);
      status = ESP_GATT_NO_RESOURCES;
    }
  } else {
    if (param->write.offset > PREPARE_BUF_MAX_SIZE) {
      status = ESP_GATT_INVALID_OFFSET;
    } else if ((param->write.offset + param->write.len) >
               PREPARE_BUF_MAX_SIZE) {
      status = ESP_GATT_INVALID_ATTR_LEN;
    }
  }
  /*send response when param->write.need_rsp is true */
  if (param->write.need_rsp) {
    esp_gatt_rsp_t *gatt_rsp = (esp_gatt_rsp_t *)malloc(sizeof(esp_gatt_rsp_t));
    if (gatt_rsp != NULL) {
      gatt_rsp->attr_value.len = param->write.len;
      gatt_rsp->attr_value.handle = param->write.handle;
      gatt_rsp->attr_value.offset = param->write.offset;
      gatt_rsp->attr_value.auth_req = ESP_GATT_AUTH_REQ_NONE;
      memcpy(gatt_rsp->attr_value.value, param->write.value, param->write.len);
      esp_err_t response_err =
          esp_ble_gatts_send_response(gatts_if, param->write.conn_id,
                                      param->write.trans_id, status, gatt_rsp);
      if (response_err != ESP_OK) {
        ESP_LOGE(TAG, "Send response error");
      }
      free(gatt_rsp);
    } else {
      ESP_LOGE(TAG, "%s, malloc failed", __func__);
    }
  }
  if (status != ESP_GATT_OK) {
    return;
  }
  memcpy(prepare_write_env->prepare_buf + param->write.offset,
         param->write.value, param->write.len);
  prepare_write_env->prepare_len += param->write.len;
}

void example_exec_write_event_env(prepare_type_env_t *prepare_write_env,
                                  esp_ble_gatts_cb_param_t *param) {
  if (param->exec_write.exec_write_flag == ESP_GATT_PREP_WRITE_EXEC &&
      prepare_write_env->prepare_buf) {
    esp_log_buffer_hex(TAG, prepare_write_env->prepare_buf,
                       prepare_write_env->prepare_len);
  } else {
    ESP_LOGI(TAG, "ESP_GATT_PREP_WRITE_CANCEL");
  }
  if (prepare_write_env->prepare_buf) {
    free(prepare_write_env->prepare_buf);
    prepare_write_env->prepare_buf = NULL;
  }
  prepare_write_env->prepare_len = 0;
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

void allocare_una_caratteristica_conf(esp_gatts_attr_db_t *input, uint16_t base,
                                      uint8_t *GATT_CHAR_UUID) {

  ASSEGNA_COSE(input[base + allocation_conf_characteristic_declaration],
               ESP_GATT_AUTO_RSP, ESP_UUID_LEN_16,
               (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
               GATTS_DEMO_CHAR_VAL_LEN_MAX, sizeof(char_value),
               (uint8_t *)&char_prop_read_write_notify);

  ASSEGNA_COSE(input[base + allocation_conf_characteristic_value],
               ESP_GATT_AUTO_RSP, ESP_UUID_LEN_16, GATT_CHAR_UUID,
               ESP_GATT_PERM_READ, CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE,
               (uint8_t *)char_value);

  ASSEGNA_COSE(input[base + allocation_conf_characteristic_configuration],
               ESP_GATT_AUTO_RSP, ESP_UUID_LEN_16,
               (uint8_t *)&character_client_config_uuid, ESP_GATT_PERM_READ,
               CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE,
               (uint8_t *)&char_prop_read_write_notify);

  ASSEGNA_COSE(input[base + allocation_conf_descriptor_name], ESP_GATT_AUTO_RSP,
               ESP_UUID_LEN_16, (uint8_t *)&descriptor_delaration,
               ESP_GATT_PERM_READ, 40, 1, (uint8_t *)&char_prop_read_notify);

  ASSEGNA_COSE(input[base + allocation_conf_descriptor_min], ESP_GATT_AUTO_RSP,
               ESP_UUID_LEN_16, (uint8_t *)&descriptor_delaration,
               ESP_GATT_PERM_READ, sizeof(int32_t), sizeof(int32_t),
               (uint8_t *)&char_prop_read_notify);

  ASSEGNA_COSE(input[base + allocation_conf_descriptor_max], ESP_GATT_AUTO_RSP,
               ESP_UUID_LEN_16, (uint8_t *)&descriptor_delaration,
               ESP_GATT_PERM_READ, sizeof(int32_t), sizeof(int32_t),
               (uint8_t *)&char_prop_read_notify);
}

void allocare_una_caratteristica_status(esp_gatts_attr_db_t *input,
                                        uint16_t base,
                                        uint8_t *GATT_CHAR_UUID) {

  ASSEGNA_COSE(input[base + allocation_status_characteristic_declaration],
               ESP_GATT_AUTO_RSP, ESP_UUID_LEN_16,
               (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
               GATTS_DEMO_CHAR_VAL_LEN_MAX, sizeof(char_value),
               (uint8_t *)&char_prop_read_write_notify);

  ASSEGNA_COSE(input[base + allocation_status_characteristic_value],
               ESP_GATT_AUTO_RSP, ESP_UUID_LEN_16, GATT_CHAR_UUID,
               ESP_GATT_PERM_READ, CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE,
               (uint8_t *)char_value);

  ASSEGNA_COSE(input[base + allocation_status_characteristic_configuration],
               ESP_GATT_AUTO_RSP, ESP_UUID_LEN_16,
               (uint8_t *)&character_client_config_uuid, ESP_GATT_PERM_READ,
               CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE,
               (uint8_t *)&char_prop_read_write_notify);

  ASSEGNA_COSE(input[base + allocation_status_descriptor_name],
               ESP_GATT_AUTO_RSP, ESP_UUID_LEN_16,
               (uint8_t *)&descriptor_delaration, ESP_GATT_PERM_READ, 40, 1,
               (uint8_t *)&char_prop_read_notify);
}

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
