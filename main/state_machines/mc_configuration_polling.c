
#include "abstract_state_machine/common.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "peripherals/uart.h"

#include "gel/abstract_state_machine/abstract_machines/mp_configuration_polling.h"
#include "gel/abstract_state_machine/abstract_machines/ms_configuration_polling.h"
#include "gel/abstract_state_machine/configuration_packet/configuration_packet.h"

#include "gel/packet_manager/packet_manager.h"

#include "mc_configuration_polling.h"

#include "esp32_bridge.h"

#define BASE_R_CONF logger_timeout
#define BASE_R_STATUS logger_ms_state

validated_field_t *routine_conf_remote;
int32_t *routine_status_remote;

machine_parameters_t mp_remote = {.routine_conf = NULL,
                                  .routine_conf_size = 0,
                                  .routine_status = NULL,
                                  .routine_status_size = 0};

enum { CONF_INIT(ENUMERATE) CONF_INIT_SIZE };

update_intervals_t *ts_list_status;
update_intervals_t *ts_list_conf;

packet_manager_t pm;

configuration_packet_t configuration_packet;
static TaskHandle_t xCANLoggerTask;
static machine_parameters_t *mp;
static QueueHandle_t queue_recv_msg;
static QueueHandle_t queue_recv_cmd;
static message_t memory_message;

remote_device_t remote_device;

static void *access_memory(const uint8_t p1, uint8_t p2) {

  switch (p1) {
  case configuration_polling_access_memory_type_conf: {
    if (p2 > mp_configuration_polling_conf_size) {
      while (1)
        ;
    }
    return (void *)(&mp->routine_conf[BASE_R_CONF + p2].value);
  }
  case configuration_polling_access_memory_type_status: {
    if (p2 > mp_configuration_polling_status_size) {
      while (1)
        ;
    }
    return (void *)(&mp->routine_status[BASE_R_STATUS + p2]);
  }
  case configuration_polling_access_memory_type_configuration_packet: {
    return (void *)(&configuration_packet);
  }
  case configuration_polling_access_memory_type_memory_message: {
    return (void *)(&memory_message);
  }
  case configuration_polling_access_memory_type_queue_recv_cmd: {
    return (void *)(queue_recv_cmd);
  }
  case configuration_polling_access_memory_type_queue_recv_msg: {
    return (void *)(queue_recv_msg);
  }

  default: {
    while (1)
      ;
    return NULL;
  }
  }
  return NULL;
}

static ms_configuration_polling_enum_t
check_condition_ms_configuration_polling_change_ms_configuration_polling_timeout_limit(
    machine_parameters_t *mp) {
  ms_configuration_polling_enum_t next_state =
      abstract_check_condition_ms_configuration_polling_change_ms_configuration_polling_timeout_limit(
          access_memory);
  if ((mp->routine_status[logger_ms_state] ==
       ms_configuration_polling_enum_write_configuration) &&
      (next_state == ms_configuration_polling_enum_communication_error)) {
    // mp->routine_status[display_homepage_can_write_err_timestamp]=
    // xTaskGetTickCount();
  }
  return next_state;
}

CONDITION_PERMANENCE_CONCRETE_CALLS_ABSTRACT(ms_configuration_polling,
                                             read_configuration, esegui_lettura)

CONDITION_PERMANENCE_CONCRETE_CALLS_ABSTRACT(ms_configuration_polling,
                                             read_configuration,
                                             ricevi_messaggio)

CONDITION_CHANGE_CONCRETE_CALLS_ABSTRACT(ms_configuration_polling,
                                         read_configuration, lettura_eseguita)

CONDITION_PERMANENCE_CONCRETE_CALLS_ABSTRACT(ms_configuration_polling,
                                             read_state, esegui_lettura)

CONDITION_PERMANENCE_CONCRETE_CALLS_ABSTRACT(ms_configuration_polling,
                                             read_state, ricevi_messaggio)

CONDITION_CHANGE_CONCRETE_CALLS_ABSTRACT(ms_configuration_polling,
                                         communication_error,
                                         comunicazione_restored)

CONDITION_PERMANENCE_CONCRETE_CALLS_ABSTRACT(ms_configuration_polling,
                                             write_configuration,
                                             esegui_scrittura)

CONDITION_PERMANENCE_CONCRETE_CALLS_ABSTRACT(ms_configuration_polling,
                                             write_configuration,
                                             ricevi_messaggio)

CONDITION_CHANGE_CONCRETE_CALLS_ABSTRACT(ms_configuration_polling,
                                         write_configuration,
                                         scrittura_eseguita)

CONDITION_PERMANENCE_CONCRETE_CALLS_ABSTRACT(ms_configuration_polling,
                                             read_conf_limit, esegui_lettura)

CONDITION_PERMANENCE_CONCRETE_CALLS_ABSTRACT(ms_configuration_polling,
                                             read_conf_limit, ricevi_messaggio)

CONDITION_CHANGE_CONCRETE_CALLS_ABSTRACT(ms_configuration_polling,
                                         read_conf_limit, lettura_eseguita)

DEFINE_FUNCTION_ROUTINE(mc_can_configuration_polling, ms_configuration_polling,
                        FOREACH_MS_CONFIGURATOR_CAN_STATE)

void read_limits(uint8_t dev_index, uint8_t index) {
  abstract_external_change_state_ms_configuration_polling(
      access_memory,
      ENUMERATION_STATE_NAME(ms_configuration_polling, read_conf_limit_max));
  if (mp->routine_status[logger_ms_state] ==
      ENUMERATION_STATE_NAME(ms_configuration_polling, read_conf_limit_max)) {
    mp->routine_status[logger_counter_index] = index;
    mp->routine_status[logger_counter_address] = dev_index;
  }
}

