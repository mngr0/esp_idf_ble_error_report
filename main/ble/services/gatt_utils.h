
static uint8_t find_char_and_desr_index(uint16_t handle) {
  uint8_t error = 0xff;

  for (int i = 0; i < GATT_MACHINE_NB; i++) {
    if (handle == machine_handle_table[i]) {
      return i;
    }
  }
  return error;
}

bool gatt_machine_send_status_update_to_client(char *json_status) {
  if (ble_is_connected()) {
    if (current_len == 0) {
      ESP_LOGI(GATTS_MACHINE_TAG, " INDICATING ");
      strcpy((char *)tmp_buffer, json_status);
      current_idx = 0;
      current_len = strlen(json_status);
      current_size_sent = min(mtu_size - 5, strlen(json_status));
      esp_ble_gatts_send_indicate(
          heart_rate_profile_tab[PROFILE_MACHINE_IDX].gatts_if, spp_conn_id,
          machine_handle_table[GATT_MACHINE_IDX_STATUS_VALUE],
          current_size_sent, (uint8_t *)json_status, true);
      return true;
    } else {
      ESP_LOGI(GATTS_MACHINE_TAG, "NOTIFICATION DISCARTED BUSY %d",
               current_len);
      return false;
    }
  }
  ESP_LOGI(GATTS_MACHINE_TAG, "NOTIFICATION DISCARTED DISCONNECTED");
  return false;
}

bool gatt_machine_send_logger_update_to_client(char *json_status) {
  if (ble_is_connected()) {
    if (current_len == 0) {
      ESP_LOGI(GATTS_MACHINE_TAG, " INDICATING ");
      strcpy((char *)tmp_buffer, json_status);
      current_idx = 0;
      current_len = strlen(json_status);
      current_size_sent = min(mtu_size - 5, strlen(json_status));
      esp_ble_gatts_send_indicate(
          heart_rate_profile_tab[PROFILE_MACHINE_IDX].gatts_if, spp_conn_id,
          machine_handle_table[GATT_MACHINE_IDX_LOGGER_STATUS_VALUE],
          current_size_sent, (uint8_t *)json_status, true);
      return true;
    } else {
      ESP_LOGI(GATTS_MACHINE_TAG, "NOTIFICATION DISCARTED BUSY %d",
               current_len);
      return false;
    }
  }
  ESP_LOGI(GATTS_MACHINE_TAG, "NOTIFICATION DISCARTED DISCONNECTED");
  return false;
}
