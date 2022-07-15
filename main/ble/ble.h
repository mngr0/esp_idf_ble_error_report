#ifndef BLE_H_
#define BLE_H_

#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"

#include "gel/abstract_state_machine/common.h"
#include "ble_lib/ble_utils.h"


//#define CONFIG_SET_RAW_ADV_DATA


#define SAMPLE_DEVICE_NAME "AIR_REF_CONFIG"

#define ADV_CONFIG_FLAG (1 << 0)
#define SCAN_RSP_CONFIG_FLAG (1 << 1)


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

void example_exec_write_event_env(prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param);

void BLE_init(machine_parameters_t *mp, machine_parameters_t *mp_remote, char status_names[][MAX_STR_LEN], char conf_names[][MAX_STR_LEN]);

bool ble_is_connected();

//int get_mtu_size();
                                    
#endif