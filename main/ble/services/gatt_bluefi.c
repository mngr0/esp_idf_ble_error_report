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

#include "esp_bt.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "nvs_flash.h"

#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"

#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_gatt_common_api.h"

#include "gatt_bluefi.h"

#include "air_ref/air_ref.h"
#include "ble/ble.h"

#define GATTS_BLUEFI_TAG "GATTS_BLUEFI"

extern machine_state_t m_state;
extern air_ref_conf_t ar_conf;
extern air_ref_state_t ar_state;

extern uint8_t raw_scan_rsp_data[10];
extern uint8_t raw_adv_data[26];
extern struct gatts_profile_inst heart_rate_profile_tab[PROFILE_NUM];
const uint16_t GATT_BLUEFI_UUID_SERVICE =
    (0xFF00 | GATT_BLUEFI_IDX_SERVICE) & (PROFILE_BLUEFI_IDX << 8);

const uint16_t primary_service_uuid = ESP_GATT_UUID_PRI_SERVICE;
// static const uint16_t character_declaration_uuid =
// ESP_GATT_UUID_CHAR_DECLARE; static const uint8_t char_prop_read_notify =
// ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY;

/* Full Database Description - Used to add attributes into the database */
const esp_gatts_attr_db_t gatt_bluefi_db[GATT_BLUEFI_NB] = {
    [GATT_BLUEFI_IDX_SERVICE] = {{ESP_GATT_AUTO_RSP},
                                 {ESP_UUID_LEN_16,
                                  (uint8_t *)&primary_service_uuid,
                                  ESP_GATT_PERM_READ, sizeof(uint16_t),
                                  sizeof(GATT_BLUEFI_UUID_SERVICE),
                                  (uint8_t *)&GATT_BLUEFI_UUID_SERVICE}},

};

void bluefi_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if,
                          esp_ble_gatts_cb_param_t *param) {
  switch (event) {
  case ESP_GATTS_REG_EVT:
    ESP_LOGI(GATTS_BLUEFI_TAG, "ESP_GATTS_REG_EVT");
    break;
  case ESP_GATTS_READ_EVT:
    ESP_LOGI(GATTS_BLUEFI_TAG, "ESP_GATTS_READ_EVT");
    break;
  case ESP_GATTS_WRITE_EVT:
    ESP_LOGI(GATTS_BLUEFI_TAG, "ESP_GATTS_WRITE_EVT");
    break;
  case ESP_GATTS_EXEC_WRITE_EVT:
    ESP_LOGI(GATTS_BLUEFI_TAG, "ESP_GATTS_EXEC_WRITE_EVT");
    break;

  case ESP_GATTS_MTU_EVT:
    ESP_LOGI(GATTS_BLUEFI_TAG, "ESP_GATTS_MTU_EVT");
    break;

  case ESP_GATTS_CONF_EVT:
    ESP_LOGI(GATTS_BLUEFI_TAG, "ESP_GATTS_CONF_EVT");
    break;

  case ESP_GATTS_START_EVT:
    ESP_LOGI(GATTS_BLUEFI_TAG, "ESP_GATTS_START_EVT");
    break;

  case ESP_GATTS_CONNECT_EVT:
    ESP_LOGI(GATTS_BLUEFI_TAG, "ESP_GATTS_CONNECT_EVT");
    break;

  case ESP_GATTS_DISCONNECT_EVT:
    ESP_LOGI(GATTS_BLUEFI_TAG, "ESP_GATTS_DISCONNECT_EVT");
    break;

  case ESP_GATTS_CREAT_ATTR_TAB_EVT:
    ESP_LOGI(GATTS_BLUEFI_TAG, "ESP_GATTS_CREAT_ATTR_TAB_EVT");
    break;

  case ESP_GATTS_STOP_EVT:
    ESP_LOGI(GATTS_BLUEFI_TAG, "ESP_GATTS_STOP_EVT");
    break;

  case ESP_GATTS_OPEN_EVT:
    ESP_LOGI(GATTS_BLUEFI_TAG, "ESP_GATTS_OPEN_EVT");
    break;

  case ESP_GATTS_CANCEL_OPEN_EVT:
    ESP_LOGI(GATTS_BLUEFI_TAG, "ESP_GATTS_CANCEL_OPEN_EVT");
    break;

  case ESP_GATTS_CLOSE_EVT:
    ESP_LOGI(GATTS_BLUEFI_TAG, "ESP_GATTS_CLOSE_EVT");
    break;

  case ESP_GATTS_LISTEN_EVT:
    ESP_LOGI(GATTS_BLUEFI_TAG, "ESP_GATTS_LISTEN_EVT");
    break;

  case ESP_GATTS_CONGEST_EVT:
    ESP_LOGI(GATTS_BLUEFI_TAG, "ESP_GATTS_CONGEST_EVT");
    break;

  case ESP_GATTS_UNREG_EVT:
    ESP_LOGI(GATTS_BLUEFI_TAG, "ESP_GATTS_UNREG_EVT");
    break;

  case ESP_GATTS_DELETE_EVT:
    ESP_LOGI(GATTS_BLUEFI_TAG, "ESP_GATTS_DELETE_EVT");
    break;

  default:
    ESP_LOGI(GATTS_BLUEFI_TAG, "SOME EVENT, MTU %d", event);
    break;
  }
}

