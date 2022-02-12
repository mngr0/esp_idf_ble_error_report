#ifndef SERIAL_SATA_H_
#define SERIAL_SATA_H_

typedef enum{
	read_routine_conf_parameter=0,
	read_routine_status_parameter,
	read_machine_conf_parameter,
	read_machine_status_parameter,
	
	write_routine_conf_parameter,
	write_machine_conf_parameter,
	reply_routine_conf_parameter,
	reply_routine_status_parameter,
	reply_machine_conf_parameter,
	reply_machine_status_parameter,
	reboot,
	hello,
	ack,
	nack
}command_type_t;

typedef struct {
	uint16_t device_address:16;
	command_type_t command_type:8;
	uint8_t parameter_address:8;
	uint32_t value:32;
}message_t;


#endif