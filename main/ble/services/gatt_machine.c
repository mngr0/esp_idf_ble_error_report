
#include <stdint.h>
#include <string.h>

#include "esp_gatt_defs.h"
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

#include "gatt_machine.h"

#include "ble/ble.h"
#include "stdbool.h"

#include "cJSON.h"
#include "air_ref/logger.h"
#define GATTS_MACHINE_TAG "GATTS_MACHINE"

extern void jsonify_machine_status(char**output);


extern uint8_t raw_scan_rsp_data[10];
extern uint8_t raw_adv_data[26];
extern struct gatts_profile_inst machine_state_profile_tab[PROFILE_NUM];

const uint16_t GATT_MACHINE_UUID_SERVICE = (GATT_MACHINE_IDX_SERVICE) | (PROFILE_MACHINE_IDX << 8);
const uint16_t GATT_MACHINE_STATUS_VALUE = ( GATT_MACHINE_IDX_STATUS_VALUE) | (PROFILE_MACHINE_IDX << 8);
const uint16_t GATT_MACHINE_CONFIG_VALUE = (GATT_MACHINE_IDX_CONFIG_VALUE) | (PROFILE_MACHINE_IDX << 8);
const uint16_t GATT_MACHINE_COMMAND_VALUE = ( GATT_MACHINE_IDX_COMMAND_VALUE) | (PROFILE_MACHINE_IDX << 8);
const uint16_t GATT_MACHINE_LOGGER_STATUS_VALUE = ( GATT_MACHINE_IDX_LOGGER_STATUS_VALUE) | (PROFILE_MACHINE_IDX << 8);


static const uint16_t primary_service_uuid = ESP_GATT_UUID_PRI_SERVICE;

static const uint16_t character_declaration_uuid = ESP_GATT_UUID_CHAR_DECLARE;
static const uint8_t char_prop_read_notify = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY | ESP_GATT_CHAR_PROP_BIT_INDICATE;
static const uint8_t char_prop_read_write = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE;
static const uint8_t char_prop_notify =  ESP_GATT_CHAR_PROP_BIT_NOTIFY | ESP_GATT_CHAR_PROP_BIT_INDICATE;
static const uint8_t char_prop_write =  ESP_GATT_CHAR_PROP_BIT_WRITE;

static prepare_type_env_t b_prepare_write_env;
//static esp_gatt_char_prop_t b_property = 0;
static uint16_t spp_conn_id = 0xffff;

static esp_gatt_if_t spp_gatts_if = 0xff;

char memory_machine_status[1024]={'a','a','a','a'};
char memory_machine_conf[1024];
char diocane3[1024]={'c','a','a','a'};
char diocane4[1024]={'d','a','a','a'};

int mtu_size= 20;


char* get_str_pnt_diocane(){
    return memory_machine_conf;
}

