
#include <stdint.h>
#include <string.h>

#include "esp_bt.h"
#include "esp_gatt_defs.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "nvs_flash.h"

#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"

#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_gatt_common_api.h"

#include "gatt_handle.h"

#include "ble/ble.h"
#include "stdbool.h"

#include "air_ref/logger.h"
#include "cJSON.h"

#define GATTS_MACHINE_TAG "GATTS_MACHINE"

static const uint16_t GATT_HANDLE_UUID_SERVICE =
    (GATT_HANDLE_IDX_SERVICE) | (PROFILE_MACHINE_IDX << 8);
static const uint16_t GATT_HANDLE_STATUS_VALUE =
    (GATT_HANDLE_IDX_STATUS_VALUE) | (PROFILE_MACHINE_IDX << 8);
static const uint16_t GATT_HANDLE_CONFIG_VALUE =
    (GATT_HANDLE_IDX_CONFIG_VALUE) | (PROFILE_MACHINE_IDX << 8);
static const uint16_t GATT_HANDLE_COMMAND_VALUE =
    (GATT_HANDLE_IDX_COMMAND_VALUE) | (PROFILE_MACHINE_IDX << 8);
static const uint16_t GATT_HANDLE_HANDLE_STATUS_VALUE =
    (GATT_HANDLE_IDX_HANDLE_STATUS_VALUE) | (PROFILE_MACHINE_IDX << 8);

static const uint16_t primary_service_uuid = ESP_GATT_UUID_PRI_SERVICE;

static const uint16_t character_declaration_uuid = ESP_GATT_UUID_CHAR_DECLARE;
static const uint8_t char_prop_read_notify = ESP_GATT_CHAR_PROP_BIT_READ |
                                             ESP_GATT_CHAR_PROP_BIT_NOTIFY |
                                             ESP_GATT_CHAR_PROP_BIT_INDICATE;
static const uint8_t char_prop_read_write =
    ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE;
static const uint8_t char_prop_notify =
    ESP_GATT_CHAR_PROP_BIT_NOTIFY | ESP_GATT_CHAR_PROP_BIT_INDICATE;
static const uint8_t char_prop_write = ESP_GATT_CHAR_PROP_BIT_WRITE;

static handle_descriptor_t machine_descriptor;

const esp_gatts_attr_db_t gatt_machine_db[GATT_HANDLE_NB] = {
    [GATT_HANDLE_IDX_SERVICE] =
        {{ESP_GATT_AUTO_RSP},
         {ESP_UUID_LEN_16, (uint8_t *)&primary_service_uuid, ESP_GATT_PERM_READ,
          sizeof(GATT_HANDLE_UUID_SERVICE), sizeof(GATT_HANDLE_UUID_SERVICE),
          (uint8_t *)&GATT_HANDLE_UUID_SERVICE}},

    [GATT_HANDLE_IDX_STATUS_CHAR] = // notify only
    {{ESP_GATT_AUTO_RSP},
     {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid,
      ESP_GATT_PERM_READ |
          ESP_GATT_PERM_WRITE, // write here should enable/disable notification
      CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE,
      (uint8_t *)&char_prop_notify}},

    [GATT_HANDLE_IDX_STATUS_VALUE] = {{ESP_GATT_RSP_BY_APP},
                                      {ESP_UUID_LEN_16,
                                       (uint8_t *)&GATT_HANDLE_STATUS_VALUE,
                                       ESP_GATT_PERM_READ,
                                       GATTS_DEMO_CHAR_VAL_LEN_MAX, 1024,
                                       (uint8_t *)machine_descriptor.status}},

    [GATT_HANDLE_IDX_CONFIG_CHAR] = // read notify write?
    {{ESP_GATT_AUTO_RSP},
     {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid,
      ESP_GATT_PERM_READ, CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE,
      (uint8_t *)&char_prop_read_write}},

    [GATT_HANDLE_IDX_CONFIG_VALUE] = {{ESP_GATT_RSP_BY_APP},
                                      {ESP_UUID_LEN_16,
                                       (uint8_t *)&GATT_HANDLE_CONFIG_VALUE,
                                       ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                                       GATTS_DEMO_CHAR_VAL_LEN_MAX, 1024,
                                       (uint8_t *)machine_descriptor.config}},

    [GATT_HANDLE_IDX_COMMAND_CHAR] = // write only
    {{ESP_GATT_AUTO_RSP},
     {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid,
      ESP_GATT_PERM_READ, CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE,
      (uint8_t *)&char_prop_write}},

    [GATT_HANDLE_IDX_COMMAND_VALUE] = {{ESP_GATT_RSP_BY_APP},
                                       {ESP_UUID_LEN_16,
                                        (uint8_t *)&GATT_HANDLE_COMMAND_VALUE,
                                        ESP_GATT_PERM_WRITE,
                                        GATTS_DEMO_CHAR_VAL_LEN_MAX, 64,
                                        (uint8_t *)machine_descriptor.command}},

    [GATT_HANDLE_IDX_HANDLE_STATUS_CHAR] = // read notify ()
    {{ESP_GATT_AUTO_RSP},
     {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid,
      ESP_GATT_PERM_READ, CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE,
      (uint8_t *)&char_prop_read_notify}},

    [GATT_HANDLE_IDX_HANDLE_STATUS_VALUE] =
        {{ESP_GATT_RSP_BY_APP},
         {ESP_UUID_LEN_16, (uint8_t *)&GATT_HANDLE_HANDLE_STATUS_VALUE,
          ESP_GATT_PERM_READ, GATTS_DEMO_CHAR_VAL_LEN_MAX, 64,
          (uint8_t *)machine_descriptor.handle_status}},
};

