
#ifndef GATT_BLUEFI_H_
#define GATT_BLUEFI_H_
#include <stdint.h>

#include "esp_gatt_defs.h"
#include "esp_gatts_api.h"

typedef enum
{
    GATT_BLUEFI_IDX_SERVICE,
    GATT_BLUEFI_NB
} gatt_bluefi_t;


uint16_t bluefi_handle_table[GATT_BLUEFI_NB];

extern const uint16_t uuid_BLUEFI_SERV;
extern const esp_gatts_attr_db_t bluefi_serv_gatt_db[GATT_BLUEFI_NB];

#endif /* DEVINFO_SERVICE_H_ */
