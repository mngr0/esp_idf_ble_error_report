
#include "logger_hw.h"
#include "logger_air_ref.h"
#include "logger_frame.h"

machine_state_t m_state;
air_ref_conf_t ar_conf;
air_ref_conf_t ar_conf_old;
air_ref_state_t ar_state;

QueueHandle_t command_queue;
TaskHandle_t xLoggerTask;
uint32_t timestamp_last_update_m_state;
uint32_t timestamp_last_update_ar_state;
uint32_t timestamp_last_update_ar_conf;
extern const uart_port_t uart_num;

#define LOGGER_BUF_SIZE 1024

static void logger_task(void *arg)
{
    BaseType_t is_command = 0;
    timestamp_last_update_m_state = xTaskGetTickCount();
    timestamp_last_update_ar_state = xTaskGetTickCount();
    timestamp_last_update_ar_conf = xTaskGetTickCount();
    //TODO list of things to check: M_STATE AR_STATE
    //uint16_t expected_reply_size = 0;
    //request_command_t current_request;

    bool done = false;
    int length;
    int tot_length = 0;
    logger_frame_t reply;
    uint8_t data[LOGGER_BUF_SIZE * 4];

    int expected_len;

    while (1)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        length = uart_read_bytes(uart_num, data, LOGGER_BUF_SIZE, 20 / portTICK_RATE_MS);
        if (length > 0)
        {
            ESP_LOGI(LOGGER_TAG, "something received");
            ESP_LOG_BUFFER_HEX(LOGGER_TAG, data, length);
            if (receive_reply(&reply, data, length))
            {
                ESP_LOGI(LOGGER_TAG, "parse ok");
                access_message_buffer(reply.buffer);
            }
            else
            {
                ESP_LOGI(LOGGER_TAG, "parse not ok");
            }
        }
        else
        {
            ESP_LOGI(LOGGER_TAG, "nothing received");
        }
    }

    while (1)
    {

        logger_frame_t req;
        ESP_LOGI(LOGGER_TAG, "logging .... ");
        // check for command request in Q
        is_command = xQueueReceive(command_queue, (uint8_t *)&req, 0);
        if (is_command == pdFALSE)
        {

            flatcc_builder_t builder;
            void *buf;
            size_t size;

            //TODO load command
            if (timestamp_last_update_m_state + 10000 / portTICK_PERIOD_MS < xTaskGetTickCount())
            {
                ESP_LOGI(LOGGER_TAG, "asking for m_state");
                timestamp_last_update_m_state = xTaskGetTickCount();
                do_request_m_state(&req);
                //current_request = req.request_code;
                //expected_reply_size = 118; //sizeof(machine_state_t);
            }
            else if (timestamp_last_update_ar_state + 30000 / portTICK_PERIOD_MS < xTaskGetTickCount())
            {
                ESP_LOGI(LOGGER_TAG, "asking for ar_state state");
                (timestamp_last_update_ar_state = xTaskGetTickCount());
                do_request_ar_state(&req);
                //current_request = req.request_code;
                //expected_reply_size = sizeof(air_ref_state_t);
            }

            else if (timestamp_last_update_ar_conf + 50000 / portTICK_PERIOD_MS < xTaskGetTickCount())
            {
                ESP_LOGI(LOGGER_TAG, "asking for ar_conf");
                timestamp_last_update_ar_conf = xTaskGetTickCount();
                do_request_ar_state(&req);
                //current_request = req.request_code;
                //expected_reply_size =
            }
            else
            {
                ESP_LOGI(LOGGER_TAG, "delay .... ");
                vTaskDelay(5000 / portTICK_PERIOD_MS);

                continue;
            }
        }
        else
        {
            ESP_LOGI(LOGGER_TAG, "cmd received");
            //current_request = req.request_code;
            // if (req.request_code == read_routine_conf)
            // {
            //     timestamp_last_update_ar_conf = xTaskGetTickCount();
            //     // memcpy ar_conf_old <- ar_conf
            // }
        }
        ESP_LOGI(LOGGER_TAG, "communication tansaction");
        // if no command request, update state
        //send_request(&req);

        do
        {
            vTaskDelay(50 / portTICK_PERIOD_MS);
            //TODO manage buffer, receive separated
            length = uart_read_bytes(uart_num, data, LOGGER_BUF_SIZE, 20 / portTICK_RATE_MS);
            //manage start, end.
            if (length > 0)
            {
                //CHECK START OF PACKET , END OF PACKET AND SHIT LIKE THAT
                tot_length += length;
                ESP_LOGI(LOGGER_TAG, "received len = %d ", length);

                ESP_LOG_BUFFER_HEX("RECVED:", data, tot_length);
                if (receive_reply(&reply, data, length))
                {
                    ESP_LOGI(LOGGER_TAG, "receive success");
                    access_message_buffer(data);
                }
                else
                {
                    ESP_LOGI(LOGGER_TAG, "receive error");
                }
                done = true;
            }
        } while (done == false);

        ESP_LOGI(LOGGER_TAG, "sizeof error_report_t is %d", sizeof(error_report_t));
        ESP_LOGI(LOGGER_TAG, "sizeof air_ref_status_t is %d", sizeof(air_ref_status_t));
    }
}

void logger_init()
{
    //TODO prepare buffers

    xTaskCreate(logger_task, "logger_task", LOGGER_TASK_STACK_SIZE, NULL, 10, &(xLoggerTask));
    command_queue = xQueueCreate(5, 8);
}