#ifndef BLE_H_
#define BLE_H_

#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"

#include "ble_lib/ble_utils.h"


void BLE_init(uint16_t size1, uint16_t size2,
    char status_names[][MAX_STR_LEN], 
    char conf_names[][MAX_STR_LEN]);

bool ble_is_connected();

                      
#endif