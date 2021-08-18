
#include "stdbool.h"
//#include "logger_hw.h"
#include "logger_frame.h"
#include "air_ref/air_ref.h"

extern machine_state_t m_state;
extern air_ref_state_t ar_state;
extern air_ref_conf_t ar_conf;
extern air_ref_conf_t ar_conf_old;

void load_ar_state(flatcc_builder_t *B, air_ref_state_t *ar_state)
{
	AirRef_AirRefState_start(B);
	AirRef_AirRefState_compressor_actual_speed_index_add(B, ar_state->compressor_actual_speed_index);
	AirRef_AirRefState_compressor_last_speed_change_time_add(B, ar_state->compressor_last_speed_change_time);
	AirRef_AirRefState_compressor_speed_to_command_add(B, ar_state->compressor_speed_to_command);
	AirRef_AirRefState_compressor_last_stop_add(B, ar_state->compressor_last_stop);
	AirRef_AirRefState_compressor_is_blocked_add(B, ar_state->compressor_is_blocked);
	AirRef_AirRefState_compressor_is_running_add(B, ar_state->compressor_is_running);
	AirRef_AirRefState_fan_speed_to_command_add(B, ar_state->fan_speed_to_command);
	AirRef_AirRefState_fan_time_last_command_add(B, ar_state->fan_time_last_command);

	AirRef_AirRefState_ref_t airRefState = AirRef_AirRefState_end(B);
	AirRef_Content_union_ref_t content = AirRef_Content_as_MachineState(airRefState);
	AirRef_Message_create_as_root(B, content);
}

void parse_ar_state(AirRef_AirRefState_table_t airRefState, air_ref_state_t *ar_state)
{
	ar_state->compressor_actual_speed_index = AirRef_AirRefState_compressor_actual_speed_index(airRefState);
	ar_state->compressor_last_speed_change_time = AirRef_AirRefState_compressor_last_speed_change_time(airRefState);
	ar_state->compressor_speed_to_command = AirRef_AirRefState_compressor_speed_to_command(airRefState);
	ar_state->compressor_last_stop = AirRef_AirRefState_compressor_last_stop(airRefState);
	ar_state->compressor_is_blocked = AirRef_AirRefState_compressor_is_blocked(airRefState);
	ar_state->compressor_is_running = AirRef_AirRefState_compressor_is_running(airRefState);
	ar_state->fan_speed_to_command = AirRef_AirRefState_fan_speed_to_command(airRefState);
	ar_state->fan_time_last_command = AirRef_AirRefState_fan_time_last_command(airRefState);
}

void load_ar_conf(flatcc_builder_t *B, air_ref_conf_t *ar_conf)
{
	AirRef_AirRefConf_start(B);

	AirRef_AirRefConf_serial_control_add(B, ar_conf->serial_control);
	AirRef_AirRefConf_fan_target_pressure_add(B, ar_conf->fan_target_pressure);
	AirRef_AirRefConf_fan_coeff_p_add(B, ar_conf->fan_coeff_P);
	AirRef_AirRefConf_fan_coeff_offset_add(B, ar_conf->fan_coeff_offset);
	AirRef_AirRefConf_fan_min_pressure_add(B, ar_conf->fan_min_pressure);
	AirRef_AirRefConf_fan_max_pressure_add(B, ar_conf->fan_max_pressure);
	AirRef_AirRefConf_compressor_target_pressure_add(B, ar_conf->compressor_target_pressure);
	AirRef_AirRefConf_compressor_activation_offset_add(B, ar_conf->compressor_activation_offset);
	AirRef_AirRefConf_compressor_action_delay_add(B, ar_conf->compressor_action_delay);
	AirRef_AirRefConf_compressor_start_interval_add(B, ar_conf->compressor_start_interval);

	AirRef_CompressorSpeed_t compressorSpeed;
	for (int i = 0; i < 10; i++)
	{
		compressorSpeed.speed[i] = ar_conf->compressor_speed[i];
	}

	AirRef_AirRefConf_compressor_speed_add(B, &compressorSpeed);
	AirRef_AirRefConf_ref_t airRefConf = AirRef_AirRefConf_end(B);
	AirRef_Content_union_ref_t content = AirRef_Content_as_AirRefConf(airRefConf);
	AirRef_Message_create_as_root(B, content);
}

