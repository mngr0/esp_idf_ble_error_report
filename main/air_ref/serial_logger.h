
#ifndef SERIAL_LOGGER_H_
#define SERIAL_LOGGER_H_

#include "stdint.h"
#include "air_ref.h"

#define LOGGER_TASK_STACK_SIZE  (16384 / sizeof(portSTACK_TYPE))
//#define TASK_PERIODIC_LOG_STACK_PRIORITY (tskIDLE_PRIORITY + 1)

#define PROTOCOL_VERSION 1

#define LOGGER_PACKET_SIZE 300
#define LOGGER_BUFFER_SIZE 348

typedef enum{
	logger_idle=0,
	logger_receiving
} logger_status_t;

typedef struct {
	volatile uint8_t buffer_in[LOGGER_BUFFER_SIZE];
	struct io_descriptor * io;
	uint16_t cnt_in_begin;
	volatile uint16_t cnt_in_end;
} logger_device_t;

typedef struct {
	uint8_t start_of_frame[2];
	uint8_t protocol_version;
	uint16_t frame_size;
	uint8_t buffer[LOGGER_PACKET_SIZE];
	uint16_t checksum;
	uint8_t end_of_frame[2];
}logger_frame_t;

#define START_OF_REQUEST 0xa2
#define END_OF_REQUEST 0xdc
#define START_OF_REPLY 0x2a
#define END_OF_REPLY 0xcd

typedef enum
{
    upload_result_nothing_to_do = 0,
    upload_result_correct,
    upload_result_timeout
} upload_result_t;

#define FRAME_AS_REQUEST(f) \
	f->start_of_frame[0]=START_OF_REQUEST; \
	f->start_of_frame[0]=START_OF_REQUEST; \
	f->end_of_frame[0]=END_OF_REQUEST; \
	f->end_of_frame[0]=END_OF_REQUEST; \

#define FRAME_AS_REPLY(f) \
	f->start_of_frame[0]=START_OF_REPLY; \
	f->start_of_frame[0]=START_OF_REPLY; \
	f->end_of_frame[0]=END_OF_REPLY; \
	f->end_of_frame[0]=END_OF_REPLY; \


void logger_init();
void init_usart_logger();
void routine_send_state();

extern air_ref_conf_t ar_conf;
extern air_ref_state_t ar_state;
extern machine_state_t m_state;

#endif /* SERIAL_LOGGER_H_ */