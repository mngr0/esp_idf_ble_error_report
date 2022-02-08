#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "serial_SATA_protocol/serial_SATA_protocol.h"

#include "air_ref/air_ref.h"
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


static char *ar_config_names[air_ref_conf_parameters_size] = {
    FOREACH_AR_CONF(STRINGIFY)};

static char *m_config_names[machine_conf_parameters_size] = {
    FOREACH_M_CONF(STRINGIFY)};

static char *ar_status_names[air_ref_status_parameters_size] = {
    FOREACH_AR_STATUS(STRINGIFY)};

static char *m_status_names[machine_status_parameters_size] = {
    FOREACH_M_STATUS(STRINGIFY)};


void print_array(const char* title,int32_t *values, char **names, uint8_t size) {
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


void jsonify(char** names, int32_t *values, uint8_t size, char* output){
  for(uint8_t i=0; i< size; i++){
    //names[i] : values[i]
  }
  //stringify
}

#define jsonify_machine_status(output) jsonify(m_status_names,m_status,machine_status_parameters_size,output)


void query_array( int32_t *values, uint8_t size, command_type_t query_cmd,
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
    //ESP_LOGI("QUERY - update", "[%d/%d]" , i, size);
    //ESP_LOG_BUFFER_HEX("BUFFER SENT", (uint8_t *)&query_msg, 8);
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
          //ESP_LOG_BUFFER_HEX("BUFFER RECV", data, length);
          //TODO check that it is a reply
          values[i] = ((message_t *)reply.buffer)->value;
          i++;
          read_done = true;
          //ESP_LOGI("QUERY - count", "%d" , count);
        }
      }
      if (!read_done) {
        vTaskDelay(1 / portTICK_PERIOD_MS);
        count++;
      }
    }
  }
}


static void query_task(void *arg) {
  char json_update[2048];
  uint8_t ar_config_size;
uint8_t m_config_size;
uint8_t ar_status_size;
uint8_t m_status_size;

  // read device type
  ar_config_size = air_ref_conf_parameters_size;
  m_config_size = machine_conf_parameters_size;
  ar_status_size = air_ref_status_parameters_size;
  m_status_size = machine_status_parameters_size;

  query_array(ar_config, ar_config_size, read_routine_conf_parameter,
              reply_routine_conf_parameter);
  print_array("air_ref_config:",ar_config, ar_config_names, ar_config_size);



  query_array(m_config, m_config_size, read_machine_conf_parameter,
              reply_machine_conf_parameter);
  print_array("machine_config:",m_config, m_config_names, m_config_size);

  while (1) {
    query_array(ar_status, ar_status_size, read_routine_status_parameter,
                reply_routine_conf_parameter);
    print_array("air_ref_status:",ar_status, ar_status_names, ar_status_size);

    //jsonify()
    //gatt_air_ref_update_status();


    query_array(m_status, m_status_size, read_machine_status_parameter,
                reply_machine_status_parameter);
    print_array("machine_status:",m_status, m_status_names, m_status_size);
    jsonify_machine_status(json_update);
    //gatt_machine_update_status(json_update);

  }
}

void logger_init() {
  packet_manager_init(&packet_structure);
  xTaskCreate(query_task, "query_task", SENDER_TASK_STACK_SIZE, NULL,
              TASK_SENDER_STACK_PRIORITY, &(xQueryTask));
}
