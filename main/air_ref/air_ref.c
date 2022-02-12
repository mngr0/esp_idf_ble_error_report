
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "serial_SATA_protocol/serial_SATA_protocol.h"

#include "air_ref.h"
#include "logger.h"
#include "packet_manager/packet_manager.h"

#include "driver/uart.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "i2c_devices/rtc/MCP7940/mcp7940.h"
#include "peripherals/i2c_devices.h"

#include "utility/utility.h"

#include "ble/services/gatt_machine.h"

#include "cJSON.h"

#define AIR_REF_TAG "AIR_REF_TAG"

#define RECEIVER_TASK_STACK_SIZE (32800 / sizeof(portSTACK_TYPE))
#define TASK_RECEIVCER_STACK_PRIORITY (tskIDLE_PRIORITY + 1)

#define SENDER_TASK_STACK_SIZE (32800 / sizeof(portSTACK_TYPE))
#define TASK_SENDER_STACK_PRIORITY (tskIDLE_PRIORITY + 1)

#define JSON_STRING_SIZE 1024

packet_ringbuffer_t packet_structure;

TaskHandle_t xQueryTask;
TaskHandle_t xReceiverTask;

uint32_t timestamp_last_update_m_state;
uint32_t timestamp_last_update_ar_state;
uint32_t timestamp_last_update_ar_conf;
extern const uart_port_t uart_num;

int32_t ar_config[256];
int32_t m_config[256];
int32_t ar_status[256];
int32_t m_status[256];

logger_memory_t logger_memory;
logger_memory_t logger_memory_tmp;

extern char* memory_machine_conf;

static char *ar_config_names[air_ref_conf_parameters_size] = {
    FOREACH_AR_CONF(STRINGIFY)};

static char *m_config_names[machine_conf_parameters_size] = {
    FOREACH_M_CONF(STRINGIFY)};

static char *ar_status_names[air_ref_status_parameters_size] = {
    FOREACH_AR_STATUS(STRINGIFY)};

static char *m_status_names[machine_status_parameters_size] = {
    FOREACH_M_STATUS(STRINGIFY)};

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

void jsonify(char **names, int32_t *values, uint8_t size, char *output) {
  cJSON *root;
  root = cJSON_CreateObject();
  for (uint8_t i = 0; i < size; i++) {
    cJSON_AddNumberToObject(root, names[i], values[i]);
  }
  cJSON_PrintPreallocated(root, output, JSON_STRING_SIZE,
                          false); // returns 1 if error
  cJSON_Delete(root);
}

#define jsonify_machine_status(output)                                         \
  jsonify(m_status_names, m_status, machine_status_parameters_size, output)

#define jsonify_machine_conf(output)                                         \
  jsonify(m_config_names, m_config, machine_conf_parameters_size, output)


#define jsonify_air_ref_status(output)                                         \
  jsonify(ar_status_names, ar_status, air_ref_status_parameters_size, output)

void query_array(int32_t *values, uint8_t size, command_type_t query_cmd,
                 command_type_t reply_cmd) {
  uint8_t data[1000 * 4];

  packet_received_t reply;
  int length;
  message_t query_msg;
  query_msg.command_type = query_cmd;
  query_msg.device_address = 4;
  query_msg.parameter_address = 0;
  int i = 0;
  while (i < size) {
    query_msg.parameter_address = i;
    packet_manager_send_data((uint8_t *)&query_msg, 8);
    // ESP_LOGI("QUERY - update", "[%d/%d]" , i, size);
    // ESP_LOG_BUFFER_HEX("BUFFER SENT", (uint8_t *)&query_msg, 8);
    bool read_done = false;
    int count = 0;
    while ((!read_done) && (count < 10)) {
      length = uart_read_bytes(uart_num, data, LOGGER_BUF_SIZE,
                               20 / portTICK_RATE_MS);
      if (length > 0) {
        packet_manager_put(&packet_structure, data, length);
      }

      if ((length = packet_manager_pop(&packet_structure, data)) > 0) {

        if (packet_is_valid(&reply, data, length)) {
          // ESP_LOG_BUFFER_HEX("BUFFER RECV", data, length);
          // TODO check that it is a reply
          values[i] = ((message_t *)reply.buffer)->value;
          i++;
          read_done = true;
          // ESP_LOGI("QUERY - count", "%d" , count);
        }
      }
      if (!read_done) {
        vTaskDelay(1 / portTICK_PERIOD_MS);
        count++;
      }
    }
  }
}

