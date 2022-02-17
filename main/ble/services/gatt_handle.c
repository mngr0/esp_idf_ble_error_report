
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "esp_bt.h"
#include "esp_gatt_defs.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"

#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"

#include "esp_gatt_common_api.h"

#include "air_ref/logger.h"
#include "ble/ble.h"
#include "cJSON.h"
#include "gatt_handle.h"

#define GATT_UTILS_TAG "UTILS"

uint8_t find_char_and_desr_index(uint16_t *handle_table, uint16_t handle) {
  uint8_t error = 0xff;
  for (int i = 0; i < GATT_HANDLE_NB; i++) {
    if (handle == handle_table[i]) {
      return i;
    }
  }
  return error;
}

// void init_handler(handle_descriptor_t *handle_descriptor, uint16_t* boh){
//   handle_descriptor->gatts_if=boh;
// }
bool gatt_handle_send_conf_update_to_client(
    handle_descriptor_t *handle_descriptor, char *json_status) {
  if (ble_is_connected()) {
    if (current_len == 0) {
      if (handle_descriptor->mtu_size == 200) {
        ESP_LOGI(GATT_UTILS_TAG, " INDICATING CONF UPDATE");
        strcpy((char *)current_buffer, json_status);
        current_idx = 0;
        current_len = strlen(json_status);
        current_size_sent =
            min(handle_descriptor->mtu_size - 5, strlen(json_status));
        esp_ble_gatts_send_indicate(
            (handle_descriptor->profile_inst->gatts_if),
            handle_descriptor->conn_id,

            handle_descriptor->handle_table[GATT_HANDLE_IDX_CONFIG_VALUE],
            current_size_sent, (uint8_t *)json_status, true);
        return true;
      } else {
        ESP_LOGI(GATT_UTILS_TAG, "NOTIFICATION DISCARTED MTU NOT 200 %d",
                 handle_descriptor->mtu_size);
        return false;
      }
    } else {
      ESP_LOGI(GATT_UTILS_TAG, "NOTIFICATION DISCARTED BUSY %d", current_len);
      return false;
    }
  }
  ESP_LOGI(GATT_UTILS_TAG, "NOTIFICATION DISCARTED DISCONNECTED");
  return false;
}


bool gatt_handle_send_status_update_to_client(
    handle_descriptor_t *handle_descriptor, char *json_status) {
  if (ble_is_connected()) {
    if (current_len == 0) {
      if (handle_descriptor->mtu_size == 200) {
        ESP_LOGI(GATT_UTILS_TAG, " INDICATING STATUS UPDATE");
        strcpy((char *)current_buffer, json_status);
        current_idx = 0;
        current_len = strlen(json_status);
        current_size_sent =
            min(handle_descriptor->mtu_size - 5, strlen(json_status));
        esp_ble_gatts_send_indicate(
            (handle_descriptor->profile_inst->gatts_if),
            handle_descriptor->conn_id,

            handle_descriptor->handle_table[GATT_HANDLE_IDX_STATUS_VALUE],
            current_size_sent, (uint8_t *)json_status, true);
        return true;
      } else {
        ESP_LOGI(GATT_UTILS_TAG, "NOTIFICATION DISCARTED MTU NOT 200 %d",
                 handle_descriptor->mtu_size);
        return false;
      }
    } else {
      ESP_LOGI(GATT_UTILS_TAG, "NOTIFICATION DISCARTED BUSY %d", current_len);
      return false;
    }
  }
  ESP_LOGI(GATT_UTILS_TAG, "NOTIFICATION DISCARTED DISCONNECTED");
  return false;
}

bool gatt_handle_send_logger_update_to_client(
    handle_descriptor_t *handle_descriptor, char *json_status) {
  if (ble_is_connected()) {
    if (current_len == 0) {
      if (handle_descriptor->mtu_size == 200) {

        ESP_LOGI(GATT_UTILS_TAG, " INDICATING LOGGER UPDATE");
        strcpy((char *)current_buffer, json_status);
        current_idx = 0;
        current_len = strlen(json_status);
        current_size_sent =
            min(handle_descriptor->mtu_size - 5, strlen(json_status));
        esp_ble_gatts_send_indicate(
            (handle_descriptor->profile_inst->gatts_if),
            handle_descriptor->conn_id,
            handle_descriptor
                ->handle_table[GATT_HANDLE_IDX_HANDLE_STATUS_VALUE],
            current_size_sent, (uint8_t *)json_status, true);
        return true;
      } else {
        ESP_LOGI(GATT_UTILS_TAG, "NOTIFICATION DISCARTED MTU NOT 200 %d",
                 handle_descriptor->mtu_size);
        return false;
      }

    } else {
      ESP_LOGI(GATT_UTILS_TAG, "NOTIFICATION DISCARTED BUSY %d", current_len);
      return false;
    }
  }
  ESP_LOGI(GATT_UTILS_TAG, "NOTIFICATION DISCARTED DISCONNECTED");
  return false;
}

