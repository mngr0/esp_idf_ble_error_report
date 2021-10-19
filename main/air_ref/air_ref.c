
#include "logger_frame/logger_hw.h"
#include "serial_logger/logger_air_ref.h"
#include "logger_frame/logger_frame.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "driver/uart.h"
extern const uart_port_t uart_num;

#define AIR_REF_TAG "AIR_REF_TAG"


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
static bool conf_received;
static bool sending_new_conf;

int access_message_buffer(const void *buffer)
{

    AirRef_Message_table_t message = AirRef_Message_as_root(buffer);

    if (message != 0)
    {

        if (AirRef_Message_content_type(message) == AirRef_Content_MachineState)
        {
            //update local machine state
            ESP_LOGI("DECODED", "MachineState");
            AirRef_MachineState_table_t machineState = (AirRef_MachineState_table_t)AirRef_Message_content(message);

            parse_m_state(machineState, &m_state);
            log_m_state(&m_state);
        }
        else if (AirRef_Message_content_type(message) == AirRef_Content_AirRefConf)
        {
            //update local airrefconf
            ESP_LOGI("DECODED", "AirRefConf");
            AirRef_AirRefConf_table_t airRefConf = (AirRef_AirRefConf_table_t)AirRef_Message_content(message);
            parse_ar_conf(airRefConf, &ar_conf);
            log_ar_conf(&ar_conf); //tmp_ar_conf
            conf_received = true;
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

                if (sending_new_conf)
                {
                    //TODO check that received conf is right) *is this necessary?()
                    //use tmp_ar_conf
                    //compare tmp_ar_conf with ar_conf

                    //if equal
                    sending_new_conf = false;
                    //memcpy(&ar_conf_old,&ar_conf,sizeof(air_ref_conf_t));
                }
            }
            //if request is air_ref_conf_received -> reset state to idle
        }
    }
    return 0;
}

void routine_send_new_conf(air_ref_conf_t *ar_conf_new)
{
    ESP_LOGI("ROUTINE SEND NEW CONF", "mandando new conf\n");
    memcpy(&ar_conf_old, &ar_conf, sizeof(air_ref_conf_t));
    memcpy(&ar_conf, ar_conf_new, sizeof(air_ref_conf_t));
    sending_new_conf = true;
    //send_new_conf(ar_conf_new);
}

static void logger_task(void *arg)
{
    conf_received = false;
    sending_new_conf = false;
    timestamp_last_update_m_state = xTaskGetTickCount();
    timestamp_last_update_ar_state = xTaskGetTickCount();
    timestamp_last_update_ar_conf = xTaskGetTickCount();
    vTaskDelay(5000 / portTICK_PERIOD_MS);

    do
    {
        ESP_LOGI(AIR_REF_TAG, "asking for ar_conf");
        do_request_ar_conf();
        vTaskDelay(3000 / portTICK_PERIOD_MS);
    } while (conf_received == false);

    while (1)
    {

        if (sending_new_conf)
        {
            if (timestamp_last_update_ar_conf + 1000 / portTICK_PERIOD_MS < xTaskGetTickCount())
            {
                ESP_LOGI(AIR_REF_TAG, "SEND UPDATED CONF");
                timestamp_last_update_ar_conf = xTaskGetTickCount();
                send_new_conf(&ar_conf);
            }
        }

        else if (timestamp_last_update_m_state + 2000 / portTICK_PERIOD_MS < xTaskGetTickCount())
        {
            ESP_LOGI(AIR_REF_TAG, "asking for m_state");
            timestamp_last_update_m_state = xTaskGetTickCount();
            do_request_m_state();
        }
        else if (timestamp_last_update_ar_state + 10000 / portTICK_PERIOD_MS < xTaskGetTickCount())
        {
            ESP_LOGI(AIR_REF_TAG, "asking for ar_state state");
            (timestamp_last_update_ar_state = xTaskGetTickCount());
            do_request_ar_state();
        }
        vTaskDelay(800 / portTICK_PERIOD_MS);

        // if status is waiting for confirm (ar_conf update) for more than given time -> resend conf
    }
}

static void receiver_task(void *arg)
{
//TODO ADD USAGE OF HEAP MEMORY TO MANAGE MULTIPLE DEVICES CONNECTED

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
                //ADD TIMESTAMP TO PACKET
                access_message_buffer(reply.buffer);
                //TODO SAVE RECEIVED FRAME TO SSD (AFTER REPLY)
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

void log_ar_conf(air_ref_conf_t *ar_conf)
{

    ESP_LOGI("LOG_AR_CONF", "serial_control : %d", ar_conf->serial_control);
    ESP_LOGI("LOG_AR_CONF", "fan_coeff_P : %d", ar_conf->fan_coeff_P);
    ESP_LOGI("LOG_AR_CONF", "fan_target_pressure : %d", ar_conf->fan_target_pressure);
    ESP_LOGI("LOG_AR_CONF", "fan_coeff_offset : %d", ar_conf->fan_coeff_offset);
    ESP_LOGI("LOG_AR_CONF", "fan_min_pressure : %d", ar_conf->fan_min_pressure);
    ESP_LOGI("LOG_AR_CONF", "fan_max_pressure : %d", ar_conf->fan_max_pressure);
    ESP_LOGI("LOG_AR_CONF", "compressor_target_pressure : %d", ar_conf->compressor_target_pressure);
    ESP_LOGI("LOG_AR_CONF", "compressor_activation_offset : %d", ar_conf->compressor_activation_offset);
    ESP_LOGI("LOG_AR_CONF", "compressor_action_delay : %d", ar_conf->compressor_action_delay);
    ESP_LOGI("LOG_AR_CONF", "compressor_start_interval : %d", ar_conf->compressor_start_interval);

    for (int i = 0; i < 10; i++)
    {
        ESP_LOGI("LOG_AR_CONF", "compressor_speed(%d) : %d", i, ar_conf->compressor_speed[i]);
    }
}

void log_m_state(machine_state_t *m_state)
{

    ESP_LOGI("LOG_M_STATE", "evaporation_pressure : %d", m_state->evaporation_pressure);
    ESP_LOGI("LOG_AR_CONF", "condensation_pressure : %d", m_state->condensation_pressure);
    ESP_LOGI("LOG_AR_CONF", "temperature_gas_scarico : %d", m_state->temperature_gas_scarico);
    ESP_LOGI("LOG_AR_CONF", "temperature_environment : %d", m_state->temperature_environment);
    ESP_LOGI("LOG_AR_CONF", "temperature_gas_ritorno : %d", m_state->temperature_gas_ritorno);
    ESP_LOGI("LOG_AR_CONF", "temperature_extra : %d", m_state->temperature_extra);
}