void parse_ar_conf(AirRef_AirRefConf_table_t airRefConf, air_ref_conf_t *ar_conf)
{
	ar_conf->serial_control = AirRef_AirRefConf_serial_control(airRefConf);

	ar_conf->fan_target_pressure = AirRef_AirRefConf_fan_target_pressure(airRefConf);
	ar_conf->fan_coeff_P = AirRef_AirRefConf_fan_coeff_p(airRefConf);
	ar_conf->fan_coeff_offset = AirRef_AirRefConf_fan_coeff_offset(airRefConf);
	ar_conf->fan_min_pressure = AirRef_AirRefConf_fan_min_pressure(airRefConf);
	ar_conf->fan_max_pressure = AirRef_AirRefConf_fan_max_pressure(airRefConf);
	ar_conf->compressor_target_pressure = AirRef_AirRefConf_compressor_target_pressure(airRefConf);
	ar_conf->compressor_activation_offset = AirRef_AirRefConf_compressor_activation_offset(airRefConf);
	ar_conf->compressor_action_delay = AirRef_AirRefConf_compressor_action_delay(airRefConf);
	ar_conf->compressor_start_interval = AirRef_AirRefConf_compressor_start_interval(airRefConf);
	const AirRef_CompressorSpeed_t *compressorSpeed = AirRef_AirRefConf_compressor_speed(airRefConf);

	for (int i = 0; i < 10; i++)
	{
		ar_conf->compressor_speed[i] = compressorSpeed->speed[i];
	}
}



void load_m_state(flatcc_builder_t *B, machine_state_t *m_state)
{
	//reset builder?
	AirRef_ErrorReport_t imc102_communication;
	AirRef_ErrorReport_t imc102_motor;
	AirRef_MotorStatus_t imc102_status;

	imc102_communication.error_code = m_state->imc102_communication.error_code;
	imc102_communication.error_state = m_state->imc102_communication.error_state;

	imc102_motor.error_code = m_state->imc102_communication.error_code;
	imc102_motor.error_state = m_state->imc102_communication.error_state;

	for (int i = 0; i < INDEX_MOTOR_SIZE; i++)
	{
		imc102_status.state[i] = m_state->imc102_status[i];
	}

	AirRef_ErrorReport_t imm101_communication;
	AirRef_ErrorReport_t imm101_motor;
	AirRef_MotorStatus_t imm101_status;

	imm101_communication.error_code = m_state->imm101_communication.error_code;
	imm101_communication.error_state = m_state->imm101_communication.error_state;

	imm101_motor.error_code = m_state->imm101_communication.error_code;
	imm101_motor.error_state = m_state->imm101_communication.error_state;

	for (int i = 0; i < INDEX_MOTOR_SIZE; i++)
	{
		imm101_status.state[i] = m_state->imm101_status[i];
	}

	AirRef_MachineState_start(B);
	AirRef_MachineState_evaporation_pressure_add(B, m_state->evaporation_pressure);
	AirRef_MachineState_condensation_pressure_add(B, m_state->condensation_pressure);
	AirRef_MachineState_temperature_gas_scarico_add(B, m_state->temperature_gas_scarico);
	AirRef_MachineState_temperature_environment_add(B, m_state->temperature_environment);
	AirRef_MachineState_temperature_gas_ritorno_add(B, m_state->temperature_gas_ritorno);
	AirRef_MachineState_temperature_extra_add(B, m_state->temperature_extra);
	AirRef_MachineState_imc102_communication_add(B, &imc102_communication);
	AirRef_MachineState_imc102_motor_add(B, &imc102_motor);
	AirRef_MachineState_imc102_status_add(B, &imc102_status);
	AirRef_MachineState_imm101_communication_add(B, &imm101_communication);
	AirRef_MachineState_imm101_motor_add(B, &imm101_motor);
	AirRef_MachineState_imm101_status_add(B, &imm101_status);
	AirRef_MachineState_ref_t machineState = AirRef_MachineState_end(B);

	AirRef_Content_union_ref_t content = AirRef_Content_as_MachineState(machineState);

	AirRef_Message_create_as_root(B, content);
}