// static void wifi_event_handler(void* arg, esp_event_base_t event_base,
//                                 int32_t event_id, void* event_data)
// {
//     wifi_event_sta_connected_t *event;
//     wifi_mode_t mode;

//     switch (event_id) {
//     case WIFI_EVENT_STA_START:
//         esp_wifi_connect();
//         break;
//     case WIFI_EVENT_STA_CONNECTED:
//         gl_sta_connected = true;
//         event = (wifi_event_sta_connected_t*) event_data;
//         memcpy(gl_sta_bssid, event->bssid, 6);
//         memcpy(gl_sta_ssid, event->ssid, event->ssid_len);
//         gl_sta_ssid_len = event->ssid_len;
//         break;
//     case WIFI_EVENT_STA_DISCONNECTED:
//         /* This is a workaround as ESP32 WiFi libs don't currently
//            auto-reassociate. */
//         gl_sta_connected = false;
//         memset(gl_sta_ssid, 0, 32);
//         memset(gl_sta_bssid, 0, 6);
//         gl_sta_ssid_len = 0;
//         esp_wifi_connect();
//         xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
//         break;
//     case WIFI_EVENT_AP_START:
//         esp_wifi_get_mode(&mode);

//         /* TODO: get config or information of softap, then set to report
//         extra_info */ if (ble_is_connected == true) {
//             if (gl_sta_connected) {
//                 esp_blufi_send_wifi_conn_report(mode,
//                 ESP_BLUFI_STA_CONN_SUCCESS, 0, NULL);
//             } else {
//                 esp_blufi_send_wifi_conn_report(mode,
//                 ESP_BLUFI_STA_CONN_FAIL, 0, NULL);
//             }
//         } else {
//             BLUFI_INFO("BLUFI BLE is not connected yet\n");
//         }
//         break;
//     case WIFI_EVENT_SCAN_DONE: {
//         uint16_t apCount = 0;
//         esp_wifi_scan_get_ap_num(&apCount);
//         if (apCount == 0) {
//             BLUFI_INFO("Nothing AP found");
//             break;
//         }
//         wifi_ap_record_t *ap_list = (wifi_ap_record_t
//         *)malloc(sizeof(wifi_ap_record_t) * apCount); if (!ap_list) {
//             BLUFI_ERROR("malloc error, ap_list is NULL");
//             break;
//         }
//         ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&apCount, ap_list));
//         esp_blufi_ap_record_t * blufi_ap_list = (esp_blufi_ap_record_t
//         *)malloc(apCount * sizeof(esp_blufi_ap_record_t)); if
//         (!blufi_ap_list) {
//             if (ap_list) {
//                 free(ap_list);
//             }
//             BLUFI_ERROR("malloc error, blufi_ap_list is NULL");
//             break;
//         }
//         for (int i = 0; i < apCount; ++i)
//         {
//             blufi_ap_list[i].rssi = ap_list[i].rssi;
//             memcpy(blufi_ap_list[i].ssid, ap_list[i].ssid,
//             sizeof(ap_list[i].ssid));
//         }

//         if (ble_is_connected == true) {
//             esp_blufi_send_wifi_list(apCount, blufi_ap_list);
//         } else {
//             BLUFI_INFO("BLUFI BLE is not connected yet\n");
//         }

//         esp_wifi_scan_stop();
//         free(ap_list);
//         free(blufi_ap_list);
//         break;
//     }
//     default:
//         break;
//     }
//     return;
// }

