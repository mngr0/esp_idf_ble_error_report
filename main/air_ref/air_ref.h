#ifndef __AIR_REF_H__
#define __AIR_REF_H__

#include "infineonlib.h"
#include "error.h"

typedef enum {
	control_type_manual_off =0,
	control_type_manual_on,
	control_type_auto,
}control_type_t;

typedef enum {
	error_sonda_none=0,
	error_sonda_scollegata,
	error_sonda_corto
}error_sonda_t;

typedef enum {
	air_ref_status_idle=0,
	air_ref_status_run_start,
	air_ref_status_run_full,
	air_ref_status_run_pre_sbrinamento,
	air_ref_status_run_sbrinamento,
	air_ref_status_run_post_sbrinamento,
	air_ref_status_critical_error
} air_ref_status_t;



typedef struct { //public
	control_type_t control_type;
	
	int32_t air_ref_start_interval;
	
	int32_t termostatica_surriscaldo_setpoint;
	int32_t termostatica_coeff_P;
	int32_t termostatica_coeff_I;
	int32_t termostatica_coeff_I_max;
	int32_t termostatica_max_step;
	
	int32_t fan_target_pressure; //(milliBar)
	int32_t fan_coeff_P;
	int32_t fan_coeff_offset;
	int32_t fan_min_pressure; //(milliBar)
	int32_t fan_max_pressure; //(milliBar)

	int32_t compressor_target_pressure; //millibar
	int32_t compressor_coeff_P;
	int32_t compressor_coeff_I;
	int32_t compressor_speed;
	int32_t compressor_start_speed;
	int32_t compressor_start_interval;
	int32_t compressor_pressure_spike;
	
	int32_t period_log;
	int32_t LP_low_pressure_limit;
	int32_t LP_low_pressure_recover;
	//HP_MOP
}air_ref_conf_t;

typedef struct{//READONLY

	int32_t air_ref_start_timestamp;
	air_ref_status_t air_ref_status;
	//int32_t compressor_last_speed_change_time;
	int32_t compressor_I_value;
	int32_t compressor_speed_to_command;
	int32_t compressor_calculated_speed;
	int32_t compressor_last_stop;
	int32_t compressor_is_blocked;
	int32_t compressor_is_running;

	int32_t fan_speed_to_command;
	int32_t fan_time_last_command;
	
	int32_t termostatica_I_value;
	int32_t termostatica_step_target;
	int32_t termostatica_step_current_position;
	
	int32_t debounce_input_timestamp;
	int32_t debounce_input_current_state;
} air_ref_state_t;



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


typedef struct{//public
	int32_t evaporation_pressure;
	int32_t evaporation_temperature;
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


int access_message_buffer(const void *buffer, int buff_len);

void logger_init();

void routine_send_state(machine_state_t* m_state);

void routine_send_new_conf(air_ref_conf_t *ar_conf_new);

void log_ar_conf( air_ref_conf_t *ar_conf);

void log_m_state( machine_state_t *m_state);


#endif /* AIR_REF_H_ */
