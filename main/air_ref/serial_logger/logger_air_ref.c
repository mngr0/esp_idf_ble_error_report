
#include "air_ref/air_ref.h"
#include "airref_reader.h"
#include "packet_manager/packet_manager.h"
#include "logger_air_ref.h"


extern machine_state_t m_state;
extern air_ref_state_t ar_state;
extern air_ref_conf_t ar_conf;
extern air_ref_conf_t ar_conf_old;

void load_ar_state(flatcc_builder_t *B, air_ref_state_t *ar_state) {
  AirRef_AirRefState_start(B);
  AirRef_AirRefState_compressor_speed_to_command_add(
      B, ar_state->compressor_speed_to_command);
  AirRef_AirRefState_compressor_calculated_speed_add(
      B, ar_state->compressor_calculated_speed);
  AirRef_AirRefState_compressor_last_stop_add(B,
                                              ar_state->compressor_last_stop);
  AirRef_AirRefState_compressor_is_blocked_add(B,
                                               ar_state->compressor_is_blocked);
  AirRef_AirRefState_compressor_is_running_add(B,
                                               ar_state->compressor_is_running);
  AirRef_AirRefState_fan_speed_to_command_add(B,
                                              ar_state->fan_speed_to_command);
  AirRef_AirRefState_fan_time_last_command_add(B,
                                               ar_state->fan_time_last_command);

  AirRef_AirRefState_ref_t airRefState = AirRef_AirRefState_end(B);
  AirRef_Content_union_ref_t content =
      AirRef_Content_as_MachineState(airRefState);
  AirRef_Message_create_as_root(B, content);
}

void parse_ar_state(AirRef_AirRefState_table_t airRefState,
                    air_ref_state_t *ar_state) {
  ar_state->compressor_speed_to_command =
      AirRef_AirRefState_compressor_speed_to_command(airRefState);
  ar_state->compressor_calculated_speed =
      AirRef_AirRefState_compressor_calculated_speed(airRefState);
  ar_state->compressor_last_stop =
      AirRef_AirRefState_compressor_last_stop(airRefState);
  ar_state->compressor_is_blocked =
      AirRef_AirRefState_compressor_is_blocked(airRefState);
  ar_state->compressor_is_running =
      AirRef_AirRefState_compressor_is_running(airRefState);
  ar_state->compressor_I_value =
      AirRef_AirRefState_compressor_I_value(airRefState);
  ar_state->fan_speed_to_command =
      AirRef_AirRefState_fan_speed_to_command(airRefState);
  ar_state->fan_time_last_command =
      AirRef_AirRefState_fan_time_last_command(airRefState);
}

void load_ar_conf(flatcc_builder_t *B, air_ref_conf_t *ar_conf) {
  AirRef_AirRefConf_start(B);

  AirRef_AirRefConf_control_type_add(B, ar_conf->control_type);
  AirRef_AirRefConf_fan_target_pressure_add(B, ar_conf->fan_target_pressure);
  AirRef_AirRefConf_fan_coeff_p_add(B, ar_conf->fan_coeff_P);
  AirRef_AirRefConf_fan_coeff_offset_add(B, ar_conf->fan_coeff_offset);
  AirRef_AirRefConf_fan_min_pressure_add(B, ar_conf->fan_min_pressure);
  AirRef_AirRefConf_fan_max_pressure_add(B, ar_conf->fan_max_pressure);
  AirRef_AirRefConf_compressor_target_pressure_add(
      B, ar_conf->compressor_target_pressure);
  AirRef_AirRefConf_compressor_coeff_P_add(B, ar_conf->compressor_coeff_P);
  AirRef_AirRefConf_compressor_coeff_I_add(B, ar_conf->compressor_coeff_I);
  AirRef_AirRefConf_compressor_start_interval_add(
      B, ar_conf->compressor_start_interval);

  AirRef_AirRefConf_low_pressure_limit_add(B, ar_conf->LP_low_pressure_limit);
  AirRef_AirRefConf_ref_t airRefConf = AirRef_AirRefConf_end(B);
  AirRef_Content_union_ref_t content = AirRef_Content_as_AirRefConf(airRefConf);
  AirRef_Message_create_as_root(B, content);
}

