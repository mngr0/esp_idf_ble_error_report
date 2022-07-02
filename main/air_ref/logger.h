#ifndef LOGGER_H_
#define LOGGER_H_

#include <stdint.h>
#include "serial_SATA_protocol/serial_SATA_protocol.h"

#define RECEIVER_TASK_STACK_SIZE (32800 / sizeof(portSTACK_TYPE))
#define TASK_RECEIVCER_STACK_PRIORITY (tskIDLE_PRIORITY + 1)

#define SENDER_TASK_STACK_SIZE (32800 / sizeof(portSTACK_TYPE))
#define TASK_SENDER_STACK_PRIORITY (tskIDLE_PRIORITY + 1)



void print_array(const char *title, int32_t *values, char **names,
                 uint8_t size);

#endif