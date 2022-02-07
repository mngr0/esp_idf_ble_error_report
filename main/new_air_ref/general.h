#ifndef GENERAL_H_
#define GENERAL_H_


typedef enum{
	device_type_termostatica =1,
	device_type_inverter,
	device_type_termostato,
	device_type_my_feeder
}device_type_t;


#define ENUMERATE(name) name,
#define STRINGIFY(name) #name,


#define CAN_ADDRESS_LOCATION 0
#define DEVICE_TYPE_LOCATION 0



#endif