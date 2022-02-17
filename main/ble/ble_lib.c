#include "string.h"

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

#include "ble/ble.h"


#define GATTS_TAG "GATTS_LIB"



void example_write_event_env(esp_gatt_if_t gatts_if, prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param){
    esp_gatt_status_t status = ESP_GATT_OK;
    ESP_LOGI(GATTS_TAG, "EXAMPLE_WRITE_EVENT_ENV");
    if (param->write.need_rsp){
        ESP_LOGI(GATTS_TAG, "NEED RSP");
        if (param->write.is_prep){
            if (prepare_write_env->prepare_buf == NULL) {
                prepare_write_env->prepare_buf = (uint8_t *)malloc(PREPARE_BUF_MAX_SIZE*sizeof(uint8_t));
                prepare_write_env->prepare_len = 0;
                if (prepare_write_env->prepare_buf == NULL) {
                    ESP_LOGE(GATTS_TAG, "Gatt_server prep no mem\n");
                    status = ESP_GATT_NO_RESOURCES;
                }
            } else {
                if(param->write.offset > PREPARE_BUF_MAX_SIZE) {
                    status = ESP_GATT_INVALID_OFFSET;
                } else if ((param->write.offset + param->write.len) > PREPARE_BUF_MAX_SIZE) {
                    status = ESP_GATT_INVALID_ATTR_LEN;
                }
            }

            esp_gatt_rsp_t *gatt_rsp = (esp_gatt_rsp_t *)malloc(sizeof(esp_gatt_rsp_t));
            gatt_rsp->attr_value.len = param->write.len;
            gatt_rsp->attr_value.handle = param->write.handle;
            gatt_rsp->attr_value.offset = param->write.offset;
            gatt_rsp->attr_value.auth_req = ESP_GATT_AUTH_REQ_NONE;
            memcpy(gatt_rsp->attr_value.value, param->write.value, param->write.len);
            esp_err_t response_err = esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, status, gatt_rsp);
            if (response_err != ESP_OK){
               ESP_LOGE(GATTS_TAG, "Send response error\n");
            }
            free(gatt_rsp);
            if (status != ESP_GATT_OK){
                return;
            }
            memcpy(prepare_write_env->prepare_buf + param->write.offset,
                   param->write.value,
                   param->write.len);
            prepare_write_env->prepare_len += param->write.len;

        }else{
            esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, status, NULL);
        }
    }
}


void example_prepare_write_event_env(esp_gatt_if_t gatts_if,
                                     prepare_type_env_t *prepare_write_env,
                                     esp_ble_gatts_cb_param_t *param) {
  ESP_LOGI(GATTS_TAG, "prepare write, handle = %d, value len = %d",
           param->write.handle, param->write.len);
  esp_gatt_status_t status = ESP_GATT_OK;
  if (prepare_write_env->prepare_buf == NULL) {
    prepare_write_env->prepare_buf =
        (uint8_t *)malloc(PREPARE_BUF_MAX_SIZE * sizeof(uint8_t));
    prepare_write_env->prepare_len = 0;
    if (prepare_write_env->prepare_buf == NULL) {
      ESP_LOGE(GATTS_TAG, "%s, Gatt_server prep no mem", __func__);
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
        ESP_LOGE(GATTS_TAG, "Send response error");
      }
      free(gatt_rsp);
    } else {
      ESP_LOGE(GATTS_TAG, "%s, malloc failed", __func__);
    }
  }
  if (status != ESP_GATT_OK) {
    return;
  }
  memcpy(prepare_write_env->prepare_buf + param->write.offset,
         param->write.value, param->write.len);
  prepare_write_env->prepare_len += param->write.len;
   ESP_LOGI(GATTS_TAG, "LEN UPDATED TO %d",prepare_write_env->prepare_len);
}


void example_exec_write_event_env_HANDLE(prepare_type_env_t *prepare_write_env,
                                  esp_ble_gatts_cb_param_t *param) {
    ESP_LOGI(GATTS_TAG, "BUFFER OUT EXEC WRITE ENV HANLDE LEN:%d flag:%d",prepare_write_env->prepare_len, param->exec_write.exec_write_flag);
    esp_log_buffer_hex(GATTS_TAG, prepare_write_env->prepare_buf,
                       prepare_write_env->prepare_len);


//ESP_LOGI(GATTS_TAG, "BUFFER OUT EXEC WRITE",prepare_write_env->prepare_len);

  if (param->exec_write.exec_write_flag == ESP_GATT_PREP_WRITE_EXEC &&
      prepare_write_env->prepare_buf) {
    ESP_LOGI(GATTS_TAG, "EXAMPLE SAID OK");
        prepare_write_env->prepare_buf[599]=0;
        prepare_write_env->prepare_buf[prepare_write_env->prepare_len]=0;
        ESP_LOGI(GATTS_TAG, "BUFF AS STRING %s",prepare_write_env->prepare_buf);
        // res = find_char_and_desr_index(handle_descriptor->handle_table,
        //                                p_data->write.handle);
        // switch (res) {
        // case GATT_HANDLE_IDX_CONFIG_VALUE: {
        //   ESP_LOGI(TAG, "RECEIVING WRITE CONFIG");
        //   logger_set_state(logger_state_write_machine_conf);
        //   break;
        // }
        // case GATT_HANDLE_IDX_COMMAND_VALUE: { // if i need to re-read conf
        //   ESP_LOGI(TAG, "RECEIVING WRITE ON COMMAND");
        //   esp_log_buffer_hex(TAG, param->write.value, param->write.len);
        //   // if (param->write.len < strlen((char*)param->write.value)){

        //   cJSON *root;
        //   root = cJSON_ParseWithLength((char *)param->write.value,
        //                                param->write.len);
        //   cJSON *width = cJSON_GetObjectItemCaseSensitive(root, "cmd");
        //   if (cJSON_IsNumber(width)) {
        //     if (width->valueint == 2) {
        //       logger_set_state(logger_state_read_machine_conf);
        //     }
        //   }
        //   cJSON_Delete(root);

        //   break;
        // }
        // }




  } else {
    ESP_LOGI(GATTS_TAG, "ESP_GATT_PREP_WRITE_CANCEL HANLDE");
  }
  if (prepare_write_env->prepare_buf) {
    free(prepare_write_env->prepare_buf);
    prepare_write_env->prepare_buf = NULL;
  }
  prepare_write_env->prepare_len = 0;
}





void example_exec_write_event_env(prepare_type_env_t *prepare_write_env,
                                  esp_ble_gatts_cb_param_t *param) {
    ESP_LOGI(GATTS_TAG, "BUFFER OUT EXEC WRITE ENV");
    esp_log_buffer_hex(GATTS_TAG, prepare_write_env->prepare_buf,
                       prepare_write_env->prepare_len);
  if (param->exec_write.exec_write_flag == ESP_GATT_PREP_WRITE_EXEC &&
      prepare_write_env->prepare_buf) {
    ESP_LOGI(GATTS_TAG, "EXAMPLE SAID OK");
  } else {
    ESP_LOGI(GATTS_TAG, "ESP_GATT_PREP_WRITE_CANCEL");
  }
  if (prepare_write_env->prepare_buf) {
    free(prepare_write_env->prepare_buf);
    prepare_write_env->prepare_buf = NULL;
  }
  prepare_write_env->prepare_len = 0;
}