bool gatt_machine_send_status_update_to_client(char *json_status) {
  return gatt_handle_send_status_update_to_client(
      machine_descriptor.handle_table, machine_descriptor.conn_id, json_status,
      machine_descriptor.mtu_size);
}

bool gatt_machine_send_logger_update_to_client(char *json_status) {
  return gatt_handle_send_logger_update_to_client(
      machine_descriptor.handle_table, machine_descriptor.conn_id, json_status,
      machine_descriptor.mtu_size);
}

char *get_str_pnt_diocane() { return machine_descriptor.config; }

void machine_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if,
                           esp_ble_gatts_cb_param_t *param) {
  handle_event_handler(GATTS_MACHINE_TAG, &machine_descriptor, event, gatts_if,
                       param);
}
// esp_ble_gatts_cb_param_t *p_data = (esp_ble_gatts_cb_param_t *) param;
// uint8_t res = 0xff;
// switch (event)
// {
// case ESP_GATTS_REG_EVT:{
//     ESP_LOGI(GATTS_MACHINE_TAG, "REGISTER_APP_EVT, status %d, app_id %d\n",
//     param->reg.status, param->reg.app_id);

//     esp_err_t create_attr_ret =
//     esp_ble_gatts_create_attr_tab(gatt_machine_db, gatts_if,
//     GATT_HANDLE_NB, PROFILE_MACHINE_IDX);//ultimo parametro modificato if
//     (create_attr_ret){
//         ESP_LOGE(GATTS_MACHINE_TAG, "create attr table failed, error code =
//         %x", create_attr_ret);
//     }
//     break;
// }
// case ESP_GATTS_READ_EVT:
// { // READ MACHINE STATE
// res = find_char_and_desr_index(machine_handle_table,p_data->read.handle);

//     ESP_LOGI(GATTS_MACHINE_TAG, "GATT_READ_EVT, conn_id %d, trans_id %d,
//     handle %d res=%d", param->read.conn_id, param->read.trans_id,
//     param->read.handle,res);
//     //sned m_state as json
//     //res = find_char_and_desr_index(p_data->read.handle);
//     switch(res){
//         case GATT_HANDLE_IDX_STATUS_VALUE:{
//             //send machine status
//             break;
//         }
//         case GATT_HANDLE_IDX_CONFIG_VALUE:{

//             ESP_LOGI(GATTS_MACHINE_TAG, "GATT_HANDLE_IDX_CONFIG_VALUE
//             OFFSET=%d, IS_LONG=%d,
//             NEED_RS=%d",param->read.offset,param->read.is_long,param->read.need_rsp);
//             esp_gatt_rsp_t rsp;
//             //if(strlen(memory_machine_conf)  > param->read.offset   ){