void parse_ar_conf(AirRef_AirRefConf_table_t airRefConf,
                   air_ref_conf_t *ar_conf) {
  ar_conf->control_type = AirRef_AirRefConf_control_type(airRefConf);

  ar_conf->fan_target_pressure =
      AirRef_AirRefConf_fan_target_pressure(airRefConf);
  ar_conf->fan_coeff_P = AirRef_AirRefConf_fan_coeff_p(airRefConf);
  ar_conf->fan_coeff_offset = AirRef_AirRefConf_fan_coeff_offset(airRefConf);
  ar_conf->fan_min_pressure = AirRef_AirRefConf_fan_min_pressure(airRefConf);
  ar_conf->fan_max_pressure = AirRef_AirRefConf_fan_max_pressure(airRefConf);
  ar_conf->compressor_target_pressure =
      AirRef_AirRefConf_compressor_target_pressure(airRefConf);
  ar_conf->compressor_coeff_P =
      AirRef_AirRefConf_compressor_coeff_P(airRefConf);
  ar_conf->compressor_coeff_I =
      AirRef_AirRefConf_compressor_coeff_I(airRefConf);
  ar_conf->compressor_start_interval =
      AirRef_AirRefConf_compressor_start_interval(airRefConf);
  ar_conf->LP_low_pressure_limit =
      AirRef_AirRefConf_low_pressure_limit(airRefConf);
}

void load_m_state(flatcc_builder_t *B, machine_state_t *m_state) {
  // reset builder?
  AirRef_ErrorReport_t imc102_communication;
  AirRef_ErrorReport_t imc102_motor;
  AirRef_MotorStatus_t imc102_status;

  imc102_communication.error_code = m_state->imc102_communication.error_code;
  imc102_communication.error_state = m_state->imc102_communication.error_state;

  imc102_motor.error_code = m_state->imc102_communication.error_code;
  imc102_motor.error_state = m_state->imc102_communication.error_state;

  for (int i = 0; i < INDEX_MOTOR_SIZE; i++) {
    imc102_status.state[i] = m_state->imc102_status[i];
  }

  AirRef_ErrorReport_t imm101_communication;
  AirRef_ErrorReport_t imm101_motor;
  AirRef_MotorStatus_t imm101_status;

  imm101_communication.error_code = m_state->imm101_communication.error_code;
  imm101_communication.error_state = m_state->imm101_communication.error_state;

  imm101_motor.error_code = m_state->imm101_communication.error_code;
  imm101_motor.error_state = m_state->imm101_communication.error_state;

  for (int i = 0; i < INDEX_MOTOR_SIZE; i++) {
    imm101_status.state[i] = m_state->imm101_status[i];
  }

  AirRef_MachineState_start(B);
  AirRef_MachineState_evaporation_pressure_add(B,
                                               m_state->evaporation_pressure);
  AirRef_MachineState_condensation_pressure_add(B,
                                                m_state->condensation_pressure);
  AirRef_MachineState_temperature_gas_scarico_add(
      B, m_state->temperature_gas_scarico);
  AirRef_MachineState_temperature_environment_add(
      B, m_state->temperature_environment);
  AirRef_MachineState_temperature_gas_ritorno_add(
      B, m_state->temperature_gas_ritorno);
  AirRef_MachineState_temperature_extra_add(B, m_state->temperature_extra);
  AirRef_MachineState_imc102_communication_add(B, &imc102_communication);
  AirRef_MachineState_imc102_motor_add(B, &imc102_motor);
  AirRef_MachineState_imc102_status_add(B, &imc102_status);
  AirRef_MachineState_imm101_communication_add(B, &imm101_communication);
  AirRef_MachineState_imm101_motor_add(B, &imm101_motor);
  AirRef_MachineState_imm101_status_add(B, &imm101_status);
  AirRef_MachineState_ref_t machineState = AirRef_MachineState_end(B);

  AirRef_Content_union_ref_t content =
      AirRef_Content_as_MachineState(machineState);

  AirRef_Message_create_as_root(B, content);
}