const esp_gatts_attr_db_t gatt_machine_db[GATT_MACHINE_NB] =
    {
        [GATT_MACHINE_IDX_SERVICE] =
            {{ESP_GATT_AUTO_RSP},  {ESP_UUID_LEN_16, (uint8_t *)&primary_service_uuid,  ESP_GATT_PERM_READ, 
            sizeof(GATT_MACHINE_UUID_SERVICE), sizeof(GATT_MACHINE_UUID_SERVICE), (uint8_t *)&GATT_MACHINE_UUID_SERVICE}},


        [GATT_MACHINE_IDX_STATUS_CHAR] = //notify only
            {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ| ESP_GATT_PERM_WRITE, //write here should enable/disable notification
             CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_notify}},

        [GATT_MACHINE_IDX_STATUS_VALUE] =
            {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&GATT_MACHINE_STATUS_VALUE, ESP_GATT_PERM_READ,
             GATTS_DEMO_CHAR_VAL_LEN_MAX, 1024, (uint8_t*)memory_machine_status }},//sizeof(machine_state_t), (uint8_t *)&m_state}},


        [GATT_MACHINE_IDX_CONFIG_CHAR] = //read notify write?
            {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ ,
             CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read_write}},

        [GATT_MACHINE_IDX_CONFIG_VALUE] =
            {{ESP_GATT_RSP_BY_APP}, {ESP_UUID_LEN_16, (uint8_t *)&GATT_MACHINE_CONFIG_VALUE, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
             GATTS_DEMO_CHAR_VAL_LEN_MAX, 1024, (uint8_t*)memory_machine_conf}},//sizeof(machine_state_t), (uint8_t *)&m_state}},


        [GATT_MACHINE_IDX_COMMAND_CHAR] = //write only
            {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
             CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_write}},

        [GATT_MACHINE_IDX_COMMAND_VALUE] =
            {{ESP_GATT_RSP_BY_APP}, {ESP_UUID_LEN_16, (uint8_t *)&GATT_MACHINE_COMMAND_VALUE, ESP_GATT_PERM_WRITE,
             GATTS_DEMO_CHAR_VAL_LEN_MAX, 64, (uint8_t*)diocane3}},//sizeof(machine_state_t), (uint8_t *)&m_state}},


        [GATT_MACHINE_IDX_LOGGER_STATUS_CHAR] = // read notify ()
            {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
             CHAR_DECLARATION_SIZE, CHAR_DECLARATION_SIZE, (uint8_t *)&char_prop_read_notify}},

        [GATT_MACHINE_IDX_LOGGER_STATUS_VALUE] =
            {{ESP_GATT_AUTO_RSP}, {ESP_UUID_LEN_16, (uint8_t *)&GATT_MACHINE_LOGGER_STATUS_VALUE, ESP_GATT_PERM_READ,
             GATTS_DEMO_CHAR_VAL_LEN_MAX, 64, (uint8_t*)diocane4}},//sizeof(machine_state_t), (uint8_t *)&m_state}},
};

static uint8_t find_char_and_desr_index(uint16_t handle)
{
    uint8_t error = 0xff;

    for(int i = 0; i < GATT_MACHINE_NB ; i++){
        if(handle == machine_handle_table[i]){
            return i;
        }
        //ESP_LOGI(GATTS_MACHINE_TAG, " handle discarded %d",machine_handle_table[i] );
    }
    return error;
}


#define min(a,b) (a<b ? a:b)


uint8_t tmp_buffer[2048];
uint16_t current_idx=0;
uint16_t current_size_sent=0;
uint16_t current_len=0;



bool gatt_machine_send_status_update_to_client(char* json_status){
    if(ble_is_connected()){
        if(current_len == 0){
            ESP_LOGI(GATTS_MACHINE_TAG, " INDICATING " );
            strcpy((char*)tmp_buffer, json_status);
            current_idx=0;
            current_len=strlen(json_status);
            current_size_sent = min(mtu_size-5, strlen(json_status));
            esp_ble_gatts_send_indicate(heart_rate_profile_tab[PROFILE_MACHINE_IDX].gatts_if, spp_conn_id, machine_handle_table[GATT_MACHINE_IDX_STATUS_VALUE],
                            current_size_sent, (uint8_t*)json_status, true);
            return true;
        }
        else{
            ESP_LOGI(GATTS_MACHINE_TAG, "NOTIFICATION DISCARTED BUSY %d",current_len);
            return false;
        }
    }
    ESP_LOGI(GATTS_MACHINE_TAG, "NOTIFICATION DISCARTED DISCONNECTED");
    return false;
}

bool gatt_machine_send_logger_update_to_client(char* json_status){
    if(ble_is_connected()){
        if(current_len == 0){
            ESP_LOGI(GATTS_MACHINE_TAG, " INDICATING " );
            strcpy((char*)tmp_buffer, json_status);
            current_idx=0;
            current_len=strlen(json_status);
            current_size_sent = min(mtu_size-5, strlen(json_status));
            esp_ble_gatts_send_indicate(heart_rate_profile_tab[PROFILE_MACHINE_IDX].gatts_if, spp_conn_id, machine_handle_table[GATT_MACHINE_IDX_LOGGER_STATUS_VALUE],
                            current_size_sent, (uint8_t*)json_status, true);
            return true;
        }
        else{
            ESP_LOGI(GATTS_MACHINE_TAG, "NOTIFICATION DISCARTED BUSY %d",current_len);
            return false;
        }
    }
    ESP_LOGI(GATTS_MACHINE_TAG, "NOTIFICATION DISCARTED DISCONNECTED");
    return false;
}




