#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "serial_SATA_protocol/serial_SATA_protocol.h"

#include "new_air_ref/air_ref.h"
#include "packet_manager/packet_manager.h"

#include "driver/uart.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "i2c_devices/rtc/MCP7940/mcp7940.h"
#include "peripherals/i2c_devices.h"

#include "utility/utility.h"

#define AIR_REF_TAG "AIR_REF_TAG"

#define RECEIVER_TASK_STACK_SIZE (32800 / sizeof(portSTACK_TYPE))
#define TASK_RECEIVCER_STACK_PRIORITY (tskIDLE_PRIORITY + 1)

#define SENDER_TASK_STACK_SIZE (32800 / sizeof(portSTACK_TYPE))
#define TASK_SENDER_STACK_PRIORITY (tskIDLE_PRIORITY + 1)

packet_ringbuffer_t packet_structure;

// machine_state_t m_state;
// air_ref_conf_t ar_conf;
// air_ref_conf_t ar_conf_old;
// air_ref_state_t ar_state;

TaskHandle_t xQueryTask;
TaskHandle_t xReceiverTask;

uint32_t timestamp_last_update_m_state;
uint32_t timestamp_last_update_ar_state;
uint32_t timestamp_last_update_ar_conf;
extern const uart_port_t uart_num;

int access_message_buffer(const void *buffer, int buff_len) {

  // AirRef_Message_table_t message = AirRef_Message_as_root(buffer);

  // if (message != 0)
  // {

  //     if (AirRef_Message_content_type(message) ==
  //     AirRef_Content_MachineState)
  //     {
  //         //update local machine state
  //         ESP_LOGI("DECODED", "MachineState");
  //         AirRef_MachineState_table_t machineState =
  //         (AirRef_MachineState_table_t)AirRef_Message_content(message);

  //         parse_m_state(machineState, &m_state);
  //         log_m_state(&m_state);
  //     }
  //     else if (AirRef_Message_content_type(message) ==
  //     AirRef_Content_AirRefConf)
  //     {
  //         //update local airrefconf
  //         ESP_LOGI("DECODED", "AirRefConf");
  //         AirRef_AirRefConf_table_t airRefConf =
  //         (AirRef_AirRefConf_table_t)AirRef_Message_content(message);
  //         parse_ar_conf(airRefConf, &ar_conf);
  //         log_ar_conf(&ar_conf); //tmp_ar_conf
  //         conf_received = true;
  //     }
  //     else if (AirRef_Message_content_type(message) ==
  //     AirRef_Content_AirRefState)
  //     {
  //         //update local airrefstate
  //         ESP_LOGI("DECODED", "AirRefState");
  //         AirRef_AirRefState_table_t airRefState =
  //         (AirRef_AirRefState_table_t)AirRef_Message_content(message);
  //         parse_ar_state(airRefState, &ar_state);
  //         log_ar_state(&ar_state);
  //     }
  //     else if (AirRef_Message_content_type(message) ==
  //     AirRef_Content_Request)
  //     {
  //         AirRef_Request_table_t request =
  //         (AirRef_Request_table_t)AirRef_Message_content(message);

  //         if (AirRef_Request_request_type_get(request) ==
  //         AirRef_RequestType_AirRefConfReceived)
  //         {
  //             ESP_LOGI("DECODED", "RICEVUTA CONFERMA DI CONFIGURAZIONE");

  //             if (sending_new_conf)
  //             {
  //                 //TODO check that received conf is right) *is this
  //                 necessary?()
  //                 //use tmp_ar_conf
  //                 //compare tmp_ar_conf with ar_conf

  //                 //if equal
  //                 sending_new_conf = false;
  //                 //memcpy(&ar_conf_old,&ar_conf,sizeof(air_ref_conf_t));
  //             }
  //         }
  //         //if request is air_ref_conf_received -> reset state to idle
  //     }
  //     else{
  //         ESP_LOGI("DECODED", "UNRECOGNIZED");
  //     }
  // }
  return 0;
}

// void routine_send_new_conf(air_ref_conf_t *ar_conf_new)
// {
//     // ESP_LOGI("ROUTINE SEND NEW CONF", "mandando new conf\n");
//     // memcpy(&ar_conf_old, &ar_conf, sizeof(air_ref_conf_t));
//     // memcpy(&ar_conf, ar_conf_new, sizeof(air_ref_conf_t));
//     // sending_new_conf = true;
//     // //send_new_conf(ar_conf_new);
// }

int32_t ar_config[256];
int32_t m_config[256];
int32_t ar_status[256];
int32_t m_status[256];

uint8_t ar_config_size;
uint8_t m_config_size;
uint8_t ar_status_size;
uint8_t m_status_size;

void print_array(int32_t *values, char **names, uint8_t size) {
  char out_string[1000] = "";

  for (int i = 0; i < size; i++) {
    char tmp_string[1000];
    sprintf(tmp_string, "%s:%d\t", names[i], values[i]);
    strcat(out_string, tmp_string);
  }
  ESP_LOGI("QUERY", "%s", out_string);
}

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
    packet_manager_send_data( (uint8_t *)&query_msg, 8);
    ESP_LOG_BUFFER_HEX("BUFFER SENT", (uint8_t *)&query_msg, 8);
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
          ESP_LOG_BUFFER_HEX("BUFFER RECV", data, length);
          values[i] = ((message_t *)data)->value;
          i++;
          read_done = true;
        }
      }
      if (!read_done) {
        vTaskDelay(500 / portTICK_PERIOD_MS);
        count++;
      }
    }
  }
}

static char *ar_config_names[air_ref_conf_parameters_size] = {
    FOREACH_AR_CONF(STRINGIFY)};

static char *m_config_names[machine_conf_parameters_size] = {
    FOREACH_M_CONF(STRINGIFY)};

static char *ar_status_names[air_ref_status_parameters_size] = {
    FOREACH_AR_STATUS(STRINGIFY)};

static char *m_status_names[machine_status_parameters_size] = {
    FOREACH_M_STATUS(STRINGIFY)};

static void query_task(void *arg) {
  // read device type
  ar_config_size = air_ref_conf_parameters_size;
  m_config_size = machine_conf_parameters_size;
  ar_status_size = air_ref_status_parameters_size;
  m_status_size = machine_status_parameters_size;

  query_array(ar_config, ar_config_size, read_routine_conf_parameter,
              reply_routine_conf_parameter);
  print_array(ar_config, ar_config_names, ar_config_size);

  query_array(m_config, m_config_size, read_machine_conf_parameter,
              reply_machine_conf_parameter);
  print_array(m_config, m_config_names, m_config_size);

  while (1) {
    query_array(ar_status, ar_status_size, read_routine_conf_parameter,
                reply_routine_conf_parameter);
    print_array(ar_status, ar_status_names, ar_status_size);

    query_array(m_status, m_status_size, read_machine_conf_parameter,
                reply_machine_conf_parameter);
    print_array(m_status, m_status_names, m_status_size);
  }
}

void logger_init() {
  packet_manager_init(&packet_structure);
  xTaskCreate(query_task, "query_task", SENDER_TASK_STACK_SIZE, NULL,
              TASK_SENDER_STACK_PRIORITY, &(xQueryTask));
}