// static void initialise_wifi(void)
// {
//     ESP_ERROR_CHECK(esp_netif_init());
//     wifi_event_group = xEventGroupCreate();
//     ESP_ERROR_CHECK(esp_event_loop_create_default());
//     esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
//     assert(sta_netif);
//     ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
//     &wifi_event_handler, NULL));
//     ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
//     &ip_event_handler, NULL));

//     wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
//     ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
//     ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
//     ESP_ERROR_CHECK( esp_wifi_start() );
// }

// //////////////////////////////// FROM EXAMPLE
// //////////////////////////////////////// static esp_blufi_callbacks_t
// example_callbacks = {
//     .event_cb = example_event_callback,
//     .negotiate_data_handler = blufi_dh_negotiate_data_handler,
//     .encrypt_func = blufi_aes_encrypt,
//     .decrypt_func = blufi_aes_decrypt,
//     .checksum_func = blufi_crc_checksum,
// };

// static void example_event_callback(esp_blufi_cb_event_t event,
// esp_blufi_cb_param_t *param)
// {
//     /* actually, should post to blufi_task handle the procedure,
//      * now, as a example, we do it more simply */
//     switch (event) {
//     case ESP_BLUFI_EVENT_INIT_FINISH:
//         BLUFI_INFO("BLUFI init finish\n");

//         esp_blufi_adv_start();
//         break;
//     case ESP_BLUFI_EVENT_DEINIT_FINISH:
//         BLUFI_INFO("BLUFI deinit finish\n");
//         break;
//     case ESP_BLUFI_EVENT_BLE_CONNECT:
//         BLUFI_INFO("BLUFI ble connect\n");
//         ble_is_connected = true;
//         esp_blufi_adv_stop();
//         blufi_security_init();
//         break;
//     case ESP_BLUFI_EVENT_BLE_DISCONNECT:
//         BLUFI_INFO("BLUFI ble disconnect\n");
//         ble_is_connected = false;
//         blufi_security_deinit();
//         esp_blufi_adv_start();
//         break;
//     case ESP_BLUFI_EVENT_SET_WIFI_OPMODE:
//         BLUFI_INFO("BLUFI Set WIFI opmode %d\n", param->wifi_mode.op_mode);
//         ESP_ERROR_CHECK( esp_wifi_set_mode(param->wifi_mode.op_mode) );
//         break;
//     case ESP_BLUFI_EVENT_REQ_CONNECT_TO_AP:
//         BLUFI_INFO("BLUFI requset wifi connect to AP\n");
//         /* there is no wifi callback when the device has already connected to
//         this wifi so disconnect wifi before connection.
//         */
//         esp_wifi_disconnect();
//         esp_wifi_connect();
//         break;
//     case ESP_BLUFI_EVENT_REQ_DISCONNECT_FROM_AP:
//         BLUFI_INFO("BLUFI requset wifi disconnect from AP\n");
//         esp_wifi_disconnect();
//         break;
//     case ESP_BLUFI_EVENT_REPORT_ERROR:
//         BLUFI_ERROR("BLUFI report error, error code %d\n",
//         param->report_error.state);
//         esp_blufi_send_error_info(param->report_error.state);
//         break;
//     case ESP_BLUFI_EVENT_GET_WIFI_STATUS: {
//         wifi_mode_t mode;
//         esp_blufi_extra_info_t info;

//         esp_wifi_get_mode(&mode);

//         if (gl_sta_connected) {
//             memset(&info, 0, sizeof(esp_blufi_extra_info_t));
//             memcpy(info.sta_bssid, gl_sta_bssid, 6);
//             info.sta_bssid_set = true;
//             info.sta_ssid = gl_sta_ssid;
//             info.sta_ssid_len = gl_sta_ssid_len;
//             esp_blufi_send_wifi_conn_report(mode, ESP_BLUFI_STA_CONN_SUCCESS,
//             0, &info);
//         } else {
//             esp_blufi_send_wifi_conn_report(mode, ESP_BLUFI_STA_CONN_FAIL, 0,
//             NULL);
//         }
//         BLUFI_INFO("BLUFI get wifi status from AP\n");