void parse_m_state(AirRef_MachineState_table_t machineState, machine_state_t *m_state)
{
	m_state->condensation_pressure = AirRef_MachineState_condensation_pressure(machineState);
	m_state->evaporation_pressure = AirRef_MachineState_evaporation_pressure(machineState);
	m_state->temperature_gas_scarico = AirRef_MachineState_temperature_gas_scarico(machineState);
	m_state->temperature_environment = AirRef_MachineState_temperature_environment(machineState);
	m_state->temperature_gas_ritorno = AirRef_MachineState_temperature_gas_ritorno(machineState);
	m_state->temperature_extra = AirRef_MachineState_temperature_extra(machineState);

	const AirRef_ErrorReport_t *imc102_communication = AirRef_MachineState_imc102_communication(machineState);
	const AirRef_ErrorReport_t *imc102_motor = AirRef_MachineState_imc102_motor(machineState);
	const AirRef_MotorStatus_t *imc102_status = AirRef_MachineState_imc102_status(machineState);

	const AirRef_ErrorReport_t *imm101_communication = AirRef_MachineState_imm101_communication(machineState);
	const AirRef_ErrorReport_t *imm101_motor = AirRef_MachineState_imm101_motor(machineState);
	const AirRef_MotorStatus_t *imm101_status = AirRef_MachineState_imm101_status(machineState);

	m_state->imm101_communication.error_code = imm101_communication->error_code;
	m_state->imm101_communication.error_state = imm101_communication->error_state;

	m_state->imm101_communication.error_code = imm101_motor->error_code;
	m_state->imm101_communication.error_state = imm101_motor->error_state;

	for (int i = 0; i < INDEX_MOTOR_SIZE; i++)
	{
		m_state->imm101_status[i] = imm101_status->state[i];
	}

	m_state->imc102_communication.error_code = imc102_communication->error_code;
	m_state->imc102_communication.error_state = imc102_communication->error_state;

	m_state->imc102_communication.error_code = imc102_motor->error_code;
	m_state->imc102_communication.error_state = imc102_motor->error_state;

	for (int i = 0; i < INDEX_MOTOR_SIZE; i++)
	{
		m_state->imc102_status[i] = imc102_status->state[i];
	}
}

void send_new_conf(air_ref_conf_t *ar_conf_new)
{


	flatcc_builder_t builder;
	logger_frame_t request;

	flatcc_builder_init(&builder);
	load_ar_conf(&builder, &ar_conf);

	build_frame(&request, &builder);
	send_frame(&request);

	flatcc_builder_clear(&builder);
}

void do_request_m_state()
{
	flatcc_builder_t builder;
	logger_frame_t request;

	flatcc_builder_init(&builder);

	AirRef_Request_start(&builder);
	AirRef_Request_request_type_add(&builder, AirRef_RequestType_RequestMachineState);
	AirRef_Request_ref_t ar_request = AirRef_Request_end(&builder);

	AirRef_Content_union_ref_t content = AirRef_Content_as_Request(ar_request);
	AirRef_Message_create_as_root(&builder, content);
	build_frame(&request, &builder);
	send_frame(&request);
	flatcc_builder_clear(&builder);
}

void do_reply_m_state()
{
	flatcc_builder_t builder;
	logger_frame_t request;

	flatcc_builder_init(&builder);
	load_m_state(&builder, &m_state);
	build_frame(&request, &builder);
	send_frame(&request);
	flatcc_builder_clear(&builder);
}

void do_request_ar_state()
{
	flatcc_builder_t builder;
	logger_frame_t request;

	flatcc_builder_init(&builder);

	AirRef_Request_start(&builder);
	AirRef_Request_request_type_add(&builder, AirRef_RequestType_RequestAirRefState);
	AirRef_Request_ref_t ar_request = AirRef_Request_end(&builder);

	AirRef_Content_union_ref_t content = AirRef_Content_as_Request(ar_request);
	AirRef_Message_create_as_root(&builder, content);
	build_frame(&request, &builder);
	send_frame(&request);
	flatcc_builder_clear(&builder);
}
void do_reply_ar_state()
{
	flatcc_builder_t builder;
	logger_frame_t request;

	flatcc_builder_init(&builder);
	load_ar_state(&builder, &ar_state);
	build_frame(&request, &builder);
	send_frame(&request);
	flatcc_builder_clear(&builder);
}

void do_request_ar_conf()
{
	flatcc_builder_t builder;
	logger_frame_t request;

	flatcc_builder_init(&builder);
	AirRef_Request_start(&builder);
	AirRef_Request_request_type_add(&builder, AirRef_RequestType_RequestAirRefConf);
	AirRef_Request_ref_t ar_request = AirRef_Request_end(&builder);
	AirRef_Content_union_ref_t content = AirRef_Content_as_Request(ar_request);
	AirRef_Message_create_as_root(&builder, content);

	build_frame(&request, &builder);
	send_frame(&request);
	flatcc_builder_clear(&builder);
}

void do_reply_ar_conf()
{
	flatcc_builder_t builder;
	logger_frame_t request;

	flatcc_builder_init(&builder);
	load_ar_conf(&builder, &ar_conf);
	build_frame(&request, &builder);
	send_frame(&request);
	flatcc_builder_clear(&builder);
}

