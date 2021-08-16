
#include "logger_hw.h"
#include "logger_air_ref.h"
#include "logger_frame.h"

machine_state_t m_state;
air_ref_conf_t ar_conf;
air_ref_conf_t ar_conf_old;
air_ref_state_t ar_state;

TaskHandle_t xSenderTask;
TaskHandle_t xReceiverTask;

uint32_t timestamp_last_update_m_state;
uint32_t timestamp_last_update_ar_state;
uint32_t timestamp_last_update_ar_conf;
extern const uart_port_t uart_num;

int access_message_buffer(const void *buffer)
{

    AirRef_Message_table_t message = AirRef_Message_as_root(buffer);
    test_assert(message != 0);
    if (AirRef_Message_content_type(message) == AirRef_Content_MachineState)
    {
        //update local machine state
        ESP_LOGI("DECODED", "MachineState");
        AirRef_MachineState_table_t machineState = (AirRef_MachineState_table_t)AirRef_Message_content(message);

        parse_m_state(machineState, &m_state);
    }
    else if (AirRef_Message_content_type(message) == AirRef_Content_AirRefConf)
    {
                //update local airrefconf
        ESP_LOGI("DECODED", "AirRefConf");
        AirRef_AirRefConf_table_t airRefConf = (AirRef_AirRefConf_table_t)AirRef_Message_content(message);
        parse_ar_conf(airRefConf, &ar_conf);
        log_ar_conf( &ar_conf);

    }
    else if (AirRef_Message_content_type(message) == AirRef_Content_AirRefState)
    {
        //update local airrefstate
        ESP_LOGI("DECODED", "AirRefState");
        AirRef_AirRefState_table_t airRefState = (AirRef_AirRefState_table_t)AirRef_Message_content(message);
        parse_ar_state(airRefState, &ar_state);
    }
    else if (AirRef_Message_content_type(message) == AirRef_Content_Request)
    {
        AirRef_Request_table_t request = (AirRef_Request_table_t)AirRef_Message_content(message);

        if (AirRef_Request_request_type_get(request) == AirRef_RequestType_AirRefConfReceived)
        {
            ESP_LOGI("DECODED", "RICEVUTA CONFERMA DI CONFIGURAZIONE\n");
        }
        //if request is air_ref_conf_received -> reset state to idle
    }
    return 0;
}

void routine_send_new_conf(air_ref_conf_t *ar_conf_new)
{
    ESP_LOGI("ROUTINE SEND NEW CONF", "mandando new conf\n");
    // send conf
    //TODO check message is received
    //machine state
    send_new_conf(ar_conf_new);
    // state 0 idle
    // state 1 waiting for reply
}

static void logger_task(void *arg)
{
    timestamp_last_update_m_state = xTaskGetTickCount();
    timestamp_last_update_ar_state = xTaskGetTickCount();
    timestamp_last_update_ar_conf = xTaskGetTickCount();
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    ESP_LOGI(LOGGER_TAG, "asking for ar_conf");
    do_request_ar_conf();
    vTaskDelay(800 / portTICK_PERIOD_MS);
    while (1)
    {


        if (timestamp_last_update_m_state + 2000 / portTICK_PERIOD_MS < xTaskGetTickCount())
        {
            ESP_LOGI(LOGGER_TAG, "asking for m_state");
            timestamp_last_update_m_state = xTaskGetTickCount();
            do_request_m_state();
        }
        else if (timestamp_last_update_ar_state + 3000 / portTICK_PERIOD_MS < xTaskGetTickCount())
        {
            ESP_LOGI(LOGGER_TAG, "asking for ar_state state");
            (timestamp_last_update_ar_state = xTaskGetTickCount());
            do_request_ar_state();
        }
        vTaskDelay(800 / portTICK_PERIOD_MS);
        
        // if status is waiting for confirm (ar_conf update) for more than given time -> resend conf
    }
}

static void receiver_task(void *arg)
{

    int length;
    logger_frame_t reply;
    uint8_t data[LOGGER_BUF_SIZE * 4];

    while (1)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
        length = uart_read_bytes(uart_num, data, LOGGER_BUF_SIZE, 20 / portTICK_RATE_MS);
        if (length > 0)
        {
            add_to_ringbuffer(data, length);
        }

        if ((length = take_frame(data)) > 0)
        {

            if (receive_frame(&reply, data, length))
            {
                access_message_buffer(reply.buffer);
            }
            else
            {

            }
        }
    }
}

void logger_init()
{

    xTaskCreate(logger_task, "logger_task", SENDER_TASK_STACK_SIZE, NULL, TASK_SENDER_STACK_PRIORITY, &(xSenderTask));
    xTaskCreate(receiver_task, "receiver_task", RECEIVER_TASK_STACK_SIZE, NULL, TASK_RECEIVCER_STACK_PRIORITY, &(xReceiverTask));
}
