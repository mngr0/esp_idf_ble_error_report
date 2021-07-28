#ifndef SERIAL_LOGGER_HW_H_
#define SERIAL_LOGGER_HW_H_

#include <string.h>
#include "air_ref/air_ref.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "driver/uart.h"


#define LOGGER_TAG "LOGGER"

#define LOGGER_BUF_SIZE 1024
#define LOGGER_BUFFER_SIZE 348

#define LOGGER_TASK_STACK_SIZE  (16384 / sizeof(portSTACK_TYPE))
//#define TASK_PERIODIC_LOG_STACK_PRIORITY (tskIDLE_PRIORITY + 1)
#define LOGGER_SUM_MOD(a,b) ((a+LOGGER_BUFFER_SIZE+b)%LOGGER_BUFFER_SIZE)

#define LOGGER_I(i) LOGGER_SUM_MOD(i,io_logger.cnt_in_begin)

typedef struct {
	volatile uint8_t buffer_in[LOGGER_BUFFER_SIZE];
	struct io_descriptor * io;
	uint16_t cnt_in_begin;
	volatile uint16_t cnt_in_end;
} logger_device_t;

void logger_init();

void send_buffer(uint8_t* data, int lenght);



void routine_send_state(machine_state_t* m_state);

#endif