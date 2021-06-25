
#ifndef GATT_M_STATE_H_
#define GATT_M_STATE_H_
#include <stdint.h>

#include "esp_gatt_defs.h"
#include "esp_gatts_api.h"

typedef enum
{
    GATT_M_STATE_IDX_SERVICE,
    GATT_M_STATE_IDX_CHAR,
    GATT_M_STATE_IDX_VALUE,
    GATT_M_STATE_NB
} gatt_m_state_t;


uint16_t m_state_handle_table[GATT_M_STATE_NB];

extern const uint16_t GATT_M_STATE_UUID_SERVICE;
extern const esp_gatts_attr_db_t gatt_m_state_db[GATT_M_STATE_NB];

#endif /* DEVINFO_SERVICE_H_ */
