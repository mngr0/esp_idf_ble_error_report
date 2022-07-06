#ifndef MC_LOGGER_POLL_H
#define MC_LOGGER_POLL_H


#include "gel/abstract_state_machine/abstract_state_machine.h"
#include "gel/abstract_state_machine/conf_generator.h"


#define FOREACH_MS_LOGGER_INIT(ITERATE_STATE)               \
ITERATE_STATE(ms_logger_init, read_dev_type)                \
ITERATE_STATE(ms_logger_init, operating)                    \
ITERATE_STATE(ms_logger_init, unknown_dev)                  \


DECLARE_MACHINE_STATE_STATES_ENUM(ms_logger_init, FOREACH_MS_LOGGER_INIT);

DECLARE_FUNCTION_ROUTINE(ms_logger_init);



#define FOREACH_PERMANENCE_CHECK_ms_logger_init_read_dev_type(ITERATE_CONDITION)			\
ITERATE_CONDITION(ms_logger_init, ms_logger_init, execute_logger)                          \
ITERATE_CONDITION(ms_logger_init, read_dev_type, start_reading_dev_type)                   

#define FOREACH_CHANGE_CHECK_ms_logger_init_read_dev_type(ITERATE_CONDITION)				\
ITERATE_CONDITION(ms_logger_init, read_dev_type, dev_type_ready_to_read)


#define FOREACH_PERMANENCE_CHECK_ms_logger_init_operating(ITERATE_CONDITION)			\
ITERATE_CONDITION(ms_logger_init, ms_logger_init, execute_logger)

#define FOREACH_CHANGE_CHECK_ms_logger_init_operating(ITERATE_CONDITION)

#define FOREACH_PERMANENCE_CHECK_ms_logger_init_unknown_dev(ITERATE_CONDITION)

#define FOREACH_CHANGE_CHECK_ms_logger_init_unknown_dev(ITERATE_CONDITION)



#endif