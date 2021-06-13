
#ifndef SERIAL_LOGGER_H_
#define SERIAL_LOGGER_H_

#include "stdint.h"
#include "air_ref.h"

#define PROTOCOL_VERSION 1

#define max(a,b) (a>b ? a :b)
#define LOGGER_PACKET_SIZE max(max(sizeof(air_ref_conf_t),sizeof(air_ref_state_t)),sizeof(machine_state_t))

#define LOGGER_BUFFER_SIZE 348
#define LOGGER_SUM_MOD(a,b) ((a+LOGGER_BUFFER_SIZE+b)%LOGGER_BUFFER_SIZE)

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

typedef enum{
	read_routine_conf=1,
	read_machine_state,
	read_routine_state,
	write_routine_conf,
	
}request_command_t;

typedef enum{
	command_done=0,
	unsupported_protocol,
	periodic_update
} reply_code_t;

typedef struct {
	uint8_t protocol_version;
	uint16_t frame_size;
	reply_code_t reply_code;
	uint8_t buffer[LOGGER_PACKET_SIZE];
	uint16_t checksum;
}logger_reply_t; //TX

typedef struct {
	uint8_t protocol_version;
	uint16_t frame_size;
	request_command_t request_code;
	uint8_t buffer[LOGGER_PACKET_SIZE];
	uint16_t checksum;
}logger_request_t; //RX

#define HEADER_SIZE 4//(sizeof(uint8_t)+sizeof(uint16_t)+sizeof(request_command_t))
#define CHECKSUM_SIZE (sizeof(uint16_t))
//#define LOGGER_I(i) LOGGER_SUM_MOD(i,io_logger.cnt_in_begin)


void parse_reply(logger_reply_t* reply, uint8_t* logger_buffer);

int8_t receive_reply(logger_reply_t* reply, uint8_t* data,int length);

#endif /* SERIAL_LOGGER_H_ */