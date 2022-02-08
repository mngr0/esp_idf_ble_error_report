/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

/****************************************************************************
 *
 * This demo showcases creating a GATT database using a predefined attribute
 *table. It acts as a GATT server and can send adv data, be connected by client.
 * Run the gatt_client demo, the client demo will automatically connect to the
 *gatt_server_service_table demo. Client demo will enable GATT server's notify
 *after connection. The two devices will then exchange data.
 *
 ****************************************************************************/
#include "string.h"

#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "nvs_flash.h"

#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_gatt_common_api.h"
#include "esp_gatts_api.h"

#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_gatt_common_api.h"
#include "esp_gatts_api.h"

#include "ble/ble.h"

#include "services/gatt_ar_conf.h"
#include "services/gatt_ar_state.h"
#include "services/gatt_bluefi.h"
#include "services/gatt_log_state.h"
#include "services/gatt_m_state.h"

#define GATTS_TABLE_TAG "GATTS_TABLE_DEMO"

#define BLINK_GPIO1 2
#define BLINK_GPIO2 15
#define BLINK_GPIO3 13
#define BTN_GPIO 3

// extern machine_state_t m_state;
// extern air_ref_conf_t ar_conf;
// extern air_ref_state_t ar_state;

void gatts_profile_event_handler(esp_gatts_cb_event_t event,
                                 esp_gatt_if_t gatts_if,
                                 esp_ble_gatts_cb_param_t *param);

static uint8_t adv_config_done = 0;

// uint16_t heart_rate_handle_table[HRS_IDX_NB];

static prepare_type_env_t prepare_write_env;

static uint8_t service_uuid[16] = {
    /* LSB
       <-------------------------------------------------------------------------------->
       MSB */
    // first uuid, 16bit, [12],[13] is the value
    0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80,
    0x00, 0x10, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
};

