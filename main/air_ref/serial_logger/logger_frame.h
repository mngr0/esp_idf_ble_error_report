#ifndef SERIAL_LOGGER_FRAME_H_
#define SERIAL_LOGGER_FRAME_H_

#include "stdint.h"
#include "logger_frame.h"
#include "fb/airref_builder.h"
#include "fb/airref_reader.h"


#define PROTOCOL_VERSION 1

#define LOGGER_PACKET_SIZE 300

#

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

#define HEADER_SIZE 5

typedef enum
{
    upload_result_nothing_to_do = 0,
    upload_result_correct,
    upload_result_timeout
} upload_result_t;

#define FRAME_AS_REQUEST(f) \
	f->start_of_frame[0]=START_OF_REQUEST; \
	f->start_of_frame[1]=START_OF_REQUEST; \
	f->end_of_frame[0]=END_OF_REQUEST; \
	f->end_of_frame[1]=END_OF_REQUEST; \

#define FRAME_AS_REPLY(f) \
	f->start_of_frame[0]=START_OF_REPLY; \
	f->start_of_frame[1]=START_OF_REPLY; \
	f->end_of_frame[0]=END_OF_REPLY; \
	f->end_of_frame[1]=END_OF_REPLY; \





uint16_t logger_checksum(uint8_t *data, uint16_t length);

void parse_reply(logger_frame_t *reply, uint8_t *logger_buffer);

int8_t receive_reply(logger_frame_t *reply, uint8_t *data, int length);





void build_frame(logger_frame_t *frame, flatcc_builder_t* builder );

void send_frame(logger_frame_t *frame);



//ringbuffer management

// ringbuffer_enqueue (data, size)

// ringbuffer_dequeue (&frame)
//{ search for start_reply ... end_reply }

#endif