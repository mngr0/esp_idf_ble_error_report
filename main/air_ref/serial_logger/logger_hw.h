#ifndef SERIAL_LOGGER_HW_H_
#define SERIAL_LOGGER_HW_H_

#include <string.h>
#include "air_ref/air_ref.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "driver/uart.h"


#define LOGGER_TAG "LOGGER"


#define LOGGER_TASK_STACK_SIZE  (16384 / sizeof(portSTACK_TYPE))
#define TASK_PERIODIC_LOG_STACK_PRIORITY (tskIDLE_PRIORITY + 1)



void logger_init();

void send_buffer(uint8_t* data, int lenght);

void routine_send_state(machine_state_t* m_state);

void routine_send_new_conf(air_ref_conf_t *ar_conf_new);

#endif