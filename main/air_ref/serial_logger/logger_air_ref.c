
#include "stdbool.h"
#include "logger_hw.h"
#include "logger_frame.h"
#include "../air_ref.h"


#ifdef ns
#error "ns already used"
#endif

#define ns(x) FLATBUFFERS_WRAP_NAMESPACE(AirRef, x) // Specified in the schema.
#define test_assert(x) do { if (!(x)) { assert(0); return -1; }} while(0)


extern air_ref_conf_t ar_conf;
extern air_ref_conf_t ar_conf_old;
extern bool ready_to_update_ar_conf;         

void send_new_conf(air_ref_conf_t *ar_conf_new)
{
    //or... just send
    flatcc_builder_t builder;
    logger_frame_t request;

    flatcc_builder_init(&builder);
    // TODO create message
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
    AirRef_Request_requestType_add(&builder,AirRef_RequestType_RequestMachineState);
    // TODO create message
    //AirRef_Request_ref_t ar_request = AirRef_Request_end(&builder);
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
     // TODO create message
    flatcc_builder_init(&builder);
   



    build_frame(&request, &builder);
    send_frame(&request);
    flatcc_builder_clear(&builder);
}

void do_request_ar_state()
{
    flatcc_builder_t builder;
    logger_frame_t request;

    flatcc_builder_init(&builder);
    // TODO create message
    build_frame(&request, &builder);
    send_frame(&request);
    flatcc_builder_clear(&builder);
}

void do_request_ar_conf()
{
    flatcc_builder_t builder;
    logger_frame_t request;

    flatcc_builder_init(&builder);
    // TODO create message
    build_frame(&request, &builder);
    send_frame(&request);
    flatcc_builder_clear(&builder);
}


int access_message_buffer(const void *buffer)
{
    // Note that we use the `table_t` suffix when reading a table object
    // as opposed to the `ref_t` suffix used during the construction of
    // the buffer.

    ns(Message_table_t) message = ns(Message_as_root(buffer));
    test_assert(message != 0);
    ESP_LOGI("DECODED", "assert ok");
    if (ns(Message_content_type(message)) == ns(Content_MachineState))
    {
        //update local machine state
        ESP_LOGI("DECODED", "right content type");
        // Note: root object pointers are NOT the same as the `buffer` pointer.

        // Make sure the buffer is accessible.

        ns(MachineState_table_t) m_state = (ns(MachineState_table_t))ns(Message_content(message));

        int32_t evaporation_pressure = ns(MachineState_evaporation_pressure(m_state));
        int32_t condensation_pressure = ns(MachineState_condensation_pressure(m_state));
        int32_t temperature_gas_scarico = ns(MachineState_temperature_gas_scarico(m_state));
        int32_t temperature_environment = ns(MachineState_temperature_environment(m_state));
        int32_t temperature_gas_ritorno = ns(MachineState_temperature_gas_ritorno(m_state));
        int32_t temperature_extra = ns(MachineState_temperature_extra(m_state));

        ESP_LOGI("DECODED", "evaporation_pressure:%d\t",evaporation_pressure);
        ESP_LOGI("DECODED", "condensation_pressure:%d\t",condensation_pressure);
        ESP_LOGI("DECODED", "temperature_gas_scarico:%d\t",temperature_gas_scarico);
        ESP_LOGI("DECODED", "temperature_environment:%d\t",temperature_environment);
        ESP_LOGI("DECODED", "temperature_gas_ritorno:%d\t",temperature_gas_ritorno);
        ESP_LOGI("DECODED", "temperature_extra:%d\n",temperature_extra);
    }
    else if (ns(Message_content_type(message)) == ns(Content_AirRefConf)){
        //update local airrefconf
    }
    else if (ns(Message_content_type(message)) == ns(Content_AirRefState)){
        //update local airrefstate
    }
    
    return 0;
}