void parse_m_state(AirRef_MachineState_table_t machineState,
                   machine_state_t *m_state) {
  m_state->condensation_pressure =
      AirRef_MachineState_condensation_pressure(machineState);
  m_state->evaporation_pressure =
      AirRef_MachineState_evaporation_pressure(machineState);
  m_state->temperature_gas_scarico =
      AirRef_MachineState_temperature_gas_scarico(machineState);
  m_state->temperature_environment =
      AirRef_MachineState_temperature_environment(machineState);
  m_state->temperature_gas_ritorno =
      AirRef_MachineState_temperature_gas_ritorno(machineState);
  m_state->temperature_extra =
      AirRef_MachineState_temperature_extra(machineState);

  const AirRef_ErrorReport_t *imc102_communication =
      AirRef_MachineState_imc102_communication(machineState);
  const AirRef_ErrorReport_t *imc102_motor =
      AirRef_MachineState_imc102_motor(machineState);
  const AirRef_MotorStatus_t *imc102_status =
      AirRef_MachineState_imc102_status(machineState);

  const AirRef_ErrorReport_t *imm101_communication =
      AirRef_MachineState_imm101_communication(machineState);
  const AirRef_ErrorReport_t *imm101_motor =
      AirRef_MachineState_imm101_motor(machineState);
  const AirRef_MotorStatus_t *imm101_status =
      AirRef_MachineState_imm101_status(machineState);

  m_state->imm101_communication.error_code = imm101_communication->error_code;
  m_state->imm101_communication.error_state = imm101_communication->error_state;

  m_state->imm101_communication.error_code = imm101_motor->error_code;
  m_state->imm101_communication.error_state = imm101_motor->error_state;

  for (int i = 0; i < INDEX_MOTOR_SIZE; i++) {
    m_state->imm101_status[i] = imm101_status->state[i];
  }

  m_state->imc102_communication.error_code = imc102_communication->error_code;
  m_state->imc102_communication.error_state = imc102_communication->error_state;

  m_state->imc102_communication.error_code = imc102_motor->error_code;
  m_state->imc102_communication.error_state = imc102_motor->error_state;

  for (int i = 0; i < INDEX_MOTOR_SIZE; i++) {
    m_state->imc102_status[i] = imc102_status->state[i];
  }
}

int finalize_and_send_builder(flatcc_builder_t *builder) {
  int ret;
  size_t size;
  void *buf;
  buf = flatcc_builder_finalize_buffer(builder, &size);

  ret = send_data(buf, size);

  flatcc_builder_aligned_free(buf);
  flatcc_builder_clear(builder);
  return ret;
}

int send_new_conf(air_ref_conf_t *ar_conf_new) {
  flatcc_builder_t builder;
  flatcc_builder_init(&builder);
  load_ar_conf(&builder, &ar_conf);
  return finalize_and_send_builder(&builder);
}

int do_request_m_state() {
  flatcc_builder_t builder;

  flatcc_builder_init(&builder);

  AirRef_Request_start(&builder);
  AirRef_Request_request_type_add(&builder,
                                  AirRef_RequestType_RequestMachineState);
  AirRef_Request_ref_t ar_request = AirRef_Request_end(&builder);

  AirRef_Content_union_ref_t content = AirRef_Content_as_Request(ar_request);
  AirRef_Message_create_as_root(&builder, content);

  return finalize_and_send_builder(&builder);
}

int do_reply_m_state() {
  flatcc_builder_t builder;

  flatcc_builder_init(&builder);
  load_m_state(&builder, &m_state);
  return finalize_and_send_builder(&builder);
}

int do_request_ar_state() {
  flatcc_builder_t builder;

  flatcc_builder_init(&builder);

  AirRef_Request_start(&builder);
  AirRef_Request_request_type_add(&builder,
                                  AirRef_RequestType_RequestAirRefState);
  AirRef_Request_ref_t ar_request = AirRef_Request_end(&builder);

  AirRef_Content_union_ref_t content = AirRef_Content_as_Request(ar_request);
  AirRef_Message_create_as_root(&builder, content);
  return finalize_and_send_builder(&builder);
}
int do_reply_ar_state() {
  flatcc_builder_t builder;

  flatcc_builder_init(&builder);
  load_ar_state(&builder, &ar_state);
  return finalize_and_send_builder(&builder);
}

int do_request_ar_conf() {
  flatcc_builder_t builder;

  flatcc_builder_init(&builder);
  AirRef_Request_start(&builder);
  AirRef_Request_request_type_add(&builder,
                                  AirRef_RequestType_RequestAirRefConf);
  AirRef_Request_ref_t ar_request = AirRef_Request_end(&builder);
  AirRef_Content_union_ref_t content = AirRef_Content_as_Request(ar_request);
  AirRef_Message_create_as_root(&builder, content);

  return finalize_and_send_builder(&builder);
}

int do_reply_ar_conf() {
  flatcc_builder_t builder;

  flatcc_builder_init(&builder);
  load_ar_conf(&builder, &ar_conf);
  return finalize_and_send_builder(&builder);
}


int configuration_is_valid(air_ref_conf_t *ar_conf){
    
    if(ar_conf->compressor_target_pressure < 0){
        return 0;
    }
    //TODO do all tests

    return 1;
}