//         break;
//     }
//     case ESP_BLUFI_EVENT_RECV_SLAVE_DISCONNECT_BLE:
//         BLUFI_INFO("blufi close a gatt connection");
//         esp_blufi_disconnect();
//         break;
//     case ESP_BLUFI_EVENT_DEAUTHENTICATE_STA:
//         /* TODO */
//         break;
// 	case ESP_BLUFI_EVENT_RECV_STA_BSSID:
//         memcpy(sta_config.sta.bssid, param->sta_bssid.bssid, 6);
//         sta_config.sta.bssid_set = 1;
//         esp_wifi_set_config(WIFI_IF_STA, &sta_config);
//         BLUFI_INFO("Recv STA BSSID %s\n", sta_config.sta.ssid);
//         break;
// 	case ESP_BLUFI_EVENT_RECV_STA_SSID:
//         strncpy((char *)sta_config.sta.ssid, (char *)param->sta_ssid.ssid,
//         param->sta_ssid.ssid_len);
//         sta_config.sta.ssid[param->sta_ssid.ssid_len] = '\0';
//         esp_wifi_set_config(WIFI_IF_STA, &sta_config);
//         BLUFI_INFO("Recv STA SSID %s\n", sta_config.sta.ssid);
//         break;
// 	case ESP_BLUFI_EVENT_RECV_STA_PASSWD:
//         strncpy((char *)sta_config.sta.password, (char
//         *)param->sta_passwd.passwd, param->sta_passwd.passwd_len);
//         sta_config.sta.password[param->sta_passwd.passwd_len] = '\0';
//         esp_wifi_set_config(WIFI_IF_STA, &sta_config);
//         BLUFI_INFO("Recv STA PASSWORD %s\n", sta_config.sta.password);
//         break;
// 	case ESP_BLUFI_EVENT_RECV_SOFTAP_SSID:
//         strncpy((char *)ap_config.ap.ssid, (char *)param->softap_ssid.ssid,
//         param->softap_ssid.ssid_len);
//         ap_config.ap.ssid[param->softap_ssid.ssid_len] = '\0';
//         ap_config.ap.ssid_len = param->softap_ssid.ssid_len;
//         esp_wifi_set_config(WIFI_IF_AP, &ap_config);
//         BLUFI_INFO("Recv SOFTAP SSID %s, ssid len %d\n", ap_config.ap.ssid,
//         ap_config.ap.ssid_len); break;
// 	case ESP_BLUFI_EVENT_RECV_SOFTAP_PASSWD:
//         strncpy((char *)ap_config.ap.password, (char
//         *)param->softap_passwd.passwd, param->softap_passwd.passwd_len);
//         ap_config.ap.password[param->softap_passwd.passwd_len] = '\0';
//         esp_wifi_set_config(WIFI_IF_AP, &ap_config);
//         BLUFI_INFO("Recv SOFTAP PASSWORD %s len = %d\n",
//         ap_config.ap.password, param->softap_passwd.passwd_len); break;
// 	case ESP_BLUFI_EVENT_RECV_SOFTAP_MAX_CONN_NUM:
//         if (param->softap_max_conn_num.max_conn_num > 4) {
//             return;
//         }
//         ap_config.ap.max_connection =
//         param->softap_max_conn_num.max_conn_num;
//         esp_wifi_set_config(WIFI_IF_AP, &ap_config);
//         BLUFI_INFO("Recv SOFTAP MAX CONN NUM %d\n",
//         ap_config.ap.max_connection); break;
// 	case ESP_BLUFI_EVENT_RECV_SOFTAP_AUTH_MODE:
//         if (param->softap_auth_mode.auth_mode >= WIFI_AUTH_MAX) {
//             return;
//         }
//         ap_config.ap.authmode = param->softap_auth_mode.auth_mode;
//         esp_wifi_set_config(WIFI_IF_AP, &ap_config);
//         BLUFI_INFO("Recv SOFTAP AUTH MODE %d\n", ap_config.ap.authmode);
//         break;
// 	case ESP_BLUFI_EVENT_RECV_SOFTAP_CHANNEL:
//         if (param->softap_channel.channel > 13) {
//             return;
//         }
//         ap_config.ap.channel = param->softap_channel.channel;
//         esp_wifi_set_config(WIFI_IF_AP, &ap_config);
//         BLUFI_INFO("Recv SOFTAP CHANNEL %d\n", ap_config.ap.channel);
//         break;
//     case ESP_BLUFI_EVENT_GET_WIFI_LIST:{
//         wifi_scan_config_t scanConf = {
//             .ssid = NULL,
//             .bssid = NULL,
//             .channel = 0,
//             .show_hidden = false
//         };
//         esp_wifi_scan_start(&scanConf, true);
//         break;
//     }
//     case ESP_BLUFI_EVENT_RECV_CUSTOM_DATA:
//         BLUFI_INFO("Recv Custom Data %d\n", param->custom_data.data_len);
//         esp_log_buffer_hex("Custom Data", param->custom_data.data,
//         param->custom_data.data_len); break;
// 	case ESP_BLUFI_EVENT_RECV_USERNAME:
//         /* Not handle currently */
//         break;
// 	case ESP_BLUFI_EVENT_RECV_CA_CERT:
//         /* Not handle currently */
//         break;
// 	case ESP_BLUFI_EVENT_RECV_CLIENT_CERT:
//         /* Not handle currently */
//         break;
// 	case ESP_BLUFI_EVENT_RECV_SERVER_CERT:
//         /* Not handle currently */
//         break;
// 	case ESP_BLUFI_EVENT_RECV_CLIENT_PRIV_KEY:
//         /* Not handle currently */
//         break;;
// 	case ESP_BLUFI_EVENT_RECV_SERVER_PRIV_KEY:
//         /* Not handle currently */
//         break;
//     default:
//         break;
//     }
// }