void prepare_mp(uint16_t conf_size, uint16_t status_size) {
  if (status_size > 0) {
    routine_status_remote = malloc(sizeof(int32_t) * status_size);
    ts_list_status = malloc(sizeof(update_intervals_t) * status_size);
  } else {
    routine_status_remote = NULL;
    ts_list_status = NULL;
  }
  mp_remote.routine_status = routine_status_remote;
  mp_remote.routine_status_size = status_size;
  remote_device.ts_list_status = ts_list_status;
  for (uint8_t i = 0; i < status_size; i++) {
    ts_list_status[i].interval = 1000;
    ts_list_status[i].last_ts = 0;
  }

  if (conf_size > 0) {
    routine_conf_remote = malloc(sizeof(validated_field_t) * conf_size);
    ts_list_conf = malloc(sizeof(update_intervals_t) * conf_size);
  } else {
    routine_conf_remote = NULL;
    ts_list_conf = NULL;
  }
  mp_remote.routine_conf = routine_conf_remote;
  mp_remote.routine_conf_size = conf_size;
  remote_device.ts_list_conf = ts_list_conf;
  for (uint8_t i = 0; i < conf_size; i++) {
    ts_list_conf[i].interval = 1000;
    ts_list_conf[i].last_ts = 0;
  }
}

DECLARE_CHANGE_FUNCTION(ms_logger_init, read_dev_type, dev_type_ready_to_read) {
  // todo err comm
  if (mp->routine_status[device_type] !=
      ms_configuration_polling_enum_read_configuration) {
    switch (routine_status_remote[device_type]) {
    case device_type_termostatica: {

      free(routine_conf_remote);
      prepare_mp(routine_termostatica_conf_parameters_size,
                 routine_termostatica_status_parameters_size);
      abstract_external_change_state_ms_configuration_polling(
          access_memory, ms_configuration_polling_enum_read_configuration);
      return ENUMERATION_STATE_NAME(ms_logger_init, operating);
      break;
    }
    case device_type_chiller: {
      free(routine_conf_remote);
      prepare_mp(routine_chiller_conf_parameters_size,
                 routine_chiller_status_parameters_size);
      abstract_external_change_state_ms_configuration_polling(
          access_memory, ms_configuration_polling_enum_read_configuration);
      return ENUMERATION_STATE_NAME(ms_logger_init, operating);
      break;
    }
    case device_type_termostato: {
      free(routine_conf_remote);
      prepare_mp(10, 10);
      return ENUMERATION_STATE_NAME(ms_logger_init, operating);
      break;
    }
    case device_type_my_feeder: {
      free(routine_conf_remote);
      prepare_mp(10, 10);
      return ENUMERATION_STATE_NAME(ms_logger_init, operating);
      break;
    }
    default: {
      return ENUMERATION_STATE_NAME(ms_logger_init, unknown_dev);
    }
    }
  }
  return ENUMERATION_STATE_NAME(ms_logger_init, read_dev_type);
}

DECLARE_PERMANENCE_FUNCTION(ms_logger_init, ms_logger_init, execute_logger) {
  mc_can_configuration_polling_routine(logger_ms_state, mp);
}

DECLARE_PERMANENCE_FUNCTION(ms_logger_init, read_dev_type,
                            start_reading_dev_type) {
  if (mp_remote.routine_status_size == 0) {
    prepare_mp(CONF_INIT_SIZE, 0);
    abstract_external_change_state_ms_configuration_polling(
        access_memory, ms_configuration_polling_enum_read_configuration);
  }
}

DEFINE_FUNCTION_ROUTINE(ms_logger_init, ms_logger_init, FOREACH_MS_LOGGER_INIT);

char logger_state_names[ms_configuration_polling_enum_size][40] = {
    FOREACH_MS_CONFIGURATOR_CAN_STATE(STRINGIFY_STATE_NAME)};

void CAN_configurator_task(void *p) {
  mp->routine_status[logger_ms_state] =
      ms_configuration_polling_enum_read_configuration;
  while (1) {
    mc_can_configuration_polling_routine(logger_ms_state, mp);
    ms_logger_init_routine(logger_init_ms_state, mp);
    vTaskDelay(30);
  }
}

int send_data(uint8_t *data, size_t lenght) {
  return packet_manager_send_data(&pm, data, lenght);
}

void configurator_SERIAL_init(
    machine_parameters_t *machine_parameters,
    void (*p_manage_event)(machine_parameters_t *machine_parameters,
                           uint8_t event_type, int32_t event_parameter),
    void (*read_from_flash)(validated_field_t *fields, const uint32_t VF_size),
    void (*write_on_flash)(validated_field_t *fields, const uint32_t VF_size)) {

  mp = machine_parameters;
  serial_configure();
  packet_manager_init(&pm, serial_send_buffer);

  configuration_packet_init(&configuration_packet, &mp_remote, send_data,
                            p_manage_event, read_from_flash, write_on_flash);
  remote_device.remote_can_address =
      machine_parameters->routine_conf[can_remote_address].value;
  configuration_packet.can_address =
      machine_parameters->routine_conf[can_local_address].value;
  configuration_packet_enable_remotes(&configuration_packet, &remote_device, 1);

  // configuration_packet.can_address = 24; 3 il remoto
  queue_recv_msg = xQueueCreate(5, sizeof(msg_and_id_t));
  queue_recv_cmd = xQueueCreate(20, 8);

  if (xTaskCreate(CAN_configurator_task, "CANLogger", 2048, mp, 3,
                  &xCANLoggerTask) != pdPASS) {
    while (1) {
      ;
    }
  }
}
