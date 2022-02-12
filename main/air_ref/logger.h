#ifndef LOGGER_H_
#define LOGGER_H_

#include <stdint.h>
#include "serial_SATA_protocol/serial_SATA_protocol.h"

typedef enum {
    logger_state_starting_poll_routine_conf=0,
    logger_state_starting_poll_machine_conf,
    logger_state_poll_machine_status,
    logger_state_poll_routine_status,
    logger_state_read_routine_conf,
    logger_state_read_machine_conf,
    logger_state_write_routine_conf,
    logger_state_write_machine_conf
}logger_state_t;

typedef struct {
    logger_state_t logger_state;
    uint16_t current_idx_read;
    command_type_t current_command_type;
    command_type_t current_expected_reply_type;
    uint32_t last_send_timestamp;//TODO check typoe
    int32_t* current_list;
    uint8_t current_size;
} logger_memory_t;


void logger_init() ;
void logger_set_state(logger_state_t new_state);

#endif