#ifndef __AIR_REF_H__
#define __AIR_REF_H__
/*
* AIR_REF.h
*
* Created: 28/04/2021 13:50:53
*  Author: marco
*/


#ifndef AIR_REF_H_
#define AIR_REF_H_

#include "infineonlib.h"
#include "error.h"


#define COMPRESSOR_SPEED_N 3 //TODO in firmware configure 16 slots, partial use

typedef struct { //public
	int32_t serial_control;
	
	int32_t fan_target_pressure; //(milliBar)
	int32_t fan_coeff_P;
	int32_t fan_coeff_offset;
	int32_t fan_min_pressure; //(milliBar)
	int32_t fan_max_pressure; //(milliBar)
	
	int32_t compressor_target_pressure; //millibar
	int32_t compressor_activation_offset; //millbar
	int32_t compressor_action_delay; // milliseconds
	int32_t compressor_speed[COMPRESSOR_SPEED_N];
	int32_t compressor_start_interval;
	
	int32_t period_log;
	int32_t LP_low_pressure_limit;
		
}air_ref_conf_t;

typedef enum {
	error_sonda_none=0,
	error_sonda_scollegata,
	error_sonda_corto
}error_sonda_t;

typedef enum {
	status_idle=0,
	status_running,
	status_critical_error
} air_ref_status_t;

typedef enum {
	index_error_pressure_evap=0,
	index_error_pressure_cond,
	
	index_error_temperature_gas_scarico,
	index_error_temperature_environment,
	index_error_temperature_gas_ritorno,
	index_error_temperature_extra,
	index_error_imm101_motor,
	index_error_imm101_communication,
	index_error_imc102_motor,
	index_error_imc102_communication,
	
	index_error_overtemperature_gas_scarico,
	index_low_pressure_protection,
	index_error_size
} air_ref_index_error_t;


typedef struct{//READONLY
	int32_t compressor_actual_speed_index;
	int32_t compressor_last_speed_change_time;
	int32_t compressor_speed_to_command;
	int32_t compressor_last_stop;
	int32_t compressor_is_blocked;
	int32_t compressor_is_running;
	
	int32_t fan_speed_to_command;
	int32_t fan_time_last_command;
} air_ref_state_t;


typedef struct{//public
	int32_t evaporation_pressure;
	int32_t condensation_pressure;
	int32_t temperature_gas_scarico;
	int32_t temperature_environment;
	int32_t temperature_gas_ritorno;
	int32_t temperature_extra;

	error_report_t imc102_communication;
	error_report_t imc102_motor;
	int16_t imc102_status[INDEX_MOTOR_SIZE];
	
	error_report_t imm101_communication;
	error_report_t imm101_motor;
	int16_t imm101_status[INDEX_MOTOR_SIZE];
	
	int32_t pin_enable;
	error_report_t ar_error[index_error_size];
	
	air_ref_status_t ar_status;
}machine_state_t;


#endif /* AIR_REF_H_ */
#endif