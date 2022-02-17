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

#include "ble/services/gatt_handle.h"

#include "cJSON.h"

#define AIR_REF_TAG "AIR_REF_TAG"

packet_ringbuffer_t packet_structure;

TaskHandle_t xQueryTask;
// TaskHandle_t xBLETask;


extern const uart_port_t uart_num;

int32_t ar_config[256];
int32_t m_config[256];
int32_t ar_status[256];
int32_t m_status[256];

static uint8_t ar_config_size;
static uint8_t m_config_size;
static uint8_t ar_status_size;
static uint8_t m_status_size;

logger_memory_t logger_memory;

extern char *memory_machine_conf;

static char *ar_config_names[air_ref_conf_parameters_size] = {
    FOREACH_AR_CONF(STRINGIFY)};

static char *m_config_names[machine_conf_parameters_size] = {
    FOREACH_M_CONF(STRINGIFY)};

static char *ar_status_names[air_ref_status_parameters_size] = {
    FOREACH_AR_STATUS(STRINGIFY)};

static char *m_status_names[machine_status_parameters_size] = {
    FOREACH_M_STATUS(STRINGIFY)};

bool send_request(logger_memory_t *logger_memory) {
  if (logger_memory->current_idx_read < logger_memory->current_size) {
    message_t query_msg;
    query_msg.command_type = logger_memory->current_command_type;
    query_msg.device_address = 4;
    query_msg.value = 0;
    query_msg.parameter_address = logger_memory->current_idx_read;
    logger_memory->last_send_timestamp = xTaskGetTickCount();
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
    logger_memory->last_send_timestamp = xTaskGetTickCount();
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

  if (xTaskGetTickCount() - logger_memory->last_send_timestamp >
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

void write_nextQUELLOVERO(logger_memory_t *logger_memory) {
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

  if (xTaskGetTickCount() - logger_memory->last_send_timestamp >
      (1000 / portTICK_RATE_MS)) { // 1 secondo di
                                   // timeout
                                   // resend
    send_write_request(logger_memory);
  }
}

bool write_next(logger_memory_t *logger_memory) {
  if (logger_memory->current_idx_read < logger_memory->current_size) {
    // logger_memory->current_list[logger_memory->current_idx_read] = ciao;
    logger_memory->current_idx_read++;
    ciao++;
    return true;
  } else {
    return false;
  }
}

void go_state_next(logger_state_t new_state) {

  logger_memory.logger_state = new_state;
  logger_memory.logger_state = new_state;
  logger_memory.current_idx_read = 0;
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

    logger_memory.current_command_type = write_routine_conf_parameter;
    logger_memory.current_expected_reply_type = ack;
    logger_memory.current_list = ar_config;
    logger_memory.current_size = ar_config_size;

    break;
  }
  case logger_state_write_machine_conf: {

    logger_memory.current_command_type = write_machine_conf_parameter;
    logger_memory.current_expected_reply_type = ack;
    logger_memory.current_list = m_config;
    logger_memory.current_size = m_config_size;

    break;
  }
  default: {
    break;
  }
  }
}

static void query_task(void *arg) {

  char json_update[JSON_STRING_SIZE];

  ar_config_size = air_ref_conf_parameters_size;
  m_config_size = machine_conf_parameters_size;
  ar_status_size = air_ref_status_parameters_size;
  m_status_size = machine_status_parameters_size;

  start_query_machine_conf(&logger_memory);
  logger_memory.logger_state = logger_state_starting_poll_machine_conf;

  while (1) {

    switch (logger_memory.logger_state) {

    case logger_state_starting_poll_machine_conf: {
      query_next(&logger_memory);
      if (logger_memory.current_idx_read == logger_memory.current_size) {
        start_query_routine_conf(&logger_memory);
        logger_memory.logger_state = logger_state_starting_poll_routine_conf;
        ESP_LOGI("LOGGER", "poll machine conf done");
        jsonify_machine_conf(get_machine_handle_ptr()->config);
      }
      break;
    }

    case logger_state_starting_poll_routine_conf: {
      query_next(&logger_memory);
      if (logger_memory.current_idx_read == logger_memory.current_size) {
        start_query_machine_status(&logger_memory);
        logger_memory.logger_state = logger_state_poll_machine_status;
        ESP_LOGI("LOGGER", "poll routine conf done");
        jsonify_routine_conf(get_routine_handle_ptr()->config);
      }
      break;
    }

    case logger_state_poll_machine_status: {
      query_next(&logger_memory);
      if (logger_memory.current_idx_read == logger_memory.current_size) {
        start_query_routine_status(&logger_memory);
        jsonify_machine_status(json_update);
        ESP_LOGI("LOGGER", "poll machine status done");
        gatt_machine_send_status_update_to_client(json_update);
      }
      if (logger_memory.logger_state_next != -1) {
        go_state_next(logger_memory.logger_state_next);
        logger_memory.logger_state_next = -1;
      }
      break;
    }
    case logger_state_poll_routine_status: {
      query_next(&logger_memory);
      if (logger_memory.current_idx_read == logger_memory.current_size) {
        start_query_machine_status(&logger_memory);
        jsonify_routine_status(json_update);
        ESP_LOGI("LOGGER", "poll routine status done");
        gatt_routine_send_status_update_to_client(json_update);
      }
      if (logger_memory.logger_state_next != -1) {
        go_state_next(logger_memory.logger_state_next);
        logger_memory.logger_state_next = -1;
      }
      break;
    }

    case logger_state_read_routine_conf: {
      if (query_next(&logger_memory)) {
        jsonify_command("read_routine_conf",
                        (logger_memory.current_idx_read * 100) /
                            logger_memory.current_size,
                        json_update);
        gatt_routine_send_logger_update_to_client(json_update);
      }
      if (logger_memory.current_idx_read == logger_memory.current_size) {
        print_array("air_ref_config:", ar_config, ar_config_names,
                    ar_config_size);
        start_query_machine_status(&logger_memory);
        jsonify_command("complete_read_routine_conf", 100, json_update);
        ESP_LOGI("LOGGER", "read routine conf done");
        gatt_routine_send_logger_update_to_client(json_update);
      }
      break;
    }

    case logger_state_read_machine_conf: {
      if (query_next(&logger_memory)) {
        jsonify_command("read_machine_conf",
                        (logger_memory.current_idx_read * 100) /
                            logger_memory.current_size,
                        json_update);
        gatt_machine_send_logger_update_to_client(json_update);
      }
      if (logger_memory.current_idx_read == logger_memory.current_size) {
        jsonify_machine_conf(get_machine_handle_ptr()->config);
        start_query_machine_status(&logger_memory);
        jsonify_command("complete_read_machine_conf", 100, json_update);
        ESP_LOGI("LOGGER", "read machine conf done");
        bool done;
        do {
          done = gatt_machine_send_logger_update_to_client(json_update);
          vTaskDelay(5 / portTICK_PERIOD_MS);
        } while (!done);
        do {
          done = gatt_machine_send_conf_update_to_client(json_update);
          vTaskDelay(5 / portTICK_PERIOD_MS);
        } while (!done);
      }

      break;
    }
    case logger_state_write_routine_conf: {
      write_next(&logger_memory);
      jsonify_command("write_routine_conf",
                      (logger_memory.current_idx_read * 100) /
                          logger_memory.current_size,
                      json_update);
      ESP_LOGI("HERE COMES THE JSON", "LEN:%u - %s", strlen(json_update),
               json_update);
      gatt_routine_send_logger_update_to_client(json_update);
      if (logger_memory.current_idx_read == logger_memory.current_size) {
        start_query_machine_status(&logger_memory);
        jsonify_command("complete_write_routine_conf", 100, json_update);
        ESP_LOGI("HERE COMES THE JSON", "LEN:%u - %s", strlen(json_update),
                 json_update);
        bool done;
        do {
          done = gatt_routine_send_logger_update_to_client(json_update);
          vTaskDelay(5 / portTICK_PERIOD_MS);
        } while (!done);
      }
      break;
    }
    case logger_state_write_machine_conf: {
      write_next(&logger_memory);
      jsonify_command("write_machine_conf",
                      (logger_memory.current_idx_read * 100) /
                          logger_memory.current_size,
                      json_update);
      ESP_LOGI("HERE COMES THE JSON", "LEN:%u - %s", strlen(json_update),
               json_update);
      if (logger_memory.current_idx_read == logger_memory.current_size) {
        start_query_machine_status(&logger_memory);
        jsonify_command("complete_write_machine_conf", 100, json_update);
        ESP_LOGI("HERE COMES THE JSON", "LEN:%u - %s", strlen(json_update),
                 json_update);
        bool done;
        do {
          done = gatt_machine_send_logger_update_to_client(json_update);
          vTaskDelay(5 / portTICK_PERIOD_MS);
        } while (!done);
      }
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
  if (logger_memory.logger_state_next == -1) {
    logger_memory.logger_state_next = new_state;
  }
  return;
}

void logger_init() {
  packet_manager_init(&packet_structure);
  xTaskCreate(query_task, "query_task", SENDER_TASK_STACK_SIZE, NULL,
              TASK_SENDER_STACK_PRIORITY, &(xQueryTask));
}