//                 memset(&rsp, 0, sizeof(esp_gatt_rsp_t));
//                 rsp.attr_value.handle = param->read.handle;
//                 rsp.attr_value.offset = param->read.offset;
//                 memcpy(rsp.attr_value.value,
//                 &memory_machine_conf[param->read.offset], min(mtu_size-1
//                 ,strlen(memory_machine_conf)-param->read.offset   ) );
//                 rsp.attr_value.len = strlen(memory_machine_conf);

//                 ESP_LOGI(GATTS_MACHINE_TAG, "RESPONSE LEN %d",
//                 min(mtu_size-1
//                 ,strlen(memory_machine_conf)-param->read.offset   )  );

//                 esp_ble_gatts_send_response(gatts_if, param->read.conn_id,
//                 param->read.trans_id, ESP_GATT_OK, &rsp);
//             //}
//             break;
//         }
//         case GATT_HANDLE_IDX_COMMAND_VALUE:{
//             //nothing to say
//             break;
//         }
//         case GATT_HANDLE_IDX_HANDLE_STATUS_VALUE:{
//             //case starting_polling (both)
//             //
//             //send logger status
//             //polling conf
//             //polling status
//             //writing conf
//             break;
//         }
//         default:{
//             break;
//         }
//     }

//     break;
// }
// case ESP_GATTS_WRITE_EVT:
// { //EDIT CONF

//     if(p_data->write.is_prep == false){
//         res =
//         find_char_and_desr_index(machine_handle_table,p_data->write.handle);
//         switch(res){
//             case GATT_HANDLE_IDX_STATUS_VALUE:{
//                 //non write
//                 break;
//             }
//             case GATT_HANDLE_IDX_CONFIG_VALUE:{
//                 logger_set_state(logger_state_write_machine_conf);
//                 break;
//             }
//             case GATT_HANDLE_IDX_COMMAND_VALUE:{
//                 ESP_LOGI(GATTS_MACHINE_TAG, "RECEIVING WRITE ON COMMAND");
//                 esp_log_buffer_hex(GATTS_MACHINE_TAG, param->write.value,
//                 param->write.len);
//                 //if (param->write.len <
//                 strlen((char*)param->write.value)){

//                     cJSON *root;
//                     root = cJSON_ParseWithLength((char*)param->write.value,
//                     param->write.len); cJSON *width =
//                     cJSON_GetObjectItemCaseSensitive(root, "cmd"); if
//                     (cJSON_IsNumber(width)){
//                         if(width->valueint == 2){
//                             logger_set_state(logger_state_read_machine_conf);
//                         }

//                     }
//                     cJSON_Delete(root);

//                 break;
//             }
//             case GATT_HANDLE_IDX_HANDLE_STATUS_VALUE:{
//                 //non write
//                 break;
//             }
//             default:{
//                 break;
//             }
//         }
//     }

//     ESP_LOGI(GATTS_MACHINE_TAG, "GATT_WRITE_EVT, conn_id %d, trans_id %d,
//     handle %d\n", param->write.conn_id, param->write.trans_id,
//     param->write.handle); if (!param->write.is_prep)
//     {
//         ESP_LOGI(GATTS_MACHINE_TAG, "GATT_WRITE_EVT, value len %d, value
//         :", param->write.len); esp_log_buffer_hex(GATTS_MACHINE_TAG,
//         param->write.value, param->write.len);
//     }
//     example_write_event_env(gatts_if, &b_prepare_write_env, param);
//     break;
// }
// case ESP_GATTS_EXEC_WRITE_EVT:
//     ESP_LOGI(GATTS_MACHINE_TAG, "ESP_GATTS_EXEC_WRITE_EVT");
//     esp_ble_gatts_send_response(gatts_if, param->write.conn_id,
//     param->write.trans_id, ESP_GATT_OK, NULL);
//     example_exec_write_event_env(&b_prepare_write_env, param);
//     break;
// case ESP_GATTS_MTU_EVT:
//     ESP_LOGI(GATTS_MACHINE_TAG, "ESP_GATTS_MTU_EVT, MTU %d",
//     param->mtu.mtu); mtu_size = param->mtu.mtu; break;
// case ESP_GATTS_START_EVT:
//     ESP_LOGI(GATTS_MACHINE_TAG, "SERVICE_START_EVT, status %d,
//     service_handle %d", param->start.status, param->start.service_handle);
//     break;
// case ESP_GATTS_UNREG_EVT:
//     break;
// case ESP_GATTS_CREATE_EVT:
//     ESP_LOGI(GATTS_MACHINE_TAG, "CREATE_SERVICE_EVT, status %d,
//     service_handle %d\n", param->create.status,
//     param->create.service_handle); break;
// case ESP_GATTS_ADD_INCL_SRVC_EVT:
//     break;
// case ESP_GATTS_CONNECT_EVT:
//     ESP_LOGI(GATTS_MACHINE_TAG, "ESP_GATTS_CONNECT_EVT, conn_id = %d",
//     param->connect.conn_id); esp_log_buffer_hex(GATTS_MACHINE_TAG,
//     param->connect.remote_bda, 6);
// 	    spp_conn_id = p_data->connect.conn_id;

