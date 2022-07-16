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


void BLE_init(machine_parameters_t *mp, 
    machine_parameters_t *mp_remote, 
    char status_names[][MAX_STR_LEN], 
    char conf_names[][MAX_STR_LEN]);

bool ble_is_connected();

//int get_mtu_size();
                                    
#endif