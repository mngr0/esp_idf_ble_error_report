#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "freertos/portmacro.h"
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
    // ESP_LOG_BUFFER_HEX("LOGGER DATA SENT", &query_msg, 8);
    packet_manager_send_data((uint8_t *)&query_msg, 8);
    return true;
  } else {
    return false;
  }
}

bool query_next(logger_memory_t *logger_memory) {
  uint8_t data[1000 * 4];

  send_request(logger_memory);
  int length;

  while (xTaskGetTickCount() - logger_memory->last_send_timestamp <
         (1000 / portTICK_RATE_MS)) {

    length =
        uart_read_bytes(uart_num, data, LOGGER_BUF_SIZE, 20 / portTICK_RATE_MS);

    if (length > 0) {
      packet_manager_put(&packet_structure, data, length);
      logger_memory->last_send_timestamp = xTaskGetTickCount();
      if ((length = packet_manager_pop(&packet_structure, data)) > 0) {
        packet_received_t reply;
        if (packet_is_valid(&reply, data, length)) { // TODO CHECK NACK???
          // if ((message_t *)reply.buffer)->command_type ==
          // logger_memory->current_expected_reply_type;
          logger_memory->current_list[logger_memory->current_idx_read] =
              ((message_t *)reply.buffer)->value;
          logger_memory->current_idx_read++;
          return true;
        }
      }
    }
  }
  ESP_LOGI("LOGGER", "TIMEOUT");
  return false;
}

