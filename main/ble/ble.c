#include <string.h>

#include "esp_bt.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "nvs_flash.h"

#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_gatt_common_api.h"
#include "esp_gatts_api.h"


#include "ble_lib/ble_utils.h"

#define TAG "GATTS_TABLE_DEMO"


/* One gatt-based profile one app_id and one gatts_if, this array will store the
 * gatts_if returned by ESP_GATTS_REG_EVT */
static struct gatts_profile_inst application_profiles_tab[PROFILE_SIZE] = {
    [PROFILE_MAIN] =
        {
            .gatts_cb = gatts_profile_main_event_handler,
            .gatts_if = ESP_GATT_IF_NONE, /* Not get the gatt_if, so initial is
                                             ESP_GATT_IF_NONE */
        },
    [PROFILE_CONF] =
        {
            .gatts_cb = gatts_profile_conf_event_handler,
            .gatts_if = ESP_GATT_IF_NONE, /* Not get the gatt_if, so initial is
                                             ESP_GATT_IF_NONE */
        },
    [PROFILE_STATUS] =
        {
            .gatts_cb = gatts_profile_status_event_handler,
            .gatts_if = ESP_GATT_IF_NONE, /* Not get the gatt_if, so initial is
                                             ESP_GATT_IF_NONE */
        },

};

void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if,
                         esp_ble_gatts_cb_param_t *param) {

  /* If event is register event, store the gatts_if for each profile */
  if (event == ESP_GATTS_REG_EVT) {
    if (param->reg.status == ESP_GATT_OK) {
      application_profiles_tab[param->reg.app_id].gatts_if = gatts_if;
    } else {
      ESP_LOGE(TAG, "reg app failed, app_id %04x, status %d",
               param->reg.app_id, param->reg.status);
      return;
    }
  }
  do {
    int idx;
    for (idx = 0; idx < PROFILE_SIZE; idx++) {
      /* ESP_GATT_IF_NONE, not specify a certain gatt_if, need to call every
       * profile cb function */
      if (gatts_if == ESP_GATT_IF_NONE ||
          gatts_if == application_profiles_tab[idx].gatts_if) {
        if (application_profiles_tab[idx].gatts_cb) {
          application_profiles_tab[idx].gatts_cb(event, gatts_if, param);
        }
      }
    }
  } while (0);
}

void BLE_init(machine_parameters_t *mp, 
  machine_parameters_t *mp_remote,
  char status_names[][MAX_STR_LEN],
  char conf_names[][MAX_STR_LEN]) {
  esp_err_t ret;


  /* Initialize NVS. */
  ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

  esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
  ret = esp_bt_controller_init(&bt_cfg);
  if (ret) {
    ESP_LOGE(TAG, "%s enable controller failed: %s", __func__,
             esp_err_to_name(ret));
    return;
  }

  ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
  if (ret) {
    ESP_LOGE(TAG, "%s enable controller failed: %s", __func__,
             esp_err_to_name(ret));
    return;
  }

  ret = esp_bluedroid_init();
  if (ret) {
    ESP_LOGE(TAG, "%s init bluetooth failed: %s", __func__,
             esp_err_to_name(ret));
    return;
  }

  ret = esp_bluedroid_enable();
  if (ret) {
    ESP_LOGE(TAG, "%s enable bluetooth failed: %s", __func__,
             esp_err_to_name(ret));
    return;
  }

  uint16_t UUIDs_array[1][2];//={{12,14}};

  allocate_conf_dynamic(mp_remote, conf_names, PROFILE_CONF, &UUIDs_array[0][0]);
  allocate_status_dynamic(mp_remote, status_names, PROFILE_STATUS, &UUIDs_array[0][1]);
  allocate_main_dynamic(mp, UUIDs_array, 1, PROFILE_MAIN);

  ESP_LOGI(TAG, "ALLOCATION DONE");

  ret = esp_ble_gatts_register_callback(gatts_event_handler);
  if (ret) {
    ESP_LOGE(TAG, "gatts register error, error code = %x", ret);
    return;
  }

    ret = esp_ble_gap_register_callback(gap_event_handler);
  if (ret) {
    ESP_LOGE(TAG, "gap register error, error code = %x", ret);
    return;
  }


  ret = esp_ble_gatts_app_register(PROFILE_MAIN);
  if (ret) {
    ESP_LOGE(TAG, "gatts app register error, error code = %x", ret);
    return;
  }
  // ret = esp_ble_gatts_app_register(PROFILE_STATUS);
  // if (ret) {
  //   ESP_LOGE(TAG, "gatts app register error, error code = %x", ret);
  //   return;
  // }

  ret = esp_ble_gatts_app_register(PROFILE_CONF);
  if (ret) {
    ESP_LOGE(TAG, "gatts app register error, error code = %x", ret);
    return;
  }
}
