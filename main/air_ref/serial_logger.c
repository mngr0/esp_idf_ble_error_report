
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_log.h"

#include "serial_logger.h"
//#include "esp_log.h"

machine_state_t m_state;
air_ref_conf_t ar_conf;
air_ref_state_t ar_state;

QueueHandle_t command_queue;
TaskHandle_t xLoggerTask;

uint16_t logger_checksum(uint8_t *data, uint16_t length)
{
    uint16_t chk_sum = 0;
    for (int i = 0; i < length; i = i + 2)
    {
        chk_sum -= ((uint16_t *)data)[i / 2];
    }
    if (length % 2 == 1)
    {
        chk_sum -= ((data[length - 1] << 8)) & 0xFFFF;
    }
    return chk_sum;
}

//TODO ACCEPT ALL REPLIES

void parse_reply(logger_reply_t *reply, uint8_t *logger_buffer)
{
    reply->protocol_version = logger_buffer[0];
    reply->frame_size = (logger_buffer[1] | (logger_buffer[2] << 8)) & 0xffff;
    reply->reply_code = logger_buffer[3];
    for (int i = 0; i < reply->frame_size; i++)
    {
        reply->buffer[i] = logger_buffer[HEADER_SIZE + i];
        //ESP_LOGI("BUFFER:","%02X", reply->buffer[i]);
    }
    reply->checksum = (logger_buffer[HEADER_SIZE + reply->frame_size] | (logger_buffer[HEADER_SIZE + reply->frame_size + 1] << 8)) & 0xffff;
}

int8_t receive_reply(logger_reply_t *reply, uint8_t *data, int length)
{
    if (length >= HEADER_SIZE + CHECKSUM_SIZE)
    {
        parse_reply(reply, data);
        ESP_LOGI("LENGTH", "LEN:%d, frame size: %d", length, reply->frame_size);
        int16_t chksum = logger_checksum((uint8_t *)data, reply->frame_size + HEADER_SIZE);
        if (chksum != reply->checksum)
        {
            ESP_LOGI("receive", "wrong chksum, expected: %2X, but found: %2X", chksum, reply->checksum);
        }
        //TODO check protocol version
        //TODO check length coerent with dataframe
        //TODO check checksum
        return 1;
    }
    return 0;
}

void send_request(logger_request_t *request)
{
    uint8_t tmp_buffer_out[LOGGER_PACKET_SIZE];
    tmp_buffer_out[0] = request->protocol_version;
    tmp_buffer_out[1] = request->frame_size & 0xff;
    tmp_buffer_out[2] = (request->frame_size >> 8) & 0xff;
    tmp_buffer_out[3] = request->request_code;
    //send
    xQueueSend(command_queue, tmp_buffer_out, 10); //TODO check size
    //busy wait for receive
}

void do_request_m_state(logger_request_t *req)
{

    req->protocol_version = PROTOCOL_VERSION;
    req->request_code = read_machine_state;
    req->frame_size = 0;
}

void do_request_ar_state(logger_request_t *req)
{

    req->protocol_version = PROTOCOL_VERSION;
    req->request_code = read_routine_state;
    req->frame_size = 0;
}

void do_request_ar_conf(logger_request_t *req)
{
    req->protocol_version = PROTOCOL_VERSION;
    req->request_code = read_routine_conf;
    req->frame_size = 0;
}

void do_overwrite_ar_conf(logger_request_t *req, air_ref_conf_t *new_ar_conf)
{
    req->protocol_version = PROTOCOL_VERSION;
    req->request_code = write_routine_conf;
    req->frame_size = sizeof(ar_conf);
    memcpy(req->buffer, (uint8_t *)new_ar_conf, sizeof(ar_conf));
}

//commands:
// send new ar_conf
// update ar_conf

static void logger_task(void *arg)
{
    BaseType_t is_command = 0;
    uint32_t timestamp_last_update_m_state = xTaskGetTickCount();
    uint32_t timestamp_last_update_ar_state = xTaskGetTickCount();
    //TODO list of things to check: M_STATE AR_STATE

    while (1)
    {
        logger_request_t req;

        // check for command request in Q
        is_command = xQueueReceive(command_queue, (uint8_t *)&req, 0);
        if (is_command == pdFALSE)
        {
            //TODO load command
            // memcpy((uint8_t*)&req, Q.pop(), sizeof(request_command_t))
            if (timestamp_last_update_m_state + 3000 < xTaskGetTickCount())
            {
                do_request_m_state(&req);
            }
            else if (timestamp_last_update_ar_state + 3000 < xTaskGetTickCount())
            {
                do_request_ar_state(&req);
            }
            else{
                vTaskDelay(500 / portTICK_PERIOD_MS);
                continue;
            }
        }
        // if no command request, update state
        send_request(&req);
    }
}

void logger_init()
{
    //TODO prepare buffers

    xTaskCreate(logger_task, "logger_task", LOGGER_TASK_STACK_SIZE, NULL, 10, &(xLoggerTask));
    //TODO pass task object
    command_queue = xQueueCreate(5, 8);
}