void esp_gatt_confirm_event(esp_ble_gatts_cb_param_t *param,
                            esp_gatt_if_t gatts_if, uint16_t conn_id,
                            uint16_t attr_handle, int mtu_size) {

  if (param->conf.status != ESP_GATT_OK) {
    ESP_LOGI(GATT_UTILS_TAG,
             "ESP_GATTS_CONF_EVT ERROR status %2x attr_handle %d",
             param->conf.status, param->conf.handle);
    // resend last indication or?
    // if(param->conf.status != ESP_GATT_BUSY)
    esp_ble_gatts_send_indicate(gatts_if, conn_id, attr_handle,
                                current_size_sent, &current_buffer[current_idx],
                                true);
    // esp_log_buffer_hex(GATTS_ROUTINE_TAG, param->conf.value,
    // param->conf.len);
  } else {
   // ESP_LOGI(GATT_UTILS_TAG, "ESP_GATTS_CONF_EVT OK status %2x attr_handle %d",
    //         param->conf.status, param->conf.handle);
    // send next piece
    current_idx += current_size_sent;
    current_size_sent =
        min(mtu_size - 5, strlen((char *)current_buffer) - current_idx);
    if (current_size_sent > 0) {
      esp_ble_gatts_send_indicate(gatts_if, conn_id, attr_handle,
                                  current_size_sent,
                                  &current_buffer[current_idx], true);
    } else {
      current_len = 0;
     // ESP_LOGI(GATT_UTILS_TAG, "RESET CURRENT LEN %d", current_len);
    }
  }
}

void reply_read(handle_descriptor_t *handle_descriptor, esp_gatt_if_t gatts_if,
                esp_ble_gatts_cb_param_t *param, char *read_argment) {
  esp_gatt_rsp_t rsp;
  // if(strlen(memory_machine_conf)  > param->read.offset   ){

  memset(&rsp, 0, sizeof(esp_gatt_rsp_t));
  rsp.attr_value.handle = param->read.handle;
  rsp.attr_value.offset = param->read.offset;
  memcpy(rsp.attr_value.value, &read_argment[param->read.offset],
         min(handle_descriptor->mtu_size - 1,
             strlen(read_argment) - param->read.offset));
  rsp.attr_value.len = strlen(read_argment);

  ESP_LOGI(GATT_UTILS_TAG, "RESPONSE LEN %d",
           min(handle_descriptor->mtu_size - 1,
               strlen(read_argment) - param->read.offset));

  esp_ble_gatts_send_response(gatts_if, param->read.conn_id,
                              param->read.trans_id, ESP_GATT_OK, &rsp);
}

