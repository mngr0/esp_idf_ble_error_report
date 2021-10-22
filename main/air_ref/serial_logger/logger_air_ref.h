#ifndef SERIAL_LOGGER_AIR_REF_H_
#define SERIAL_LOGGER_AIR_REF_H_

#include "air_ref/air_ref.h"
#include "airref_builder.h"

int send_new_conf(air_ref_conf_t *ar_conf_new);

int do_request_m_state();

int do_reply_m_state();

int do_request_ar_state();

int do_request_ar_conf();

void load_ar_state(flatcc_builder_t *B, air_ref_state_t *ar_state);

void load_m_state(flatcc_builder_t *B, machine_state_t *m_state);

void load_ar_conf(flatcc_builder_t *B, air_ref_conf_t *ar_conf);

void load_log_state(flatcc_builder_t *B, int sticazzi);

void parse_m_state(AirRef_MachineState_table_t machineState, machine_state_t *m_state);

void parse_ar_conf(AirRef_AirRefConf_table_t airRefConf, air_ref_conf_t *ar_conf);

void parse_ar_state(AirRef_AirRefState_table_t airRefState, air_ref_state_t *ar_state);


int configuration_is_valid(air_ref_conf_t *ar_conf);
    

#endif