//     esp_ble_conn_update_params_t conn_params = {0};
//     memcpy(conn_params.bda, param->connect.remote_bda,
//     sizeof(esp_bd_addr_t));
//     /* For the iOS system, please refer to Apple official documents about
//     the BLE connection parameters restrictions. */ conn_params.latency = 0;
//     conn_params.max_int = 0x20;    // max_int = 0x20*1.25ms = 40ms
//     conn_params.min_int = 0x10;    // min_int = 0x10*1.25ms = 20ms
//     conn_params.timeout = 400;    // timeout = 400*10ms = 4000ms
//     //start sent the update connection parameters to the peer device.
//     esp_ble_gap_update_conn_params(&conn_params);
//     current_len=0;
//     ESP_LOGI(GATTS_MACHINE_TAG, "RESET CURRENT LEN %d",current_len);
//     break;
// case ESP_GATTS_ADD_CHAR_DESCR_EVT:
//     ESP_LOGI(GATTS_MACHINE_TAG, "ADD_DESCR_EVT, status %d, attr_handle %d,
//     service_handle %d\n",
//              param->add_char_descr.status,
//              param->add_char_descr.attr_handle,
//              param->add_char_descr.service_handle);
//     break;
// case ESP_GATTS_DELETE_EVT:
//     break;
// case ESP_GATTS_STOP_EVT:
//     break;
// case ESP_GATTS_CONF_EVT:

//     esp_gatt_confirm_event(param,
//                         heart_rate_profile_tab[PROFILE_MACHINE_IDX].gatts_if,
//                         spp_conn_id,
//                         machine_handle_table[GATT_HANDLE_IDX_STATUS_VALUE],mtu_size);

//     break;
// case ESP_GATTS_CREAT_ATTR_TAB_EVT:{
//     if (param->add_attr_tab.status != ESP_GATT_OK){
//         ESP_LOGE(GATTS_MACHINE_TAG, "create attribute table failed, error
//         code=0x%x", param->add_attr_tab.status);
//     }
//     else if (param->add_attr_tab.num_handle != GATT_HANDLE_NB){
//         ESP_LOGE(GATTS_MACHINE_TAG, "create attribute table abnormally,
//         num_handle (%d) doesn't equal to HRS_IDX_NB(%d)",
//         param->add_attr_tab.num_handle, GATT_HANDLE_NB);
//     }
//     else {
//         ESP_LOGI(GATTS_MACHINE_TAG, "create attribute table successfully,
//         the number handle = %d\n",param->add_attr_tab.num_handle);
//         memcpy(machine_handle_table, param->add_attr_tab.handles,
//         sizeof(machine_handle_table));
//         esp_ble_gatts_start_service(machine_handle_table[GATT_HANDLE_IDX_SERVICE]);
//     }
//     break;
// }
// case ESP_GATTS_DISCONNECT_EVT:
//     ESP_LOGI(GATTS_MACHINE_TAG, "ESP_GATTS_DISCONNECT_EVT, reason = 0x%x",
//     param->disconnect.reason); break;
// case ESP_GATTS_OPEN_EVT:
// case ESP_GATTS_CANCEL_OPEN_EVT:
// case ESP_GATTS_CLOSE_EVT:
// case ESP_GATTS_LISTEN_EVT:
// case ESP_GATTS_CONGEST_EVT:
// default:
//     break;
// }
