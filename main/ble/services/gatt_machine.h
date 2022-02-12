#ifndef GATT_MACHINE_H_
#define GATT_MACHINE_H_
#include <stdint.h>

#include "esp_gatt_defs.h"
#include "esp_gatts_api.h"

typedef enum
{
    GATT_MACHINE_IDX_SERVICE,

    GATT_MACHINE_IDX_STATUS_CHAR,
    GATT_MACHINE_IDX_STATUS_VALUE,

    GATT_MACHINE_IDX_CONFIG_CHAR,
    GATT_MACHINE_IDX_CONFIG_VALUE,

    GATT_MACHINE_IDX_COMMAND_CHAR,
    GATT_MACHINE_IDX_COMMAND_VALUE,

    GATT_MACHINE_IDX_LOGGER_STATUS_CHAR,
    GATT_MACHINE_IDX_LOGGER_STATUS_VALUE,

    GATT_MACHINE_NB
} gatt_machine_t;




//status is notified, readonly
//logger status is notified, as json {logger_status:[reading|writing], completed:percentuale}

//config is read/write. writing the config

//possible command are : [read_conf | ]

// typedef struct {
//     //pointer to send
// } thing_t;

char* get_str_pnt_diocane();

bool gatt_machine_send_status_update_to_client(char* json_status);
bool gatt_machine_send_logger_update_to_client(char* json_status);

uint16_t machine_handle_table[GATT_MACHINE_NB];

extern const uint16_t GATT_MACHINE_UUID_SERVICE;
extern const esp_gatts_attr_db_t gatt_machine_db[GATT_MACHINE_NB];
extern void machine_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
#endif /* DEVINFO_SERVICE_H_ */
