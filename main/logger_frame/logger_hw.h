#ifndef LOGGER_FRAME_HW_H_
#define LOGGER_FRAME_HW_H_


#include "stdint.h"




#define RECEIVER_TASK_STACK_SIZE  (32800 / sizeof(portSTACK_TYPE))
#define TASK_RECEIVCER_STACK_PRIORITY (tskIDLE_PRIORITY + 1)


#define SENDER_TASK_STACK_SIZE  (32800 / sizeof(portSTACK_TYPE))
#define TASK_SENDER_STACK_PRIORITY (tskIDLE_PRIORITY + 1)




void send_buffer(uint8_t* data, int lenght);



#endif