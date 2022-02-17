#ifndef LOGGER_H_
#define LOGGER_H_

#include <stdint.h>
#include "serial_SATA_protocol/serial_SATA_protocol.h"

#define JSON_STRING_SIZE 1024


#define RECEIVER_TASK_STACK_SIZE (32800 / sizeof(portSTACK_TYPE))
#define TASK_RECEIVCER_STACK_PRIORITY (tskIDLE_PRIORITY + 1)

#define SENDER_TASK_STACK_SIZE (32800 / sizeof(portSTACK_TYPE))
#define TASK_SENDER_STACK_PRIORITY (tskIDLE_PRIORITY + 1)


typedef enum {
    logger_state_starting_poll_routine_conf=0,
    logger_state_starting_poll_machine_conf,
    logger_state_poll_machine_status,
    logger_state_poll_routine_status,
    logger_state_read_routine_conf,
    logger_state_read_machine_conf,
    // logger_state_write_routine_conf,
    // logger_state_write_machine_conf
}logger_state_t;

typedef struct {
    logger_state_t logger_state;
    logger_state_t logger_state_next;
    uint16_t current_idx_read;
    command_type_t current_command_type;
    command_type_t current_expected_reply_type;
    uint32_t last_send_timestamp;//TODO check typoe
    int32_t* current_list;
    uint8_t current_size;
} logger_memory_t;


void logger_init() ;
void logger_set_state(logger_state_t new_state);

void print_array(const char *title, int32_t *values, char **names,
                 uint8_t size);

void jsonify(char **names, int32_t *values, uint8_t size, char *output);

void start_query_logger_status(logger_memory_t *logger, logger_state_t state,
                               command_type_t cmd, command_type_t reply,
                               int32_t *current_list, uint8_t current_size);
#define jsonify_machine_status(output)                                         \
  jsonify(m_status_names, m_status, machine_status_parameters_size, output)

#define jsonify_machine_conf(output)                                           \
  jsonify(m_config_names, m_config, machine_conf_parameters_size, output)

#define jsonify_routine_status(output)                                         \
  jsonify(ar_status_names, ar_status, air_ref_status_parameters_size, output)

#define jsonify_routine_conf(output)                                           \
  jsonify(ar_config_names, ar_config, air_ref_conf_parameters_size, output)


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

void jsonify_command(char *status, int32_t advancement, char *output);

void enqueue_cmd(char* name, int32_t value);

#endif