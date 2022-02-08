
#ifndef GATT_M_STATE_H_
#define GATT_M_STATE_H_
#include <stdint.h>

#include "esp_gatt_defs.h"
#include "esp_gatts_api.h"

typedef enum
{
    GATT_MACHINE_IDX_SERVICE,

    GATT_MACHINE_STATUS_CHAR,
    GATT_MACHINE_STATUS_VALUE,

    GATT_MACHINE_CONFIG_CHAR,
    GATT_MACHINE_CONFIG_VALUE,

    GATT_MACHINE_COMMAND_CHAR,
    GATT_MACHINE_COMMAND_VALUE,

    GATT_MACHINE_LOGGER_STATUS_CHAR,
    GATT_MACHINE_LOGGER_STATUS_VALUE,

    GATT_MACHINE_NB
} gatt_m_state_t;


//status is notified, readonly
//logger status is notified, as json {logger_status:[reading|writing], completed:percentuale}

//config is read/write. writing the config

//possible command are : [read_conf | ]


uint16_t m_state_handle_table[GATT_MACHINE_NB];

extern const uint16_t GATT_M_STATE_UUID_SERVICE;
extern const esp_gatts_attr_db_t gatt_m_state_db[GATT_MACHINE_NB];
extern void m_state_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
#endif /* DEVINFO_SERVICE_H_ */