bool send_request(logger_memory_t *logger_memory) {
  if (logger_memory->current_idx_read < logger_memory->current_size) {
    message_t query_msg;
    query_msg.command_type = logger_memory->current_command_type;
    query_msg.device_address = 4;
    query_msg.value = 0;
    query_msg.parameter_address = logger_memory->current_idx_read;
    logger_memory->last_send_timestamp = 4242;
    packet_manager_send_data((uint8_t *)&query_msg, 8);
    return true;
  } else {
    return false;
  }
}

void send_write_request(logger_memory_t *logger_memory) {
  if (logger_memory->current_idx_read < logger_memory->current_size) {
    message_t query_msg;
    query_msg.command_type = logger_memory->current_command_type;
    query_msg.device_address = 4;
    query_msg.value =
        logger_memory->current_list[logger_memory->current_idx_read];
    query_msg.parameter_address = logger_memory->current_idx_read;
    logger_memory->last_send_timestamp = 4242;
    packet_manager_send_data((uint8_t *)&query_msg, 8);
  }
}

bool query_nextQUELLOVERO(logger_memory_t *logger_memory) {
  uint8_t data[1000 * 4];

  int length;
  length =
      uart_read_bytes(uart_num, data, LOGGER_BUF_SIZE, 20 / portTICK_RATE_MS);

  if (length > 0) {
    packet_manager_put(&packet_structure, data, length);
    if ((length = packet_manager_pop(&packet_structure, data)) > 0) {
      packet_received_t reply;
      if (packet_is_valid(&reply, data, length)) {
        // if ((message_t *)reply.buffer)->command_type ==
        // logger_memory->current_expected_reply_type;
        logger_memory->current_list[logger_memory->current_idx_read] =
            ((message_t *)reply.buffer)->value;
        logger_memory->current_idx_read++;
        return send_request(logger_memory);
      }
    }
  }

  // 4242 = get time tick
  if (4242 - logger_memory->last_send_timestamp >
      (1000 / portTICK_RATE_MS)) { // 1 secondo di
                                   // timeout
                                   // resend
    return send_request(logger_memory);
  }
  return false;
}
uint8_t ciao = 42;

bool query_next(logger_memory_t *logger_memory) {
  if (logger_memory->current_idx_read < logger_memory->current_size) {
    logger_memory->current_list[logger_memory->current_idx_read] = ciao;
    logger_memory->current_idx_read++;
    ciao++;
    return true;
  } else {
    return false;
  }
}

void write_next(logger_memory_t *logger_memory) {
  uint8_t data[1000 * 4];

  int length;
  length =
      uart_read_bytes(uart_num, data, LOGGER_BUF_SIZE, 20 / portTICK_RATE_MS);

  if (length > 0) {
    packet_manager_put(&packet_structure, data, length);
    if ((length = packet_manager_pop(&packet_structure, data)) > 0) {
      packet_received_t reply;
      if (packet_is_valid(&reply, data, length)) {
        // TODO REPLY CAN BE NACK
        // if ((message_t *)reply.buffer)->command_type ==
        // logger_memory->current_expected_reply_type;
        logger_memory->current_idx_read++;
        send_write_request(logger_memory);
      }
    }
  }

  // 4242 = get time tick
  if (4242 - logger_memory->last_send_timestamp >
      (1000 / portTICK_RATE_MS)) { // 1 secondo di
                                   // timeout
                                   // resend
    send_write_request(logger_memory);
  }
}

static uint8_t ar_config_size;
static uint8_t m_config_size;
static uint8_t ar_status_size;
static uint8_t m_status_size;

void start_query_logger_status(logger_memory_t *logger, logger_state_t state,
                               command_type_t cmd, command_type_t reply,
                               int32_t *current_list, uint8_t current_size) {
  logger_memory.current_idx_read = 0;
  logger_memory.current_command_type = cmd;
  logger_memory.current_expected_reply_type = reply;
  logger_memory.current_list = current_list;
  logger_memory.current_size = current_size;
  logger->logger_state = state;
}

#define start_query_machine_status(logger)                                     \
  start_query_logger_status(                                                   \
      logger, logger_state_poll_machine_status, read_machine_status_parameter, \
      reply_machine_status_parameter, m_status, m_status_size);

#define start_query_routine_status(logger)                                     \
  start_query_logger_status(                                                   \
      logger, logger_state_poll_routine_status, read_routine_status_parameter, \
      reply_routine_status_parameter, ar_status, ar_status_size);

