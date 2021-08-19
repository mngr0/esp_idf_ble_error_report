#ifndef SERIAL_LOGGER_HW_H_
#define SERIAL_LOGGER_HW_H_

#include <string.h>
#include "air_ref/air_ref.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "driver/uart.h"


#define LOGGER_TAG "LOGGER"


#define RECEIVER_TASK_STACK_SIZE  (32800 / sizeof(portSTACK_TYPE))
#define TASK_RECEIVCER_STACK_PRIORITY (tskIDLE_PRIORITY + 1)


#define SENDER_TASK_STACK_SIZE  (32800 / sizeof(portSTACK_TYPE))
#define TASK_SENDER_STACK_PRIORITY (tskIDLE_PRIORITY + 1)

extern const uart_port_t uart_num;


void send_buffer(uint8_t* data, int lenght);



#endif