// void btc_blufi_recv_handler(uint8_t *data, int len)
// {
//     struct blufi_hdr *hdr = (struct blufi_hdr *)data;
//     uint16_t checksum, checksum_pkt;
//     int ret;

//     if (hdr->seq != blufi_env.recv_seq) {
//         BTC_TRACE_ERROR("%s seq %d is not expect %d\n", __func__, hdr->seq,
//         blufi_env.recv_seq + 1);
//         btc_blufi_report_error(ESP_BLUFI_SEQUENCE_ERROR);
//         return;
//     }

//     blufi_env.recv_seq++;

//     // first step, decrypt
//     if (BLUFI_FC_IS_ENC(hdr->fc)
//             && (blufi_env.cbs && blufi_env.cbs->decrypt_func)) {
//         ret = blufi_env.cbs->decrypt_func(hdr->seq, hdr->data,
//         hdr->data_len); if (ret != hdr->data_len) { /* enc must be success
//         and enc len must equal to plain len */
//             BTC_TRACE_ERROR("%s decrypt error %d\n", __func__, ret);
//             btc_blufi_report_error(ESP_BLUFI_DECRYPT_ERROR);
//             return;
//         }
//     }

//     // second step, check sum
//     if (BLUFI_FC_IS_CHECK(hdr->fc)
//             && (blufi_env.cbs && blufi_env.cbs->checksum_func)) {
//         checksum = blufi_env.cbs->checksum_func(hdr->seq, &hdr->seq,
//         hdr->data_len + 2); checksum_pkt = hdr->data[hdr->data_len] |
//         (((uint16_t) hdr->data[hdr->data_len + 1]) << 8); if (checksum !=
//         checksum_pkt) {
//             BTC_TRACE_ERROR("%s checksum error %04x, pkt %04x\n", __func__,
//             checksum, checksum_pkt);
//             btc_blufi_report_error(ESP_BLUFI_CHECKSUM_ERROR);
//             return;
//         }
//     }

//     if (BLUFI_FC_IS_REQ_ACK(hdr->fc)) {
//         btc_blufi_send_ack(hdr->seq);
//     }

//     if (BLUFI_FC_IS_FRAG(hdr->fc)) {
//         if (blufi_env.offset == 0) {
//             blufi_env.total_len = hdr->data[0] | (((uint16_t) hdr->data[1])
//             << 8); blufi_env.aggr_buf = osi_malloc(blufi_env.total_len); if
//             (blufi_env.aggr_buf == NULL) {
//                 BTC_TRACE_ERROR("%s no mem, len %d\n", __func__,
//                 blufi_env.total_len);
//                 btc_blufi_report_error(ESP_BLUFI_DH_MALLOC_ERROR);
//                 return;
//             }
//         }
//         if (blufi_env.offset + hdr->data_len  - 2 <= blufi_env.total_len){
//             memcpy(blufi_env.aggr_buf + blufi_env.offset, hdr->data + 2,
//             hdr->data_len  - 2); blufi_env.offset += (hdr->data_len - 2);
//         } else {
//             BTC_TRACE_ERROR("%s payload is longer than packet length, len %d
//             \n", __func__, blufi_env.total_len);
//             btc_blufi_report_error(ESP_BLUFI_DATA_FORMAT_ERROR);
//             return;
//         }

//     } else {
//         if (blufi_env.offset > 0) {   /* if previous pkt is frag */
//             memcpy(blufi_env.aggr_buf + blufi_env.offset, hdr->data,
//             hdr->data_len);

