#ifndef _ESP32_BRIDGE_H
#define _ESP32_BRIDGE_H


#include "gel/abstract_state_machine/conf_generator.h"
#include "gel/abstract_state_machine/configuration_packet/configuration_packet.h"
#include "gel/abstract_state_machine/abstract_machines/mp_configuration_polling.h"
#include "gel/abstract_state_machine/abstract_state_machine.h"

#include "ble/ble_lib/ble_utils.h"


#define REMOTE_DECLARATION
#include "chiller.h"

#undef FOREACH_R_STATUS
#undef FOREACH_R_CONF

#include "termostatica.h"
static char termostatica_status_names[][MAX_STR_LEN] = {FOREACH_R_STATUS(STRINGIFY)};
static char termostatica_conf_names[][MAX_STR_LEN] = {FOREACH_R_CONF(STRINGIFY)};
#undef FOREACH_R_STATUS
#undef FOREACH_R_CONF


#undef REMOTE_DECLARATION

typedef enum{
logger_initializiation_state_read_dev_type =0,
logger_initializiation_state_size_unknown_device,
logger_initializiation_state_size_operate,
}logger_initializiation_state_t;


#define FOREACH_R_CONF(OPERATION)                       \
FOREACH_CONFIGURATION_POLLING_CONF(logger, OPERATION)	\
OPERATION(can_remote_address)		                    \
OPERATION(can_local_address)		                    \




#define FOREACH_R_STATUS(OPERATION)                     \
FOREACH_CONFIGURATION_POLLING_STATUS(logger, OPERATION) \
OPERATION(logger_init_ms_state) 		                \
OPERATION(logger_initializiation_state)                 \
OPERATION(current_symbol_shown)		

DECLARE_STATUS;

DECLARE_CONF;


#endif