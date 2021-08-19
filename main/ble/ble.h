#ifndef BLE_H_
#define BLE_H_

#include "esp_bt.h"

#define ESP_APP_ID 0x55
#define ESP_APP_ID_M_STATE 0x56
#define ESP_APP_ID_AR_CONF 0x57
#define ESP_APP_ID_AR_STATE 0x58

#define CONFIG_SET_RAW_ADV_DATA


#define SAMPLE_DEVICE_NAME "AIR_REF_CONF"
#define SVC_INST_ID 0
#define GATTS_DEMO_CHAR_VAL_LEN_MAX 500
#define PREPARE_BUF_MAX_SIZE 1024
#define CHAR_DECLARATION_SIZE (sizeof(uint8_t))

#define ADV_CONFIG_FLAG (1 << 0)
#define SCAN_RSP_CONFIG_FLAG (1 << 1)


//TODO in future, try to understand how the hell blufi works
typedef enum
{
    PROFILE_APP_IDX = 0,
    PROFILE_M_STATE_IDX,
    PROFILE_AR_STATE_IDX,
    PROFILE_AR_CONF_IDX,
    PROFILE_LOG_STATE_IDX,
   // PROFILE_BLUEFI_IDX,
    PROFILE_NUM
} PROFILE_NUMBERS;



typedef struct
{
    uint8_t *prepare_buf;
    int prepare_len;
} prepare_type_env_t;


struct gatts_profile_inst
{
    esp_gatts_cb_t gatts_cb;
    uint16_t gatts_if;
    uint16_t app_id;
    uint16_t conn_id;
    uint16_t service_handle;
    esp_gatt_srvc_id_t service_id;
    uint16_t char_handle;
    esp_bt_uuid_t char_uuid;
    esp_gatt_perm_t perm;
    esp_gatt_char_prop_t property;
    uint16_t descr_handle;
    esp_bt_uuid_t descr_uuid;
};

extern struct gatts_profile_inst heart_rate_profile_tab[PROFILE_NUM];

void example_exec_write_event_env(prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param);

void BLE_init(void);

void example_write_event_env(esp_gatt_if_t gatts_if, prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param);

// void gatts_profile_event_handler(esp_gatts_cb_event_t event,
//                                         esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

// void m_state_event_handler(esp_gatts_cb_event_t event,
//                                   esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

// void ar_state_event_handler(esp_gatts_cb_event_t event,
//                                    esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

// void ar_conf_event_handler(esp_gatts_cb_event_t event,
//                                   esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

// void bluefi_event_handler(esp_gatts_cb_event_t event,
//                                  esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);


#endif