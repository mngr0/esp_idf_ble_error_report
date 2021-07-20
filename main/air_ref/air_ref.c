
#include "esp_log.h"

#include "fb/airref_builder.h"
#include "fb/airref_reader.h"


#ifdef ns
#error "ns already used"
#endif

#define ns(x) FLATBUFFERS_WRAP_NAMESPACE(AirRef, x) // Specified in the schema.
#define test_assert(x) do { if (!(x)) { assert(0); return -1; }} while(0)

// current conf

void send_ar_conf(ns(AirRefConf_table_t) * ar_conf, uint8_t *buffer)
{
    //build object configuration edits
}

void receive_m_state(const void *buffer)
{
    //ns(Monster_table_t) monster = ns(Monster_as_root(buffer));
}

void receive_ar_state(const void *buffer)
{
}

void receive_ar_conf(const void *buffer)
{
}

#include "air_ref.h"

void send_m_state(flatcc_builder_t *B, machine_state_t *m_state)
{

    //ns(MachineState_table_t) machineState =
    ns(MachineState_start(B));
    //ns(MachineState_push_start(B));
    ns(MachineState_evaporation_pressure_add(B, 3));

    //ns(MachineState_push_end(B));
    ns(MachineState_ref_t) machineState = ns(MachineState_end(B));

    ns(Content_union_ref_t) content = ns(Content_as_MachineState(machineState));

    ns(Message_create_as_root(B, content));
}
