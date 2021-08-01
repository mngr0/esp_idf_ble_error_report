
#ifndef GATT_AR_CONF_H_
#define GATT_AR_CONF_H_
#include <stdint.h>

#include "esp_gatt_defs.h"
#include "esp_gatts_api.h"

typedef enum
{
    GATT_AR_CONF_IDX_SERVICE,
    GATT_AR_CONF_IDX_CHAR,
    GATT_AR_CONF_IDX_VALUE,
    GATT_AR_CONF_NB
} gatt_ar_conf_t;

uint16_t ar_conf_handle_table[GATT_AR_CONF_NB];

extern const uint16_t GATT_AR_CONF_UUID_SERVICE;
extern const esp_gatts_attr_db_t gatt_ar_conf_db[GATT_AR_CONF_NB];
extern void ar_conf_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
#endif /* DEVINFO_SERVICE_H_ */