#define start_query_routine_conf(logger)                                       \
  start_query_logger_status(                                                   \
      logger, logger_state_read_routine_conf, read_routine_conf_parameter,     \
      reply_routine_conf_parameter, ar_config, ar_config_size);

#define start_query_machine_conf(logger)                                       \
  start_query_logger_status(                                                   \
      logger, logger_state_read_machine_conf, read_machine_conf_parameter,     \
      reply_machine_conf_parameter, m_config, m_config_size);

static void query_task(void *arg) {

  char json_update[JSON_STRING_SIZE];

  // read device type

  ar_config_size = air_ref_conf_parameters_size;
  m_config_size = machine_conf_parameters_size;
  ar_status_size = air_ref_status_parameters_size;
  m_status_size = machine_status_parameters_size;

  // query_array(ar_config, ar_config_size, read_routine_conf_parameter,
  //             reply_routine_conf_parameter);
  // print_array("air_ref_config:", ar_config, ar_config_names,
  // ar_config_size);

  // query_array(m_config, m_config_size, read_machine_conf_parameter,
  //             reply_machine_conf_parameter);
  // print_array("machine_config:", m_config, m_config_names,
  // m_config_size); logger_memory.logger_state = logger_state_polling;

  start_query_machine_conf(&logger_memory);
  logger_memory.logger_state = logger_state_starting_poll_machine_conf;

  while (1) {
    // query_array(ar_status, ar_status_size,
    // read_routine_status_parameter,
    //             reply_routine_conf_parameter);
    // print_array("air_ref_status:", ar_status, ar_status_names,
    // ar_status_size);

    // // jsonify()
    // // gatt_air_ref_update_status();

    // query_array(m_status, m_status_size, read_machine_status_parameter,
    //             reply_machine_status_parameter);
    // print_array("machine_status:", m_status, m_status_names,
    // m_status_size); jsonify_machine_status(json_update);
    // gatt_machine_send_status_update_to_client(json_update);
    // // gatt_machine_update_status(json_update);

    switch (logger_memory.logger_state) {

    case logger_state_starting_poll_machine_conf: {
      query_next(&logger_memory);
      if (logger_memory.current_idx_read == logger_memory.current_size) {
        start_query_routine_conf(&logger_memory);
        logger_memory.logger_state = logger_state_starting_poll_routine_conf;

        print_array("machine_config:", m_config, m_config_names, m_config_size);
        jsonify_machine_conf(get_str_pnt_diocane());
      }
      break;
    }

    case logger_state_starting_poll_routine_conf: {
      query_next(&logger_memory);
      if (logger_memory.current_idx_read == logger_memory.current_size) {
        start_query_machine_status(&logger_memory);
        logger_memory.logger_state = logger_state_poll_machine_status;
        print_array("air_ref_config:", ar_config, ar_config_names,
                    ar_config_size);
      }
      break;
    }

    case logger_state_poll_machine_status: {
      query_next(&logger_memory);

      if (logger_memory.current_idx_read == logger_memory.current_size) {
        start_query_routine_status(&logger_memory);
        print_array("machine_status:", m_status, m_status_names, m_status_size);

        jsonify_machine_status(json_update);
        ESP_LOGI("HERE COMES THE JSON", "LEDN:%u - %s", strlen(json_update),
                 json_update);
        gatt_machine_send_status_update_to_client(json_update);
      }
      break;
    }
    case logger_state_poll_routine_status: {
      query_next(&logger_memory);
      if (logger_memory.current_idx_read == logger_memory.current_size) {
        start_query_machine_status(&logger_memory);
        print_array("air_ref_status:", ar_status, ar_status_names,
                    ar_status_size);
        jsonify_air_ref_status(json_update);
        ESP_LOGI("HERE COMES THE JSON", "%s", json_update);
        // gatt_machine_send_status_update_to_client(json_update);
      }

      break;
    }
    case logger_state_read_routine_conf: {
      if (query_next(&logger_memory)) {
        cJSON *root;
        root = cJSON_CreateObject();
        cJSON_AddStringToObject(root, "cmd", "read_routine_conf");
        cJSON_AddNumberToObject(root, "perc",
                                (logger_memory.current_idx_read * 100) /
                                    logger_memory.current_size);
        cJSON_PrintPreallocated(root, json_update, JSON_STRING_SIZE, false);
        cJSON_Delete(root);
        ESP_LOGI("HERE COMES THE JSON", "LEDN:%u - %s", strlen(json_update),
                 json_update);
        gatt_machine_send_logger_update_to_client(json_update);
      }
      if (logger_memory.current_idx_read == logger_memory.current_size) {
                print_array("air_ref_config:", ar_config, ar_config_names,
                    ar_config_size);
        start_query_machine_status(&logger_memory);
        cJSON *root;
        root = cJSON_CreateObject();
        cJSON_AddStringToObject(root, "cmd", "complete_read_routine_conf");
        cJSON_AddNumberToObject(root, "perc", 100);
        cJSON_PrintPreallocated(root, json_update, JSON_STRING_SIZE, false);
        cJSON_Delete(root);
        ESP_LOGI("HERE COMES THE JSON", "LEDN:%u - %s", strlen(json_update),
                 json_update);
        gatt_machine_send_logger_update_to_client(json_update);
      }
      // if changed...
      // jsonify
      // gatt_machine_send_logger_update_to_client(json_update);
      break;
    }
    case logger_state_read_machine_conf: {
      if (query_next(&logger_memory)) {
        cJSON *root;
        root = cJSON_CreateObject();
        cJSON_AddStringToObject(root, "cmd", "read_machine_conf");
        cJSON_AddNumberToObject(root, "perc",
                                (logger_memory.current_idx_read * 100) /
                                    logger_memory.current_size);
        cJSON_PrintPreallocated(root, json_update, JSON_STRING_SIZE, false);
        cJSON_Delete(root);
        ESP_LOGI("HERE COMES THE JSON", "LEN:%u - %s", strlen(json_update),
                 json_update);
        gatt_machine_send_logger_update_to_client(json_update);
      }
      if (logger_memory.current_idx_read == logger_memory.current_size) {
        jsonify_machine_conf(get_str_pnt_diocane());
         ESP_LOGI("HERE COMES THE JSON", "LEN:%u - %s", strlen(get_str_pnt_diocane()), get_str_pnt_diocane());
        print_array("machine_config:", m_config, m_config_names, m_config_size);
        start_query_machine_status(&logger_memory);
        cJSON *root;
        root = cJSON_CreateObject();
        cJSON_AddStringToObject(root, "cmd", "complete_read_machine_conf");
        cJSON_AddNumberToObject(root, "perc", 100);
        cJSON_PrintPreallocated(root, json_update, JSON_STRING_SIZE, false);
        cJSON_Delete(root);
        ESP_LOGI("HERE COMES THE JSON", "LEN:%u - %s", strlen(json_update),
                 json_update);
        gatt_machine_send_logger_update_to_client(json_update);
         

      }
      // if changed...
      // jsonify
      // gatt_machine_send_logger_update_to_client(json_update);
      break;
    }
    case logger_state_write_routine_conf: {
      // TODO
      break;
    }
    case logger_state_write_machine_conf: {
      // TODO
      break;
    }
    default: {
      break;
    }
    }
    // ESP_LOGI("QUERY - count", "here %d", logger_memory.logger_state);
    // gatt_machine_send_status_update_to_client("ciao");
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void logger_set_state(logger_state_t new_state) {
  if ((logger_memory.logger_state == logger_state_poll_routine_status) ||
      (logger_memory.logger_state == logger_state_poll_routine_status)) {

    logger_memory.logger_state = new_state;
    logger_memory_tmp.logger_state = new_state;
    logger_memory_tmp.current_idx_read = 0;
    switch (new_state) {
    case logger_state_read_routine_conf: {
      start_query_routine_conf(&logger_memory);
      break;
    }
    case logger_state_read_machine_conf: {
      start_query_machine_conf(&logger_memory);
      break;
    }
    case logger_state_write_routine_conf: {

      logger_memory_tmp.current_command_type = write_routine_conf_parameter;
      logger_memory_tmp.current_expected_reply_type = ack;
      logger_memory_tmp.current_list = ar_config;
      logger_memory_tmp.current_size = ar_config_size;

      break;
    }
    case logger_state_write_machine_conf: {

      logger_memory_tmp.current_command_type = write_machine_conf_parameter;
      logger_memory_tmp.current_expected_reply_type = ack;
      logger_memory_tmp.current_list = m_config;
      logger_memory_tmp.current_size = m_config_size;

      break;
    }
    default: {
      break;
    }
    }
  }
  else{
    ESP_LOGI("LOGGER SET STATE", "CHANGE REJECTED");
  }
}

void logger_init() {
  packet_manager_init(&packet_structure);
  xTaskCreate(query_task, "query_task", SENDER_TASK_STACK_SIZE, NULL,
              TASK_SENDER_STACK_PRIORITY, &(xQueryTask));
}