bool query_nextQUELLOVECCHIO(logger_memory_t *logger_memory) {
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

bool query_nextQUELLOFINTO(logger_memory_t *logger_memory) {
  if (logger_memory->current_idx_read < logger_memory->current_size) {
    logger_memory->current_list[logger_memory->current_idx_read] = ciao;
    logger_memory->current_idx_read++;
    ciao++;
    return true;
  } else {
    return false;
  }
}

void send_command(uint8_t *buf) {
  bool done, sent;
  char json_update[1000];
  uint8_t data[1000 * 4];
  TickType_t timestamp;
  int length;

  message_t query_msg;
  query_msg.command_type = buf[4];
  query_msg.device_address = 4;
  query_msg.value = *(int32_t *)buf;
  query_msg.parameter_address = buf[5];
  //ESP_LOG_BUFFER_HEX("MESSAGE DATA SENT", &query_msg, 8);
  packet_manager_send_data((uint8_t *)&query_msg, 8);
  timestamp = xTaskGetTickCount();
  sent = false;
  // wait for reply
  while ((xTaskGetTickCount() - timestamp < (3000 / portTICK_RATE_MS)&&(!sent)) ) {
    length =
        uart_read_bytes(uart_num, data, LOGGER_BUF_SIZE, 20 / portTICK_RATE_MS);

    if (length > 0) {
      packet_manager_put(&packet_structure, data, length);
      timestamp = xTaskGetTickCount();
      if ((length = packet_manager_pop(&packet_structure, data)) > 0) {
        packet_received_t reply;
        if (packet_is_valid(&reply, data, length)) { // TODO CHECK NACK???
          //ESP_LOG_BUFFER_HEX("MESSAGE DATA RECVED", &reply.buffer, 8);
          sent = true;
        }
      }
    }
  }
  char** names;
  if(buf[4]==write_routine_conf_parameter){
    names=ar_config_names;
  }
  if(buf[4]==write_machine_conf_parameter){
    names=m_config_names;
  }
  if (sent) { // TODO translate index in name
    jsonify_report("written", names[buf[5]], json_update);
    ESP_LOGI("SATA COMM OK", "%s",json_update);
  } else {
    jsonify_report("refused", names[buf[5]], json_update);
    ESP_LOGI("SATA COMM NOK", "%s",json_update);
  }

  do {
    done = gatt_machine_send_logger_update_to_client(json_update);
  } while (!done);
}

inline static void status_read(char *json_update) {
  query_next(&logger_memory);
  if (logger_memory.current_idx_read == logger_memory.current_size) {

    if (logger_memory.logger_state == logger_state_poll_routine_status) {
      jsonify_routine_status(json_update);
      ESP_LOGI("JSON:", "%s", json_update);
      gatt_routine_send_status_update_to_client(json_update);
    }
    if (logger_memory.logger_state == logger_state_poll_machine_status) {
      jsonify_machine_status(json_update);
      ESP_LOGI("JSON:", "%s", json_update);
      gatt_machine_send_status_update_to_client(json_update);
    }

    if (logger_memory.logger_state_next == logger_state_read_machine_conf) {
      start_query_machine_conf(&logger_memory);
      logger_memory.logger_state_next = -1;
      return;
    }
    if (logger_memory.logger_state_next == logger_state_read_routine_conf) {
      start_query_routine_conf(&logger_memory);
      logger_memory.logger_state_next = -1;
      return;
    }

    if (logger_memory.logger_state == logger_state_poll_routine_status) {

      start_query_machine_status(&logger_memory);
      return;
    }
    if (logger_memory.logger_state == logger_state_poll_machine_status) {
      start_query_routine_status(&logger_memory);
      return;
    }
  }
}

static inline void conf_read(char *json_update) {
  if (query_next(&logger_memory)) {
    if (logger_memory.logger_state == logger_state_read_routine_conf) {
      jsonify_command("read_routine_conf",
                      (logger_memory.current_idx_read * 100) /
                          logger_memory.current_size,
                      json_update);
      gatt_routine_send_logger_update_to_client(json_update);
    }
    if (logger_memory.logger_state == logger_state_read_machine_conf) {
      jsonify_command("read_machine_conf",
                      (logger_memory.current_idx_read * 100) /
                          logger_memory.current_size,
                      json_update);
      gatt_machine_send_logger_update_to_client(json_update);
    }
  }
  if (logger_memory.current_idx_read == logger_memory.current_size) {
    bool done;
    if (logger_memory.logger_state == logger_state_read_routine_conf) {
      jsonify_routine_conf(json_update);
      do {
        done = gatt_routine_send_conf_update_to_client(json_update);
        vTaskDelay(5 / portTICK_PERIOD_MS);
      } while (!done);
      ESP_LOGI("LOGGER", "ROUTINE SEND CONF FATTO");
      start_query_machine_conf(&logger_memory);
      return;
    }
    if (logger_memory.logger_state == logger_state_read_machine_conf) {
      jsonify_machine_conf(json_update);
      do {
        done = gatt_machine_send_conf_update_to_client(json_update);
        vTaskDelay(5 / portTICK_PERIOD_MS);
      } while (!done);
      start_query_machine_status(&logger_memory);
      return;
    }
  }
}

static QueueHandle_t command_queue;

static void query_task(void *arg) {
  // 6 = data(4) + type (routine/machine)(1) + index (1)
  command_queue = xQueueCreate(64, 6);
  char json_update[JSON_STRING_SIZE];

  ar_config_size = air_ref_conf_parameters_size;
  m_config_size = machine_conf_parameters_size;
  ar_status_size = air_ref_status_parameters_size;
  m_status_size = machine_status_parameters_size;

  start_query_machine_conf(&logger_memory);
  logger_memory.logger_state = logger_state_starting_poll_machine_conf;

  while (1) {
    uint8_t tmp_buffer[6];
    if (xQueueReceive(command_queue, tmp_buffer, 0)) {
      ESP_LOGI("LOGGER", "sending CMD");
      send_command(tmp_buffer);
    }
    // if queue has values, write command nad reply

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
      status_read(json_update);
      break;
    }
    case logger_state_poll_routine_status: {
      status_read(json_update);
      break;
    }
    case logger_state_read_routine_conf: {
      ESP_LOGI("LOGGER", "polling routine conf");
      conf_read(json_update);
      break;
    }
    case logger_state_read_machine_conf: {
      ESP_LOGI("LOGGER", "polling machine conf");
      conf_read(json_update);
      break;
    }

    default: {
      break;
    }
    }
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
  logger_memory.logger_state_next = -1;
  packet_manager_init(&packet_structure);
  xTaskCreate(query_task, "query_task", SENDER_TASK_STACK_SIZE, NULL,
              TASK_SENDER_STACK_PRIORITY, &(xQueryTask));
}

void enqueue_cmd(char *name, int32_t value) {
  ESP_LOGI("ENQUEUE", "CMD: assign %d TO %s",value,name);
  uint8_t frame[6];
  bool found = false;
  frame[0] = ((uint8_t *)&value)[0];
  frame[1] = ((uint8_t *)&value)[1];
  frame[2] = ((uint8_t *)&value)[2];
  frame[3] = ((uint8_t *)&value)[3];
  for (int i = 0; i < (machine_conf_parameters_size) && (!found); i++) {
    if (strcmp(name, m_config_names[i]) == 0) {
      // ESP_LOGI("ENQUEUE", "FOUND in m_conf: %d:%s",i,m_config_names[i]);
      frame[4] = write_machine_conf_parameter;
      frame[5] = i;
      found = true;
    }
  }
  for (int i = 0; (i < air_ref_conf_parameters_size) && (!found); i++) {
    if (strcmp(name, ar_config_names[i]) == 0) {
      // ESP_LOGI("ENQUEUE", "FOUND in ar_conf: %d:%s",i,ar_config_names[i]);
      frame[4] = write_routine_conf_parameter;
      frame[5] = i;
      found = true;
    }
  }
  if (found) {

    xQueueSend(command_queue, frame, 10);
  }
}