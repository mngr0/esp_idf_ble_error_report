#ifndef SERIAL_LOGGER_AIR_REF_H_
#define SERIAL_LOGGER_AIR_REF_H_

#include "logger_hw.h"

#define test_assert(x) \
	do                 \
	{                  \
		if (!(x))      \
		{              \
			assert(0); \
		}              \
	} while (0)


void send_new_conf(air_ref_conf_t *ar_conf_new);

void do_request_m_state();

void do_reply_m_state();

void do_request_ar_state();

void do_request_ar_conf();

void load_ar_state(flatcc_builder_t *B, air_ref_state_t *ar_state);

void load_m_state(flatcc_builder_t *B, machine_state_t *m_state);

void load_ar_conf(flatcc_builder_t *B, air_ref_conf_t *ar_conf);

void parse_m_state(AirRef_MachineState_table_t machineState, machine_state_t *m_state);

void parse_ar_conf(AirRef_AirRefConf_table_t airRefConf, air_ref_conf_t *ar_conf);


void parse_ar_state(AirRef_AirRefState_table_t airRefState, air_ref_state_t *ar_state);

void log_ar_conf( air_ref_conf_t *ar_conf);

#endif