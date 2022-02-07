#ifndef AIR_REF_H_
#define AIR_REF_H_


#include "general.h"
#include "validator_field.h"


typedef enum {
	control_type_manual_off =0,
	control_type_manual_on,
	control_type_digital_input,
	control_type_pump_down,
	control_type_size,
}control_type_t;

typedef enum {
	error_sonda_none=0,
	error_sonda_scollegata,
	error_sonda_corto
}error_sonda_t;

typedef enum {
	error_level_none=0,
	error_level_warning,
	error_level_critical
} error_level_t;

typedef enum {
	air_ref_status_timeout=0,
	air_ref_status_idle,
	air_ref_status_run_start,
	air_ref_status_run_full,
	air_ref_status_run_defrost,
	air_ref_status_spegnimento_pumpdown,
	air_ref_status_critical_error
} air_ref_machine_states_t;

typedef enum {
	compressor_status_blocked=0,
	compressor_status_idle,
	compressor_status_run,
	compressor_status_run_acceleration,
} compressor_machine_states_t;

typedef enum {
	valvola_gas_scarico_status_open=0,
	valvola_gas_scarico_status_close,
} valvola_gas_scarico_machine_states_t;

typedef enum {
	fan_status_enabled=0,
	fan_status_disabled,
} fan_machine_states_t;


typedef enum {
	termostatica_full_close=0,
	termostatica_modulating,
	termostatica_starting,
	termostatica_fixed_position
} termostatica_machine_states_t;

typedef enum {
	air_ref_gas_R32=0,
	air_ref_gas_R134a,
	air_ref_gas_R507,
	air_ref_gas_max
}air_ref_gas_type_t;

#define FOREACH_AR_CONF(OPERATION)					\
OPERATION(operator_password)						\
OPERATION(control_type)								\
OPERATION(gas_type)									\
OPERATION(air_ref_start_interval)					\
OPERATION(air_ref_timeout_interval)					\
OPERATION(air_ref_pump_down_pressure_control)		\
OPERATION(termostatica_surriscaldo_setpoint)		\
OPERATION(termostatica_coeff_P)						\
OPERATION(termostatica_coeff_I)						\
OPERATION(termostatica_coeff_I_max)					\
OPERATION(termostatica_max_step)					\
OPERATION(termostatica_interval_fixed_poistion)		\
OPERATION(termostatica_initial_fixed_position)		\
OPERATION(fan_target_pressure)						\
OPERATION(fan_coeff_P)								\
OPERATION(fan_coeff_offset)							\
OPERATION(fan_min_pressure)							\
OPERATION(fan_max_pressure)							\
OPERATION(compressor_speed_full)					\
OPERATION(compressor_start_speed)					\
OPERATION(compressor_defrost_speed)					\
OPERATION(compressor_pressure_spike)				\
OPERATION(valvola_gas_scarico_open_temperature)		\
OPERATION(valvola_gas_scarico_close_temperature)	\
OPERATION(period_log)								\
OPERATION(LP_low_pressure_limit)					\
OPERATION(LP_low_pressure_recover)


#define FOREACH_M_CONF(OPERATION)			\
OPERATION(can_address)						\
OPERATION(evap_pres_V1)						\
OPERATION(evap_pres_V2)						\
OPERATION(evap_pres_P1)						\
OPERATION(evap_pres_P2)						\
OPERATION(cond_pres_V1)						\
OPERATION(cond_pres_V2)						\
OPERATION(cond_pres_P1)						\
OPERATION(cond_pres_P2)						\
OPERATION(temperature_gas_scarico_res25)	\
OPERATION(temperature_gas_scarico_beta)		\
OPERATION(temperature_gas_scarico_offset)	\
OPERATION(temperature_environment_res25)	\
OPERATION(temperature_environment_beta)		\
OPERATION(temperature_environment_offset)	\
OPERATION(temperature_gas_ritorno_res25)	\
OPERATION(temperature_gas_ritorno_beta)		\
OPERATION(temperature_gas_ritorno_offset)	\
OPERATION(temperature_extra_res25)			\
OPERATION(temperature_extra_beta)			\
OPERATION(temperature_extra_offset)				




#define FOREACH_AR_STATUS(OPERATION)                \
OPERATION(air_ref_start_timestamp)		        	\
OPERATION(air_ref_status)			                \
OPERATION(compressor_status)		            	\
OPERATION(termostatica_status)			            \
OPERATION(fan_status)			                    \
OPERATION(compressor_I_value)			            \
OPERATION(compressor_speed_to_command)			    \
OPERATION(compressor_calculated_speed)			    \
OPERATION(fan_speed_to_command)			            \
OPERATION(fan_time_last_command)			        \
OPERATION(termostatica_I_value)			            \
OPERATION(termostatica_step_target)		        	\
OPERATION(termostatica_step_current_position)		\
OPERATION(termostatica_timestamp_initial_position)  \
OPERATION(debounce_input_timestamp)			        \
OPERATION(debounce_input_current_state)		    	\





#define FOREACH_M_STATUS(OPERATION)                \
OPERATION(device_type)		        	\
OPERATION(evaporation_pressure)		        	\
OPERATION(evaporation_temperature)		        	\
OPERATION(condensation_pressure)		        	\
OPERATION(temperature_gas_scarico)		        	\
OPERATION(temperature_environment)		        	\
OPERATION(temperature_gas_ritorno)		        	\
OPERATION(temperature_extra)		        	\
OPERATION(imc102_status_INDEX_MOTOR_FAULT)		        	\
OPERATION(imc102_status_INDEX_MOTOR_SPEED)		        	\
OPERATION(imc102_status_INDEX_MOTOR_MOTOR_CURRENT)		        	\
OPERATION(imc102_status_INDEX_MOTOR_TARGET_SPEED)		        	\
OPERATION(imc102_status_routine)		        	\
OPERATION(imm101_status_INDEX_MOTOR_FAULT)		        	\
OPERATION(imm101_status_INDEX_MOTOR_SPEED)		        	\
OPERATION(imm101_status_INDEX_MOTOR_MOTOR_CURRENT)		        	\
OPERATION(imm101_status_INDEX_MOTOR_TARGET_SPEED)		        	\
OPERATION(imm101_status_routine)		        	\
OPERATION(pin_enable)		        	\
OPERATION(index_error_pressure_evap)		        	\
OPERATION(index_error_pressure_cond)		        	\
OPERATION(index_error_temperature_gas_scarico)		        	\
OPERATION(index_error_temperature_environment)		        	\
OPERATION(index_error_temperature_gas_ritorno)		        	\
OPERATION(index_error_temperature_extra)		        	\
OPERATION(index_error_imm101_motor)		        	\
OPERATION(index_error_imc102_motor)		        	\
OPERATION(index_error_overtemperature_gas_scarico)		        	\
OPERATION(index_low_pressure_protection)		        	\




typedef enum{//public
    FOREACH_M_STATUS(ENUMERATE)
	machine_status_parameters_size
}machine_status_params_t;



typedef enum { //public
	FOREACH_AR_CONF(ENUMERATE)
	air_ref_conf_parameters_size
}air_ref_conf_parameters_t;


typedef enum{
	FOREACH_M_CONF(ENUMERATE)
	machine_conf_parameters_size,
}machine_conf_t;


typedef enum{
	FOREACH_AR_STATUS(ENUMERATE)
	air_ref_status_parameters_size,
}air_ref_status_params_t;



#endif /* AIR_REF_H_ */