//             btc_blufi_protocol_handler(hdr->type, blufi_env.aggr_buf,
//             blufi_env.total_len); blufi_env.offset = 0;
//             osi_free(blufi_env.aggr_buf);
//             blufi_env.aggr_buf = NULL;
//         } else {
//             btc_blufi_protocol_handler(hdr->type, hdr->data, hdr->data_len);
//             blufi_env.offset = 0;
//         }
//     }
// }

// void btc_blufi_send_encap(uint8_t type, uint8_t *data, int total_data_len)
// {
//     struct blufi_hdr *hdr = NULL;
//     int remain_len = total_data_len;
//     uint16_t checksum;
//     int ret;

//     if (blufi_env.is_connected == false) {
//         BTC_TRACE_ERROR("blufi connection has been disconnected \n");
//         return;
//     }

//     while (remain_len > 0) {
//         if (remain_len > blufi_env.frag_size) {
//             hdr = osi_malloc(sizeof(struct blufi_hdr) + 2 +
//             blufi_env.frag_size + 2); if (hdr == NULL) {
//                 BTC_TRACE_ERROR("%s no mem\n", __func__);
//                 return;
//             }
//             hdr->fc = 0x0;
//             hdr->data_len = blufi_env.frag_size + 2;
//             hdr->data[0] = remain_len & 0xff;
//             hdr->data[1] = (remain_len >> 8) & 0xff;
//             memcpy(hdr->data + 2, &data[total_data_len - remain_len],
//             blufi_env.frag_size); //copy first, easy for check sum hdr->fc |=
//             BLUFI_FC_FRAG;
//         } else {
//             hdr = osi_malloc(sizeof(struct blufi_hdr) + remain_len + 2);
//             if (hdr == NULL) {
//                 BTC_TRACE_ERROR("%s no mem\n", __func__);
//                 return;
//             }
//             hdr->fc = 0x0;
//             hdr->data_len = remain_len;
//             memcpy(hdr->data, &data[total_data_len - remain_len],
//             hdr->data_len); //copy first, easy for check sum
//         }

//         hdr->type = type;
//         hdr->fc |= BLUFI_FC_DIR_E2P;
//         hdr->seq = blufi_env.send_seq++;

//         if (BLUFI_TYPE_IS_CTRL(hdr->type)) {
//             if ((blufi_env.sec_mode & BLUFI_CTRL_SEC_MODE_CHECK_MASK)
//                     && (blufi_env.cbs && blufi_env.cbs->checksum_func)) {
//                 hdr->fc |= BLUFI_FC_CHECK;
//                 checksum = blufi_env.cbs->checksum_func(hdr->seq, &hdr->seq,
//                 hdr->data_len + 2); memcpy(&hdr->data[hdr->data_len],
//                 &checksum, 2);
//             }
//         } else if (!BLUFI_TYPE_IS_DATA_NEG(hdr->type) &&
//         !BLUFI_TYPE_IS_DATA_ERROR_INFO(hdr->type)) {
//             if ((blufi_env.sec_mode & BLUFI_DATA_SEC_MODE_CHECK_MASK)
//                     && (blufi_env.cbs && blufi_env.cbs->checksum_func)) {
//                 hdr->fc |= BLUFI_FC_CHECK;
//                 checksum = blufi_env.cbs->checksum_func(hdr->seq, &hdr->seq,
//                 hdr->data_len + 2); memcpy(&hdr->data[hdr->data_len],
//                 &checksum, 2);
//             }

//             if ((blufi_env.sec_mode & BLUFI_DATA_SEC_MODE_ENC_MASK)
//                     && (blufi_env.cbs && blufi_env.cbs->encrypt_func)) {
//                 ret = blufi_env.cbs->encrypt_func(hdr->seq, hdr->data,
//                 hdr->data_len); if (ret == hdr->data_len) { /* enc must be
//                 success and enc len must equal to plain len */
//                     hdr->fc |= BLUFI_FC_ENC;
//                 } else {
//                     BTC_TRACE_ERROR("%s encrypt error %d\n", __func__, ret);
//                     btc_blufi_report_error(ESP_BLUFI_ENCRYPT_ERROR);
//                     osi_free(hdr);
//                     return;
//                 }
//             }
//         }

//         if (hdr->fc & BLUFI_FC_FRAG) {
//             remain_len -= (hdr->data_len - 2);
//         } else {
//             remain_len -= hdr->data_len;
//         }

//        esp_blufi_send_encap(hdr);

//         osi_free(hdr);
//         hdr =  NULL;
//     }
// }