void handle_event_handler(char *TAG, handle_descriptor_t *handle_descriptor,
                          esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if,
                          esp_ble_gatts_cb_param_t *param, uint8_t srv_inst_id,
                          const esp_gatts_attr_db_t *gatt_db) {
  esp_ble_gatts_cb_param_t *p_data = (esp_ble_gatts_cb_param_t *)param;
  uint8_t res = 0xff;
  switch (event) {
  case ESP_GATTS_REG_EVT: {
    ESP_LOGI(TAG, "REGISTER_APP_EVT, status %d, app_id %d\n", param->reg.status,
             param->reg.app_id);

    esp_err_t create_attr_ret = esp_ble_gatts_create_attr_tab(
        gatt_db, gatts_if, GATT_HANDLE_NB, srv_inst_id);
    if (create_attr_ret) {
      ESP_LOGE(TAG, "create attr table failed, error code = %x",
               create_attr_ret);
    }
    break;
  }
  case ESP_GATTS_READ_EVT: { // READ MACHINE STATE
    // res = find_char_and_desr_index(handle_descriptor->handle_table,
    //                                p_data->read.handle);
    // ESP_LOGI(TAG, "GATT_READ_EVT, conn_id %d, trans_id %d, handle %d res=%d",
    //          param->read.conn_id, param->read.trans_id, param->read.handle,
    //          res);
    // switch (res) {
    // case GATT_HANDLE_IDX_STATUS_VALUE: {
    //   //reply_read(handle_descriptor, gatts_if, param, handle_descriptor->status);
    //   // send machine status
    //   break;
    // }
    // case GATT_HANDLE_IDX_CONFIG_VALUE: {
    //   //reply_read(handle_descriptor, gatts_if, param, handle_descriptor->config);
    //   break;
    // }
    // case GATT_HANDLE_IDX_COMMAND_VALUE: {
    //   // nothing to say
    //   break;
    // }
    // case GATT_HANDLE_IDX_HANDLE_STATUS_VALUE: {
    //   // case starting_polling (both)
    //   //
    //   // send logger status
    //   // polling conf
    //   // polling status
    //   // writing conf
    //   break;
    // }
    // default: {
    //   break;
    // }
    // }

    break;
  }
  case ESP_GATTS_WRITE_EVT: { // EDIT CONF

    ESP_LOGI("GATT_HANDLE_WRITE",
             "conn_id:%d ,trans_id:%d "
             ",handle:%d,offset:%d,need_rsp:%d,is_prep:%d,len:%d ",
             p_data->write.conn_id, p_data->write.trans_id,
             p_data->write.handle, p_data->write.offset, p_data->write.need_rsp,
             p_data->write.is_prep, p_data->write.len);

    if (!param->write.is_prep) {
      if (param->write.need_rsp) {
        esp_ble_gatts_send_response(gatts_if, param->write.conn_id,
                                    param->write.trans_id, ESP_GATT_OK, NULL);
        break;
      }
    } else {
      /* handle prepare write */
      example_prepare_write_event_env(
          gatts_if, &handle_descriptor->prepare_write_env, param);
    }

    break;
  }
  case ESP_GATTS_EXEC_WRITE_EVT:
    ESP_LOGI(TAG, "ESP_GATTS_EXEC_WRITE_EVT");
    //esp_ble_gatts_send_response(gatts_if, param->write.conn_id,
    //                            param->write.trans_id, ESP_GATT_OK, NULL);
    example_exec_write_event_env_HANDLE(&handle_descriptor->prepare_write_env,
                                 param);





    break;
  case ESP_GATTS_MTU_EVT:
    ESP_LOGI(TAG, "ESP_GATTS_MTU_EVT, MTU %d", param->mtu.mtu);
    handle_descriptor->mtu_size = param->mtu.mtu;
    break;
  case ESP_GATTS_START_EVT:
    ESP_LOGI(TAG, "SERVICE_START_EVT, status %d, service_handle %d",
             param->start.status, param->start.service_handle);
    break;
  case ESP_GATTS_UNREG_EVT:
    break;
  case ESP_GATTS_CREATE_EVT:
    ESP_LOGI(TAG, "CREATE_SERVICE_EVT, status %d,  service_handle %d\n",
             param->create.status, param->create.service_handle);
    break;
  case ESP_GATTS_ADD_INCL_SRVC_EVT:
    break;
  case ESP_GATTS_CONNECT_EVT:
    ESP_LOGI(TAG, "ESP_GATTS_CONNECT_EVT, conn_id = %d",
             param->connect.conn_id);
    esp_log_buffer_hex(TAG, param->connect.remote_bda, 6);
    handle_descriptor->conn_id = p_data->connect.conn_id;

    // TODO CONSIDER IF NEEDED
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

    current_len = 0;
    ESP_LOGI(TAG, "RESET CURRENT LEN %d", current_len);
    break;
  case ESP_GATTS_ADD_CHAR_DESCR_EVT:
    ESP_LOGI(TAG,
             "ADD_DESCR_EVT, status %d, attr_handle %d, service_handle %d\n",
             param->add_char_descr.status, param->add_char_descr.attr_handle,
             param->add_char_descr.service_handle);
    break;
  case ESP_GATTS_DELETE_EVT:
    break;
  case ESP_GATTS_STOP_EVT:
    break;
  case ESP_GATTS_CONF_EVT:

    esp_gatt_confirm_event(
        param, gatts_if, handle_descriptor->conn_id,
        handle_descriptor->handle_table[GATT_HANDLE_IDX_STATUS_VALUE],
        handle_descriptor->mtu_size);

    break;
  case ESP_GATTS_CREAT_ATTR_TAB_EVT: {
    if (param->add_attr_tab.status != ESP_GATT_OK) {
      ESP_LOGE(TAG, "create attribute table failed, error code=0x%x",
               param->add_attr_tab.status);
    } else if (param->add_attr_tab.num_handle != GATT_HANDLE_NB) {
      ESP_LOGE(TAG, "create attribute table abnormally, num_handle (%d) \
                    doesn't equal to HRS_IDX_NB(%d)",
               param->add_attr_tab.num_handle, GATT_HANDLE_NB);
    } else {
      ESP_LOGI(TAG,
               "create attribute table successfully, the number handle = %d\n",
               param->add_attr_tab.num_handle);
      memcpy(handle_descriptor->handle_table, param->add_attr_tab.handles,
             sizeof(handle_descriptor->handle_table));
      esp_ble_gatts_start_service(
          handle_descriptor->handle_table[GATT_HANDLE_IDX_SERVICE]);
    }
    break;
  }
  case ESP_GATTS_DISCONNECT_EVT:
    ESP_LOGI(TAG, "ESP_GATTS_DISCONNECT_EVT, reason = 0x%x",
             param->disconnect.reason);
    break;
  case ESP_GATTS_OPEN_EVT:
  case ESP_GATTS_CANCEL_OPEN_EVT:
  case ESP_GATTS_CLOSE_EVT:
  case ESP_GATTS_LISTEN_EVT:
  case ESP_GATTS_CONGEST_EVT:
  default:
    break;
  }
}