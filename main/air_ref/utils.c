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









// void query_array(int32_t *values, uint8_t size, command_type_t query_cmd,
//                  command_type_t reply_cmd) {
//   uint8_t data[1000 * 4];

//   packet_received_t reply;
//   int length;
//   message_t query_msg;
//   query_msg.command_type = query_cmd;
//   query_msg.device_address = 4;
//   query_msg.parameter_address = 0;
//   int i = 0;
//   while (i < size) {
//     query_msg.parameter_address = i;
//     packet_manager_send_data((uint8_t *)&query_msg, 8);
//     // ESP_LOGI("QUERY - update", "[%d/%d]" , i, size);
//     // ESP_LOG_BUFFER_HEX("BUFFER SENT", (uint8_t *)&query_msg, 8);
//     bool read_done = false;
//     int count = 0;
//     while ((!read_done) && (count < 10)) {
//       length = uart_read_bytes(uart_num, data, LOGGER_BUF_SIZE,
//                                20 / portTICK_RATE_MS);
//       if (length > 0) {
//         packet_manager_put(&packet_structure, data, length);
//       }

//       if ((length = packet_manager_pop(&packet_structure, data)) > 0) {

//         if (packet_is_valid(&reply, data, length)) {
//           // ESP_LOG_BUFFER_HEX("BUFFER RECV", data, length);
//           // TODO check that it is a reply
//           values[i] = ((message_t *)reply.buffer)->value;
//           i++;
//           read_done = true;
//           // ESP_LOGI("QUERY - count", "%d" , count);
//         }
//       }
//       if (!read_done) {
//         vTaskDelay(1 / portTICK_PERIOD_MS);
//         count++;
//       }
//     }
//   }
// }
