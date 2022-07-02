#include "esp_bt.h"
#include "esp_log.h"

#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_gatt_common_api.h"
#include "esp_gatts_api.h"

#include "gel/abstract_state_machine/common.h"
#define MAX_STR_LEN 40

#define GATTS_DEMO_CHAR_VAL_LEN_MAX 500
#define PREPARE_BUF_MAX_SIZE 1024
#define CHAR_DECLARATION_SIZE (sizeof(uint8_t))

enum { PROFILE_MAIN = 0, PROFILE_CONF, PROFILE_STATUS, PROFILE_SIZE };

enum { UUID_INDEX_CONF = 0, UUID_INDEX_STATUS, UUID_INDEX_SIZE };

#define CALC_STATUS_SIZE(status_size) (1 + status_size * allocation_status_size)
#define CALC_CONF_SIZE(conf_size) (1 + conf_size * allocation_conf_size)
#define CALC_MAIN_SIZE(UUIDs_len) (1 + UUIDs_len * allocation_main_size)

typedef struct {
  uint8_t *prepare_buf;
  int prepare_len;
} prepare_type_env_t;

#define ASSEGNA_COSE(dove, AUTO_RSP, UUID_LEN, UUIP_P, PERM, MAX_LEN, LEN,     \
                     VAL)                                                      \
  dove.attr_control.auto_rsp = ESP_GATT_AUTO_RSP;                              \
  dove.att_desc.uuid_length = UUID_LEN;                                        \
  dove.att_desc.uuid_p = UUIP_P;                                               \
  dove.att_desc.perm = PERM;                                                   \
  dove.att_desc.max_length = MAX_LEN;                                          \
  dove.att_desc.length = LEN;                                                  \
  dove.att_desc.value = VAL;

typedef enum {
  allocation_conf_characteristic_declaration = 0,
  allocation_conf_characteristic_value,
  allocation_conf_descriptor_name,
  allocation_conf_descriptor_min,
  allocation_conf_descriptor_max,
  allocation_conf_characteristic_configuration,
  allocation_conf_size
} allocation_conf_t;

typedef enum {
  allocation_status_characteristic_declaration = 0,
  allocation_status_characteristic_value,
  allocation_status_characteristic_configuration,
  allocation_status_descriptor_name,
  allocation_status_size
} allocation_status_t;

typedef enum {
  allocation_main_characteristic_declaration = 0,
  allocation_main_characteristic_value,
  allocation_main_characteristic_configuration,
  allocation_main_descriptor_uuid_conf,
  allocation_main_descriptor_uuid_status,
  allocation_main_size
} allocation_main_t; // todo add sizes?

#define UUID_CONF_BASE 0xF100
#define UUID_STATUS_BASE 0xF200
#define UUID_MAIN_BASE 0xF300

#define UUID_CONF_NAME_BASE 0xF400

static uint16_t inline generate_uuid(uint16_t base, uint16_t index) {
  // ESP_LOGI("BLE_UTILS", "BASE:%04x \tINDEX:%04x(%d) \tRESULT:%04x",base,
  // index,index,(base & 0xff00) | (index & 0x00ff));
  return (base & 0xff00) | (index & 0x00ff);
}

void allocate_conf_dynamic(machine_parameters_t *mp, char *names[MAX_STR_LEN],
                           uint8_t p_srvc_inst_id, uint16_t *uuid_ptr);

void allocate_status_dynamic(machine_parameters_t *mp, char *names[MAX_STR_LEN],
                             uint8_t p_srvc_inst_id, uint16_t *uuid_ptr);

void allocate_main_dynamic(uint16_t UUIDs[][2], uint16_t UUIDs_len,
                           uint8_t srvc_inst_id);

void gatts_profile_conf_event_handler(esp_gatts_cb_event_t event,
                                      esp_gatt_if_t gatts_if,
                                      esp_ble_gatts_cb_param_t *param);
void gatts_profile_status_event_handler(esp_gatts_cb_event_t event,
                                        esp_gatt_if_t gatts_if,
                                        esp_ble_gatts_cb_param_t *param);

void gatts_profile_main_event_handler(esp_gatts_cb_event_t event,
                                      esp_gatt_if_t gatts_if,
                                      esp_ble_gatts_cb_param_t *param);

void gap_event_handler(esp_gap_ble_cb_event_t event,
                       esp_ble_gap_cb_param_t *param);

void example_exec_write_event_env(prepare_type_env_t *prepare_write_env,
                                  esp_ble_gatts_cb_param_t *param);

void example_prepare_write_event_env(esp_gatt_if_t gatts_if,
                                     prepare_type_env_t *prepare_write_env,
                                     esp_ble_gatts_cb_param_t *param);
