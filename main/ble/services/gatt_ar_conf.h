
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

extern const uint16_t uuid_AR_CONF_SERV;
extern const esp_gatts_attr_db_t ar_conf_serv_gatt_db[GATT_AR_CONF_NB];

#endif /* DEVINFO_SERVICE_H_ */
