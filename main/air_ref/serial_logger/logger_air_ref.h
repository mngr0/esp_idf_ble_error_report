#ifndef SERIAL_LOGGER_AIR_REF_H_
#define SERIAL_LOGGER_AIR_REF_H_

#include "logger_hw.h"



void send_new_conf(air_ref_conf_t *ar_conf_new);

void do_request_m_state();

void do_reply_m_state();

void do_request_ar_state();

void do_request_ar_conf();


#endif