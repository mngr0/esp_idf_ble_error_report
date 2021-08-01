
#ifndef GATT_AR_STATE_H_
#define GATT_AR_STATE_H_
#include <stdint.h>

#include "esp_gatt_defs.h"
#include "esp_gatts_api.h"

typedef enum
{
    GATT_AR_STATE_IDX_SERVICE,
    GATT_AR_STATE_IDX_CHAR,
    GATT_AR_STATE_IDX_VALUE,
    GATT_AR_STATE_NB
} gatt_ar_state_t;

uint16_t ar_state_handle_table[GATT_AR_STATE_NB];

extern const uint16_t GATT_AR_STATE_UUID_SERVICE;
extern const esp_gatts_attr_db_t gatt_ar_state_db[GATT_AR_STATE_NB];
extern void ar_state_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
#endif /* DEVINFO_SERVICE_H_ */
