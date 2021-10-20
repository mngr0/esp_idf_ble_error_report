#ifndef SERIAL_LOGGER_FRAME_H_
#define SERIAL_LOGGER_FRAME_H_

#include <stddef.h>
#include <stdint.h>

#define PROTOCOL_VERSION 1

#define FRAME_MAX_PACKET_SIZE 500 //max size of the frame in the message

#define LOGGER_BUF_SIZE 1024 // size of the phisical buffer 


#define LOGGER_SUM_MOD(a,b) ((a+LOGGER_BUF_SIZE+b)%LOGGER_BUF_SIZE)
#define LOGGER_I(i) LOGGER_SUM_MOD(i,io_logger.cnt_in_begin)

typedef struct {
	volatile uint8_t buffer_in[LOGGER_BUF_SIZE];
	uint16_t cnt_in_begin;
	volatile uint16_t cnt_in_end;
} logger_device_t;

typedef struct {
	uint8_t start_of_frame[3];
	uint8_t protocol_version;
	uint16_t frame_size;
	uint8_t buffer[FRAME_MAX_PACKET_SIZE];
	uint16_t checksum;
	//RECEIVE_TIMESTAMP
	uint8_t end_of_frame[3];

}logger_frame_t;
//RENAME IN PACKET_RECEIVED_T

#define START_OF_FRAME 0xa2
#define END_OF_FRAME 0xdc

#define DELIMITER_SIZE 3
#define HEADER_SIZE DELIMITER_SIZE+3//1 for protocol version2 for size
#define FOOTER_SIZE DELIMITER_SIZE+2 //2 for checksum

#define DELIMITER_FRAME(f) \
	f->start_of_frame[0]=START_OF_FRAME; \
	f->start_of_frame[1]=START_OF_FRAME; \
	f->start_of_frame[2]=START_OF_FRAME; \
	f->end_of_frame[0]=END_OF_FRAME; \
	f->end_of_frame[1]=END_OF_FRAME; \
	f->end_of_frame[2]=END_OF_FRAME; \

#define DELIMITER_PACKET(buffer,size) \
	buffer[0]=START_OF_FRAME; \
	buffer[1]=START_OF_FRAME; \
	buffer[2]=START_OF_FRAME; \
	buffer[HEADER_SIZE + size + 0]=END_OF_FRAME; \
	buffer[HEADER_SIZE + size + 1]=END_OF_FRAME; \
	buffer[HEADER_SIZE + size + 2]=END_OF_FRAME; \


int send_data(uint8_t *data, size_t size);

//uint16_t logger_checksum(uint8_t *data, uint16_t length);

//void parse_reply(logger_frame_t *reply, uint8_t *logger_buffer);

int8_t receive_frame(logger_frame_t *reply, uint8_t *data, int length);

void add_to_ringbuffer(uint8_t *new_buf, int16_t length);

int16_t take_frame(uint8_t *frame_buf);

#endif