/* The length of adv data must be less than 31 bytes */
static esp_ble_adv_data_t adv_data = {
    .set_scan_rsp = false,
    .include_name = true,
    .include_txpower = true,
    .min_interval = 0x0006, // slave connection min interval, Time =
                            // min_interval * 1.25 msec
    .max_interval = 0x0010, // slave connection max interval, Time =
                            // max_interval * 1.25 msec
    .appearance = 0x00,
    .manufacturer_len = 0,       // TEST_MANUFACTURER_DATA_LEN,
    .p_manufacturer_data = NULL, // test_manufacturer,
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = sizeof(service_uuid),
    .p_service_uuid = service_uuid,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

// scan response data
static esp_ble_adv_data_t scan_rsp_data = {
    .set_scan_rsp = true,
    .include_name = true,
    .include_txpower = true,
    .min_interval = 0x0006,
    .max_interval = 0x0010,
    .appearance = 0x00,
    .manufacturer_len = 0,       // TEST_MANUFACTURER_DATA_LEN,
    .p_manufacturer_data = NULL, //&test_manufacturer[0],
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = sizeof(service_uuid),
    .p_service_uuid = service_uuid,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

static esp_ble_adv_params_t adv_params = {
    .adv_int_min = 0x20,
    .adv_int_max = 0x40,
    .adv_type = ADV_TYPE_IND,
    .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
    .channel_map = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

/* One gatt-based profile one app_id and one gatts_if, this array will store the
 * gatts_if returned by ESP_GATTS_REG_EVT */
struct gatts_profile_inst heart_rate_profile_tab[PROFILE_NUM] = {
    [PROFILE_APP_IDX] =
        {
            .gatts_cb = gatts_profile_event_handler,
            .gatts_if = ESP_GATT_IF_NONE, /* Not get the gatt_if, so initial is
                                             ESP_GATT_IF_NONE */
        },
    [PROFILE_M_STATE_IDX] =
        {
            .gatts_cb = m_state_event_handler,
            .gatts_if = ESP_GATT_IF_NONE, /* Not get the gatt_if, so initial is
                                             ESP_GATT_IF_NONE */

        },
    [PROFILE_AR_CONF_IDX] =
        {
            .gatts_cb = ar_conf_event_handler,
            .gatts_if = ESP_GATT_IF_NONE, /* Not get the gatt_if, so initial is
                                             ESP_GATT_IF_NONE */

        },
    [PROFILE_AR_STATE_IDX] =
        {
            .gatts_cb = ar_state_event_handler,
            .gatts_if = ESP_GATT_IF_NONE, /* Not get the gatt_if, so initial is
                                             ESP_GATT_IF_NONE */

        },

    [PROFILE_BLUEFI_IDX] =
        {
            .gatts_cb = bluefi_event_handler,
            .gatts_if = ESP_GATT_IF_NONE, /* Not get the gatt_if, so initial is
                                             ESP_GATT_IF_NONE */

        },

    [PROFILE_LOG_STATE_IDX] =
        {
            .gatts_cb = log_state_event_handler,
            .gatts_if = ESP_GATT_IF_NONE, /* Not get the gatt_if, so initial is
                                             ESP_GATT_IF_NONE */
        },

};

static void gap_event_handler(esp_gap_ble_cb_event_t event,
                              esp_ble_gap_cb_param_t *param) {
  switch (event) {

  case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
    adv_config_done &= (~ADV_CONFIG_FLAG);
    if (adv_config_done == 0) {
      esp_ble_gap_start_advertising(&adv_params);
    }
    break;
  case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT:
    adv_config_done &= (~SCAN_RSP_CONFIG_FLAG);
    if (adv_config_done == 0) {
      esp_ble_gap_start_advertising(&adv_params);
    }
    break;
  case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
    /* advertising start complete event to indicate advertising start
     * successfully or failed */
    if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS) {
      ESP_LOGE(GATTS_TABLE_TAG, "advertising start failed");
    } else {
      ESP_LOGI(GATTS_TABLE_TAG, "advertising start successfully");
    }
    break;
  case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
    if (param->adv_stop_cmpl.status != ESP_BT_STATUS_SUCCESS) {
      ESP_LOGE(GATTS_TABLE_TAG, "Advertising stop failed");
    } else {
      ESP_LOGI(GATTS_TABLE_TAG, "Stop adv successfully\n");
    }
    break;
  case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
    ESP_LOGI(
        GATTS_TABLE_TAG,
        "update connection params status = %d, min_int = %d, max_int = "
        "%d,conn_int = %d,latency = %d, timeout = %d",
        param->update_conn_params.status, param->update_conn_params.min_int,
        param->update_conn_params.max_int, param->update_conn_params.conn_int,
        param->update_conn_params.latency, param->update_conn_params.timeout);
    break;
  default:
    break;
  }
}

void example_prepare_write_event_env(esp_gatt_if_t gatts_if,
                                     prepare_type_env_t *prepare_write_env,
                                     esp_ble_gatts_cb_param_t *param) {
  ESP_LOGI(GATTS_TABLE_TAG, "prepare write, handle = %d, value len = %d",
           param->write.handle, param->write.len);
  esp_gatt_status_t status = ESP_GATT_OK;
  if (prepare_write_env->prepare_buf == NULL) {
    prepare_write_env->prepare_buf =
        (uint8_t *)malloc(PREPARE_BUF_MAX_SIZE * sizeof(uint8_t));
    prepare_write_env->prepare_len = 0;
    if (prepare_write_env->prepare_buf == NULL) {
      ESP_LOGE(GATTS_TABLE_TAG, "%s, Gatt_server prep no mem", __func__);
      status = ESP_GATT_NO_RESOURCES;
    }
  } else {
    if (param->write.offset > PREPARE_BUF_MAX_SIZE) {
      status = ESP_GATT_INVALID_OFFSET;
    } else if ((param->write.offset + param->write.len) >
               PREPARE_BUF_MAX_SIZE) {
      status = ESP_GATT_INVALID_ATTR_LEN;
    }
  }
  /*send response when param->write.need_rsp is true */
  if (param->write.need_rsp) {
    esp_gatt_rsp_t *gatt_rsp = (esp_gatt_rsp_t *)malloc(sizeof(esp_gatt_rsp_t));
    if (gatt_rsp != NULL) {
      gatt_rsp->attr_value.len = param->write.len;
      gatt_rsp->attr_value.handle = param->write.handle;
      gatt_rsp->attr_value.offset = param->write.offset;
      gatt_rsp->attr_value.auth_req = ESP_GATT_AUTH_REQ_NONE;
      memcpy(gatt_rsp->attr_value.value, param->write.value, param->write.len);
      esp_err_t response_err =
          esp_ble_gatts_send_response(gatts_if, param->write.conn_id,
                                      param->write.trans_id, status, gatt_rsp);
      if (response_err != ESP_OK) {
        ESP_LOGE(GATTS_TABLE_TAG, "Send response error");
      }
      free(gatt_rsp);
    } else {
      ESP_LOGE(GATTS_TABLE_TAG, "%s, malloc failed", __func__);
    }
  }
  if (status != ESP_GATT_OK) {
    return;
  }
  memcpy(prepare_write_env->prepare_buf + param->write.offset,
         param->write.value, param->write.len);
  prepare_write_env->prepare_len += param->write.len;
}

void example_exec_write_event_env(prepare_type_env_t *prepare_write_env,
                                  esp_ble_gatts_cb_param_t *param) {
  if (param->exec_write.exec_write_flag == ESP_GATT_PREP_WRITE_EXEC &&
      prepare_write_env->prepare_buf) {
    esp_log_buffer_hex(GATTS_TABLE_TAG, prepare_write_env->prepare_buf,
                       prepare_write_env->prepare_len);
  } else {
    ESP_LOGI(GATTS_TABLE_TAG, "ESP_GATT_PREP_WRITE_CANCEL");
  }
  if (prepare_write_env->prepare_buf) {
    free(prepare_write_env->prepare_buf);
    prepare_write_env->prepare_buf = NULL;
  }
  prepare_write_env->prepare_len = 0;
}

void gatts_profile_event_handler(esp_gatts_cb_event_t event,
                                 esp_gatt_if_t gatts_if,
                                 esp_ble_gatts_cb_param_t *param) {
  switch (event) {
  case ESP_GATTS_REG_EVT: {
    esp_err_t set_dev_name_ret =
        esp_ble_gap_set_device_name(SAMPLE_DEVICE_NAME);
    if (set_dev_name_ret) {
      ESP_LOGE(GATTS_TABLE_TAG, "set device name failed, error code = %x",
               set_dev_name_ret);
    }
    // config adv data
    esp_err_t ret = esp_ble_gap_config_adv_data(&adv_data);
    if (ret) {
      ESP_LOGE(GATTS_TABLE_TAG, "config adv data failed, error code = %x", ret);
    }
    adv_config_done |= ADV_CONFIG_FLAG;
    // config scan response data
    ret = esp_ble_gap_config_adv_data(&scan_rsp_data);
    if (ret) {
      ESP_LOGE(GATTS_TABLE_TAG,
               "config scan response data failed, error code = %x", ret);
    }
    adv_config_done |= SCAN_RSP_CONFIG_FLAG;

  } break;
  case ESP_GATTS_READ_EVT:
    ESP_LOGI(GATTS_TABLE_TAG, "ESP_GATTS_READ_EVT");
    ESP_LOGI(GATTS_TABLE_TAG,
             "GATT_READ_EVT, conn_id %d, trans_id %d, handle %d\n",
             param->read.conn_id, param->read.trans_id, param->read.handle);
    esp_gatt_rsp_t rsp;
    memset(&rsp, 0, sizeof(esp_gatt_rsp_t));
    rsp.attr_value.handle = param->read.handle;
    rsp.attr_value.len = 4;
    rsp.attr_value.value[0] = 0xde;
    rsp.attr_value.value[1] = 0xed;
    rsp.attr_value.value[2] = 0xbe;
    rsp.attr_value.value[3] = 0xef;
    esp_ble_gatts_send_response(gatts_if, param->read.conn_id,
                                param->read.trans_id, ESP_GATT_OK, &rsp);
    break;
  case ESP_GATTS_WRITE_EVT:
    if (!param->write.is_prep) {
      if (param->write.need_rsp) {
        esp_ble_gatts_send_response(gatts_if, param->write.conn_id,
                                    param->write.trans_id, ESP_GATT_OK, NULL);
      }
    } else {
      /* handle prepare write */
      example_prepare_write_event_env(gatts_if, &prepare_write_env, param);
    }
    break;
  case ESP_GATTS_EXEC_WRITE_EVT:
    // the length of gattc prepare write data must be less than
    // GATTS_DEMO_CHAR_VAL_LEN_MAX.
    ESP_LOGI(GATTS_TABLE_TAG, "ESP_GATTS_EXEC_WRITE_EVT");
    example_exec_write_event_env(&prepare_write_env, param);
    break;
  case ESP_GATTS_MTU_EVT:
    ESP_LOGI(GATTS_TABLE_TAG, "ESP_GATTS_MTU_EVT, MTU %d", param->mtu.mtu);
    break;
  case ESP_GATTS_CONF_EVT:
    ESP_LOGI(GATTS_TABLE_TAG, "ESP_GATTS_CONF_EVT, status = %d, attr_handle %d",
             param->conf.status, param->conf.handle);
    break;
  case ESP_GATTS_START_EVT:
    ESP_LOGI(GATTS_TABLE_TAG, "SERVICE_START_EVT, status %d, service_handle %d",
             param->start.status, param->start.service_handle);
    break;
  case ESP_GATTS_CONNECT_EVT:
    ESP_LOGI(GATTS_TABLE_TAG, "ESP_GATTS_CONNECT_EVT, conn_id = %d",
             param->connect.conn_id);
    esp_log_buffer_hex(GATTS_TABLE_TAG, param->connect.remote_bda, 6);
    esp_ble_conn_update_params_t conn_params = {0};
    memcpy(conn_params.bda, param->connect.remote_bda, sizeof(esp_bd_addr_t));
    /* For the iOS system, please refer to Apple official documents about the
     * BLE connection parameters restrictions. */
    conn_params.latency = 0;
    conn_params.max_int = 0x20; // max_int = 0x20*1.25ms = 40ms // apple: 0x1e 30ms
    conn_params.min_int = 0x10; // min_int = 0x10*1.25ms = 20ms // apple: 0x0c 15ms
    conn_params.timeout = 400;  // timeout = 400*10ms = 4000ms //apple: compreso tra 2 e 6 secondi
    // start sent the update connection parameters to the peer device.
    esp_ble_gap_update_conn_params(&conn_params);
    break;
  case ESP_GATTS_DISCONNECT_EVT:
    ESP_LOGI(GATTS_TABLE_TAG, "ESP_GATTS_DISCONNECT_EVT, reason = 0x%x",
             param->disconnect.reason);
    esp_ble_gap_start_advertising(&adv_params);
    break;
  case ESP_GATTS_CREAT_ATTR_TAB_EVT: {
    if (param->add_attr_tab.status != ESP_GATT_OK) {
      ESP_LOGE(GATTS_TABLE_TAG,
               "create attribute table failed, error code=0x%x",
               param->add_attr_tab.status);
    }

    else if (param->add_attr_tab.svc_uuid.uuid.uuid16 ==
             GATT_AR_CONF_UUID_SERVICE) {
      if (param->add_attr_tab.num_handle != GATT_AR_CONF_NB) {
        ESP_LOGE(GATTS_TABLE_TAG,
                 "create attribute ar conf table abnormally, num_handle (%d) "
                 "isn't equal to INFO_NB(%d)",
                 param->add_attr_tab.num_handle, GATT_AR_CONF_NB);
      } else {
        ESP_LOGI(GATTS_TABLE_TAG,
                 "create attribute ar_conf table successfully, the number "
                 "handle = %d\n",
                 param->add_attr_tab.num_handle);
        memcpy(ar_conf_handle_table, param->add_attr_tab.handles,
               sizeof(ar_conf_handle_table));
        esp_ble_gatts_start_service(
            ar_conf_handle_table[GATT_AR_CONF_IDX_SERVICE]);
      }
    }

    else if (param->add_attr_tab.svc_uuid.uuid.uuid16 ==
             GATT_AR_STATE_UUID_SERVICE) {
      if (param->add_attr_tab.num_handle != GATT_AR_STATE_NB) {
        ESP_LOGE(GATTS_TABLE_TAG,
                 "create attribute ar state table abnormally, num_handle (%d) "
                 "isn't equal to INFO_NB(%d)",
                 param->add_attr_tab.num_handle, GATT_AR_STATE_NB);
      } else {
        ESP_LOGI(GATTS_TABLE_TAG,
                 "create attribute ar state table successfully, the number "
                 "handle = %d\n",
                 param->add_attr_tab.num_handle);
        memcpy(ar_state_handle_table, param->add_attr_tab.handles,
               sizeof(ar_state_handle_table));
        esp_ble_gatts_start_service(
            ar_state_handle_table[GATT_AR_STATE_IDX_SERVICE]);
      }
    }

    else if (param->add_attr_tab.svc_uuid.uuid.uuid16 ==
             GATT_M_STATE_UUID_SERVICE) {
      if (param->add_attr_tab.num_handle != GATT_M_STATE_NB) {
        ESP_LOGE(GATTS_TABLE_TAG,
                 "create attribute m state table abnormally, num_handle (%d) "
                 "isn't equal to INFO_NB(%d)",
                 param->add_attr_tab.num_handle, GATT_M_STATE_NB);
      } else {
        ESP_LOGI(GATTS_TABLE_TAG,
                 "create attribute m state table successfully, the number "
                 "handle = %d\n",
                 param->add_attr_tab.num_handle);
        memcpy(m_state_handle_table, param->add_attr_tab.handles,
               sizeof(m_state_handle_table));
        esp_ble_gatts_start_service(
            m_state_handle_table[GATT_M_STATE_IDX_SERVICE]);
      }
    }

    else if (param->add_attr_tab.svc_uuid.uuid.uuid16 ==
             GATT_LOG_STATE_UUID_SERVICE) {
      if (param->add_attr_tab.num_handle != GATT_LOG_STATE_NB) {
        ESP_LOGE(GATTS_TABLE_TAG,
                 "create attribute log state table abnormally, num_handle (%d) "
                 "isn't equal to INFO_NB(%d)",
                 param->add_attr_tab.num_handle, GATT_LOG_STATE_NB);
      } else {
        ESP_LOGI(GATTS_TABLE_TAG,
                 "create attribute log state table successfully, the number "
                 "handle = %d\n",
                 param->add_attr_tab.num_handle);
        memcpy(log_state_handle_table, param->add_attr_tab.handles,
               sizeof(log_state_handle_table));
        esp_ble_gatts_start_service(
            log_state_handle_table[GATT_LOG_STATE_IDX_SERVICE]);
      }
    }

    break;
  }
  case ESP_GATTS_STOP_EVT:
  case ESP_GATTS_OPEN_EVT:
  case ESP_GATTS_CANCEL_OPEN_EVT:
  case ESP_GATTS_CLOSE_EVT:
  case ESP_GATTS_LISTEN_EVT:
  case ESP_GATTS_CONGEST_EVT:
  case ESP_GATTS_UNREG_EVT:
  case ESP_GATTS_DELETE_EVT:
  default:
    break;
  }
}

static void gatts_event_handler(esp_gatts_cb_event_t event,
                                esp_gatt_if_t gatts_if,
                                esp_ble_gatts_cb_param_t *param) {
  /* If event is register event, store the gatts_if for each profile */
  if (event == ESP_GATTS_REG_EVT) {
    if (param->reg.status == ESP_GATT_OK) {
      heart_rate_profile_tab[param->reg.app_id].gatts_if = gatts_if;
    } else {
      ESP_LOGE(GATTS_TABLE_TAG, "reg app failed, app_id %04x, status %d",
               param->reg.app_id, param->reg.status);
      return;
    }
  }
  do {
    int idx;
    for (idx = 0; idx < PROFILE_NUM; idx++) {
      /* ESP_GATT_IF_NONE, not specify a certain gatt_if, need to call every
       * profile cb function */
      if (gatts_if == ESP_GATT_IF_NONE ||
          gatts_if == heart_rate_profile_tab[idx].gatts_if) {
        if (heart_rate_profile_tab[idx].gatts_cb) {
          heart_rate_profile_tab[idx].gatts_cb(event, gatts_if, param);
        }
      }
    }
  } while (0);
}

void BLE_init(void) {
  esp_err_t ret;

  ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

  esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
  ret = esp_bt_controller_init(&bt_cfg);
  if (ret) {
    ESP_LOGE(GATTS_TABLE_TAG, "%s enable controller failed: %s", __func__,
             esp_err_to_name(ret));
    return;
  }

  ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
  if (ret) {
    ESP_LOGE(GATTS_TABLE_TAG, "%s enable controller failed: %s", __func__,
             esp_err_to_name(ret));
    return;
  }

  ret = esp_bluedroid_init();
  if (ret) {
    ESP_LOGE(GATTS_TABLE_TAG, "%s init bluetooth failed: %s", __func__,
             esp_err_to_name(ret));
    return;
  }

  ret = esp_bluedroid_enable();
  if (ret) {
    ESP_LOGE(GATTS_TABLE_TAG, "%s enable bluetooth failed: %s", __func__,
             esp_err_to_name(ret));
    return;
  }

  esp_ble_gatt_set_local_mtu(500);

  ret = esp_ble_gatts_register_callback(gatts_event_handler);
  if (ret) {
    ESP_LOGE(GATTS_TABLE_TAG, "gatts register error, error code = %x", ret);
    return;
  }

  ret = esp_ble_gap_register_callback(gap_event_handler);
  if (ret) {
    ESP_LOGE(GATTS_TABLE_TAG, "gap register error, error code = %x", ret);
    return;
  }

  ret = esp_ble_gatts_app_register(PROFILE_APP_IDX);
  if (ret) {
    ESP_LOGE(GATTS_TABLE_TAG, "gatts app register error, error code = %x", ret);
    return;
  }
  ret = esp_ble_gatts_app_register(PROFILE_M_STATE_IDX);
  if (ret) {
    ESP_LOGE(GATTS_TABLE_TAG, "gatts app register error, error code = %x", ret);
    return;
  }
  ret = esp_ble_gatts_app_register(PROFILE_AR_CONF_IDX);
  if (ret) {
    ESP_LOGE(GATTS_TABLE_TAG, "gatts app register error, error code = %x", ret);
    return;
  }

  ret = esp_ble_gatts_app_register(PROFILE_AR_STATE_IDX);
  if (ret) {
    ESP_LOGE(GATTS_TABLE_TAG, "gatts app register error, error code = %x", ret);
    return;
  }

  ret = esp_ble_gatts_app_register(PROFILE_LOG_STATE_IDX);
  if (ret) {
    ESP_LOGE(GATTS_TABLE_TAG, "gatts app register error, error code = %x", ret);
    return;
  }

  ret = esp_ble_gatts_app_register(PROFILE_BLUEFI_IDX);
  if (ret) {
    ESP_LOGE(GATTS_TABLE_TAG, "gatts app register error, error code = %x", ret);
    return;
  }

  esp_err_t local_mtu_ret = esp_ble_gatt_set_local_mtu(500);
  if (local_mtu_ret) {
    ESP_LOGE(GATTS_TABLE_TAG, "set local  MTU failed, error code = %x",
             local_mtu_ret);
  }
}
