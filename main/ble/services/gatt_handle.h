#ifndef GATT_HANDLE_H_
#define GATT_HANDLE_H_

#include "ble/ble.h"
#include <stdint.h>

#include "esp_gatt_defs.h"
#include "esp_gatts_api.h"

#define min(a, b) (a < b ? a : b)

typedef enum {
  GATT_HANDLE_IDX_SERVICE,

  GATT_HANDLE_IDX_STATUS_CHAR,
  GATT_HANDLE_IDX_STATUS_VALUE,

  GATT_HANDLE_IDX_CONFIG_CHAR,
  GATT_HANDLE_IDX_CONFIG_VALUE,

  GATT_HANDLE_IDX_COMMAND_CHAR,
  GATT_HANDLE_IDX_COMMAND_VALUE,

  GATT_HANDLE_IDX_HANDLE_STATUS_CHAR,
  GATT_HANDLE_IDX_HANDLE_STATUS_VALUE,

  GATT_HANDLE_NB
} gatt_machine_t;

typedef struct {
  uint16_t handle_table[GATT_HANDLE_NB];
  prepare_type_env_t b_prepare_write_env;
  uint16_t conn_id;
  char status[1024];
  char config[1024];
  char command[1024];
  char handle_status[1024];
  int mtu_size;
} handle_descriptor_t;

bool gatt_handle_send_status_update_to_client(uint16_t *handle_table,
                                              uint16_t conn_id,
                                              char *json_status, int mtu_size);
bool gatt_handle_send_logger_update_to_client(uint16_t *handle_table,
                                              uint16_t conn_id,
                                              char *json_status, int mtu_size);

bool gatt_routine_send_status_update_to_client(char *json_status);
bool gatt_routine_send_logger_update_to_client(char *json_status);
bool gatt_machine_send_status_update_to_client(char *json_status);
bool gatt_machine_send_logger_update_to_client(char *json_status);

uint8_t find_char_and_desr_index(uint16_t *handle_table, uint16_t handle);

extern uint8_t current_buffer[2048];
extern uint16_t current_idx;
extern uint16_t current_size_sent;
extern uint16_t current_len;

void esp_gatt_confirm_event(esp_ble_gatts_cb_param_t *param,
                            esp_gatt_if_t gatts_if, uint16_t spp_conn_id,
                            uint16_t attr_handle, int mtu_size);


void handle_event_handler(char *TAG, handle_descriptor_t *handle_descriptor,
                          esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if,
                          esp_ble_gatts_cb_param_t *param);

// status is notified, readonly
// logger status is notified, as json {logger_status:[reading|writing],
// completed:percentuale}

// config is read/write. writing the config

// possible command are : [read_conf | ]

char *get_str_pnt_diocane();

extern const esp_gatts_attr_db_t gatt_machine_db[GATT_HANDLE_NB];
extern void machine_event_handler(esp_gatts_cb_event_t event,
                                  esp_gatt_if_t gatts_if,
                                  esp_ble_gatts_cb_param_t *param);

extern const esp_gatts_attr_db_t gatt_routine_db[GATT_HANDLE_NB];
extern void routine_event_handler(esp_gatts_cb_event_t event,
                                  esp_gatt_if_t gatts_if,
                                  esp_ble_gatts_cb_param_t *param);

#endif /* DEVINFO_SERVICE_H_ */
