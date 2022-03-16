#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include "esp_log.h"
#include "logger.h"
#include "cJSON.h"

#include "peripherals/i2c_devices.h"
#include "peripherals/spi_sd.h"

#include "i2c_ports/esp-idf/esp_idf_i2c_port.h"
#include "i2c_common/i2c_common.h"
#include "i2c_devices/rtc/MCP7940/mcp7940.h"

#include "utility/utility.h"

void print_array(const char *title, int32_t *values, char **names,
                 uint8_t size) {
  char out_string[3000] = "";
  strcat(out_string, title);
  strcat(out_string, "\n");
  for (int i = 0; i < size; i++) {
    char tmp_string[1000];
    sprintf(tmp_string, "\t\t%s:\t%d\n", names[i], values[i]);
    strcat(out_string, tmp_string);
  }
  ESP_LOGI("QUERY", "%s", out_string);
}

void save_json_to_sd(char* json_string){
   rtc_time_t pTime;
  mcp7940_get_time(rtc_driver,&pTime);
  log_received_messageHR(&pTime, json_string);
}


void jsonify(char **names, int32_t *values, uint8_t size, char *output) {
  cJSON *root;
  root = cJSON_CreateObject();
  for (uint8_t i = 0; i < size; i++) {
    cJSON_AddNumberToObject(root, names[i], values[i]);
  }
  cJSON_PrintPreallocated(root, output, JSON_STRING_SIZE,
                          false); // returns 1 if error
  cJSON_Delete(root);
  save_json_to_sd(output);


}


void start_query_logger_status(logger_memory_t *logger, logger_state_t state,
                               command_type_t cmd, command_type_t reply,
                               int32_t *current_list, uint8_t current_size) {
  logger->current_idx_read = 0;
  logger->current_command_type = cmd;
  logger->current_expected_reply_type = reply;
  logger->current_list = current_list;
  logger->current_size = current_size;
  logger->logger_state = state;
}