void machine_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    esp_ble_gatts_cb_param_t *p_data = (esp_ble_gatts_cb_param_t *) param;
    uint8_t res = 0xff;
    switch (event)
    {
    case ESP_GATTS_REG_EVT:{
        ESP_LOGI(GATTS_MACHINE_TAG, "REGISTER_APP_EVT, status %d, app_id %d\n", param->reg.status, param->reg.app_id);

        esp_err_t create_attr_ret = esp_ble_gatts_create_attr_tab(gatt_machine_db, gatts_if, GATT_MACHINE_NB, PROFILE_MACHINE_IDX);//ultimo parametro modificato
        if (create_attr_ret){
            ESP_LOGE(GATTS_MACHINE_TAG, "create attr table failed, error code = %x", create_attr_ret);
        }

        break;
    }
    case ESP_GATTS_READ_EVT:
    { // READ MACHINE STATE
    res = find_char_and_desr_index(p_data->read.handle);
        ESP_LOGI(GATTS_MACHINE_TAG, "GATT_READ_EVT, conn_id %d, trans_id %d, handle %d res=%d", param->read.conn_id, param->read.trans_id, param->read.handle,res);
        //sned m_state as json
        //res = find_char_and_desr_index(p_data->read.handle);
        switch(res){
            case GATT_MACHINE_IDX_STATUS_VALUE:{
                //send machine status
                break;
            }
            case GATT_MACHINE_IDX_CONFIG_VALUE:{

                ESP_LOGI(GATTS_MACHINE_TAG, "GATT_MACHINE_IDX_CONFIG_VALUE   OFFSET=%d, IS_LONG=%d, NEED_RS=%d",param->read.offset,param->read.is_long,param->read.need_rsp);
                esp_gatt_rsp_t rsp;
                //if(strlen(memory_machine_conf)  > param->read.offset   ){

                    memset(&rsp, 0, sizeof(esp_gatt_rsp_t));
                    rsp.attr_value.handle = param->read.handle;
                    rsp.attr_value.offset = param->read.offset;
                    memcpy(rsp.attr_value.value, &memory_machine_conf[param->read.offset], min(mtu_size-1 ,strlen(memory_machine_conf)-param->read.offset   ) );
                    rsp.attr_value.len = strlen(memory_machine_conf);
                    

                    ESP_LOGI(GATTS_MACHINE_TAG, "RESPONSE LEN %d", min(mtu_size-1 ,strlen(memory_machine_conf)-param->read.offset   )  );
                    
                    
                    esp_ble_gatts_send_response(gatts_if, param->read.conn_id, param->read.trans_id, ESP_GATT_OK, &rsp);
                //}
                break;
            }
            case GATT_MACHINE_IDX_COMMAND_VALUE:{
                //nothing to say
                break;
            }
            case GATT_MACHINE_IDX_LOGGER_STATUS_VALUE:{
                //case starting_polling (both)
                //
                //send logger status
                //polling conf
                //polling status
                //writing conf
                break;
            }
            default:{
                break;
            }
        }

        break;
    }
    case ESP_GATTS_WRITE_EVT:
    { //EDIT CONF
        
        if(p_data->write.is_prep == false){
            res = find_char_and_desr_index(p_data->write.handle);
            switch(res){
                case GATT_MACHINE_IDX_STATUS_VALUE:{
                    //non write
                    break;
                }
                case GATT_MACHINE_IDX_CONFIG_VALUE:{
                    //start config update procedure
                    break;
                }
                case GATT_MACHINE_IDX_COMMAND_VALUE:{
                    //TODO HERE
                    ESP_LOGI(GATTS_MACHINE_TAG, "RECEIVING WRITE ON COMMAND");
                    esp_log_buffer_hex(GATTS_MACHINE_TAG, param->write.value, param->write.len);
                    //if (param->write.len < strlen((char*)param->write.value)){
                        ESP_LOGI(GATTS_MACHINE_TAG, "STRING IS VALID");
                    
                        cJSON *root;
                        root = cJSON_ParseWithLength((char*)param->write.value, param->write.len);
                        cJSON *width = cJSON_GetObjectItemCaseSensitive(root, "cmd");
                        ESP_LOGI(GATTS_MACHINE_TAG, "VALUE IS %d",width->valueint);
                        if (cJSON_IsNumber(width)){
                            ESP_LOGI(GATTS_MACHINE_TAG, "VALUE IS VALID");
                            if(width->valueint == 2){
                                ESP_LOGI(GATTS_MACHINE_TAG, "VALUE IS 2");
                                logger_set_state(logger_state_read_machine_conf);
                            }
                        }
                        cJSON_Delete(root);
                    //}
                    //if command==read conf
                    ///start config read
   
                    break;
                }
                case GATT_MACHINE_IDX_LOGGER_STATUS_VALUE:{
                    //non write
                    break;
                }
                default:{
                    break;
                }
            }
        }


        ESP_LOGI(GATTS_MACHINE_TAG, "GATT_WRITE_EVT, conn_id %d, trans_id %d, handle %d\n", param->write.conn_id, param->write.trans_id, param->write.handle);
        if (!param->write.is_prep)
        {
            ESP_LOGI(GATTS_MACHINE_TAG, "GATT_WRITE_EVT, value len %d, value :", param->write.len);
            esp_log_buffer_hex(GATTS_MACHINE_TAG, param->write.value, param->write.len);
        }
        example_write_event_env(gatts_if, &b_prepare_write_env, param);
        break;
    }
    case ESP_GATTS_EXEC_WRITE_EVT:
        ESP_LOGI(GATTS_MACHINE_TAG, "ESP_GATTS_EXEC_WRITE_EVT");
        esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, ESP_GATT_OK, NULL);
        example_exec_write_event_env(&b_prepare_write_env, param);
        break;
    case ESP_GATTS_MTU_EVT:
        ESP_LOGI(GATTS_MACHINE_TAG, "ESP_GATTS_MTU_EVT, MTU %d", param->mtu.mtu);
        mtu_size = param->mtu.mtu;
        break;
    case ESP_GATTS_START_EVT:
        ESP_LOGI(GATTS_MACHINE_TAG, "SERVICE_START_EVT, status %d, service_handle %d", param->start.status, param->start.service_handle);
        break;
    case ESP_GATTS_UNREG_EVT:
        break;
    case ESP_GATTS_CREATE_EVT:
        ESP_LOGI(GATTS_MACHINE_TAG, "CREATE_SERVICE_EVT, status %d,  service_handle %d\n", param->create.status, param->create.service_handle);
        break;
    case ESP_GATTS_ADD_INCL_SRVC_EVT:
        break;
    case ESP_GATTS_CONNECT_EVT:
        ESP_LOGI(GATTS_MACHINE_TAG, "ESP_GATTS_CONNECT_EVT, conn_id = %d", param->connect.conn_id);
        esp_log_buffer_hex(GATTS_MACHINE_TAG, param->connect.remote_bda, 6);
    	    spp_conn_id = p_data->connect.conn_id;
    	    spp_gatts_if = gatts_if;

        esp_ble_conn_update_params_t conn_params = {0};
        memcpy(conn_params.bda, param->connect.remote_bda, sizeof(esp_bd_addr_t));
        /* For the iOS system, please refer to Apple official documents about the BLE connection parameters restrictions. */
        conn_params.latency = 0;
        conn_params.max_int = 0x20;    // max_int = 0x20*1.25ms = 40ms
        conn_params.min_int = 0x10;    // min_int = 0x10*1.25ms = 20ms
        conn_params.timeout = 400;    // timeout = 400*10ms = 4000ms
        //start sent the update connection parameters to the peer device.
        esp_ble_gap_update_conn_params(&conn_params);
        current_len=0;
        ESP_LOGI(GATTS_MACHINE_TAG, "RESET CURRENT LEN %d",current_len);
        break;
    case ESP_GATTS_ADD_CHAR_DESCR_EVT:
        ESP_LOGI(GATTS_MACHINE_TAG, "ADD_DESCR_EVT, status %d, attr_handle %d, service_handle %d\n",
                 param->add_char_descr.status, param->add_char_descr.attr_handle, param->add_char_descr.service_handle);
        break;
    case ESP_GATTS_DELETE_EVT:
        break;
    case ESP_GATTS_STOP_EVT:
        break;
    case ESP_GATTS_CONF_EVT:
        
        if (param->conf.status != ESP_GATT_OK)
        {
            ESP_LOGI(GATTS_MACHINE_TAG, "ESP_GATTS_CONF_EVT ERROR status %2x attr_handle %d", param->conf.status, param->conf.handle);
            //resend last indication
            //if(param->conf.status != ESP_GATT_BUSY)
            esp_ble_gatts_send_indicate(heart_rate_profile_tab[PROFILE_MACHINE_IDX].gatts_if, spp_conn_id, machine_handle_table[GATT_MACHINE_IDX_STATUS_VALUE],
                        current_size_sent, &tmp_buffer[current_idx], true);
            //esp_log_buffer_hex(GATTS_MACHINE_TAG, param->conf.value, param->conf.len);
        }
        else{
            ESP_LOGI(GATTS_MACHINE_TAG, "ESP_GATTS_CONF_EVT OK status %2x attr_handle %d", param->conf.status, param->conf.handle);
            //send next piece
            current_idx+=current_size_sent;
            current_size_sent = min(mtu_size-5, strlen((char*)tmp_buffer)-current_idx);
            if(current_size_sent>0){
                esp_ble_gatts_send_indicate(heart_rate_profile_tab[PROFILE_MACHINE_IDX].gatts_if, spp_conn_id, machine_handle_table[GATT_MACHINE_IDX_STATUS_VALUE],
                        current_size_sent, &tmp_buffer[current_idx], true);
            }
            else{
                current_len =0;
                ESP_LOGI(GATTS_MACHINE_TAG, "RESET CURRENT LEN %d",current_len);
            }
        }
        break;
    case ESP_GATTS_CREAT_ATTR_TAB_EVT:{
        if (param->add_attr_tab.status != ESP_GATT_OK){
            ESP_LOGE(GATTS_MACHINE_TAG, "create attribute table failed, error code=0x%x", param->add_attr_tab.status);
        }
        else if (param->add_attr_tab.num_handle != GATT_MACHINE_NB){
            ESP_LOGE(GATTS_MACHINE_TAG, "create attribute table abnormally, num_handle (%d) \
                    doesn't equal to HRS_IDX_NB(%d)", param->add_attr_tab.num_handle, GATT_MACHINE_NB);
        }
        else {
            ESP_LOGI(GATTS_MACHINE_TAG, "create attribute table successfully, the number handle = %d\n",param->add_attr_tab.num_handle);
            memcpy(machine_handle_table, param->add_attr_tab.handles, sizeof(machine_handle_table));
            esp_ble_gatts_start_service(machine_handle_table[GATT_MACHINE_IDX_SERVICE]);
        }
        break;
    }
    case ESP_GATTS_DISCONNECT_EVT:
        ESP_LOGI(GATTS_MACHINE_TAG, "ESP_GATTS_DISCONNECT_EVT, reason = 0x%x", param->disconnect.reason);
        //esp_ble_gap_start_advertising(&adv_params);
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