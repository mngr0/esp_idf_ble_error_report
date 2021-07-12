
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "driver/uart.h"

#include "serial_logger.h"


#include "air_ref.h"
#include "airref_builder.h"

#define LOGGER_TAG "LOGGER"

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

uint16_t logger_checksum(uint8_t *data, uint16_t length)
{
    uint16_t chk_sum = 0;
    for (int i = 0; i < length - 1; i = i + 2)
    {
        chk_sum -= (data[i] & 0xFFFF) | ((data[i + 1] << 8) & 0xFFFF);
        //ESP_LOGI("CHKSUM", "chk_sum: %4x", chk_sum);
        //ESP_LOGI("CHKSUM", "combined: %4x (made of %4x and %4x)",(data[i] & 0xFFFF) | ( (data[i + 1] << 8) & 0xFFFF ),   (data[i] & 0xFFFF) , ( (data[i + 1] << 8) & 0xFFFF ));
    }
    if (length % 2 == 1)
    {
        chk_sum -= ((data[length - 1] << 8)) & 0xFFFF;
    }
    return chk_sum & 0xFFFF;
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
    ESP_LOGI("LENGTH", "LEN:%d", length);
    if (length >= HEADER_SIZE + CHECKSUM_SIZE)
    {
        parse_reply(reply, data);
        ESP_LOGI("LENGTH", " frame size: %d", reply->frame_size);
        int16_t chksum = logger_checksum((uint8_t *)data, reply->frame_size + HEADER_SIZE);
        if ((0xFFFF & chksum) != (0xFFFF & reply->checksum))
        {
            ESP_LOGI("receive", "wrong chksum, expected: %2X, but found: %2X", chksum, reply->checksum);
        }
        else
        {
            ESP_LOGI("receive", "correct chksum: %2X", chksum);
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
    uint16_t chksum = logger_checksum(tmp_buffer_out, 4);
    tmp_buffer_out[4] = chksum & 0xff;
    tmp_buffer_out[5] = (chksum >> 8) & 0xff;
    //send
    ESP_LOGI(LOGGER_TAG, "sending %2x %2x %2x %2x %2x %2x ", tmp_buffer_out[0], tmp_buffer_out[1], tmp_buffer_out[2], tmp_buffer_out[3], tmp_buffer_out[4], tmp_buffer_out[5]);
    //uart_send
    uart_write_bytes(uart_num, (const char *)tmp_buffer_out, 6);

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

typedef enum
{
    upload_result_nothing_to_do = 0,
    upload_result_correct,
    upload_result_timeout
} upload_result_t;

upload_result_t upload_configuration(air_ref_conf_t *new_ar_conf)
{
    upload_result_t result = upload_result_nothing_to_do;
    logger_request_t req_upload;
    logger_request_t req_reload;
    do_overwrite_ar_conf(&req_upload, new_ar_conf);
    do_request_ar_conf(&req_reload);

    while (0) //memcmp((uint8_t*)&ar_conf_old, (uint8_t*)&new_ar_conf, sizeof(air_ref_conf_t)) != 0)
    {
        result = upload_result_correct;
        uint32_t prev_time = timestamp_last_update_ar_conf;
        xQueueSend(command_queue, &req_upload, sizeof(request_command_t));
        xQueueSend(command_queue, &req_reload, sizeof(request_command_t));

        while (timestamp_last_update_ar_conf == prev_time)
        {
            vTaskDelay(50 / portTICK_PERIOD_MS);
        }
    }
    //end while
    return result;
}

//commands:
// send new ar_conf
// update ar_conf

static void logger_task(void *arg)
{
    BaseType_t is_command = 0;
    timestamp_last_update_m_state = xTaskGetTickCount();
    timestamp_last_update_ar_state = xTaskGetTickCount();
    timestamp_last_update_ar_conf = xTaskGetTickCount();
    //TODO list of things to check: M_STATE AR_STATE
    uint16_t expected_reply_size = 0;
    request_command_t current_request;

    bool done = false;
    int length;
    int tot_length = 0;
    logger_reply_t reply;
    uint8_t data[LOGGER_BUF_SIZE * 4];

    int expected_len;

    while (1)
    {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        length = uart_read_bytes(uart_num, data, LOGGER_BUF_SIZE, 20 / portTICK_RATE_MS);
        if (length > 0)
        {
            ESP_LOGI(LOGGER_TAG, "something received");
            ESP_LOG_BUFFER_HEX(LOGGER_TAG,data,length);
            if (receive_reply(&reply, data, length)){
                 ESP_LOGI(LOGGER_TAG, "parse ok");
                    access_message_buffer(reply.buffer);

            }
            else{
                ESP_LOGI(LOGGER_TAG, "parse nok");
            }
        }
        else{
            ESP_LOGI(LOGGER_TAG, "nothing received");
        }
    }

    while (1)
    {

        logger_request_t req;
        ESP_LOGI(LOGGER_TAG, "logging .... ");
        // check for command request in Q
        is_command = xQueueReceive(command_queue, (uint8_t *)&req, 0);
        if (is_command == pdFALSE)
        {

            flatcc_builder_t builder;
            void *buf;
            size_t size;

            flatcc_builder_init(&builder);
            send_m_state(&builder, &m_state);
            buf = flatcc_builder_finalize_buffer(&builder, &size);

            logger_request_t request;
            request.protocol_version = PROTOCOL_VERSION;
            request.request_code = read_machine_state;

            memcpy(request.buffer, buf, size);
            request.frame_size = size;
            uint16_t chk_sum = logger_checksum((uint8_t *)&request, HEADER_SIZE + size);
            request.checksum = chk_sum;
            //serialize(&reply,(uint8_t*)m_state,sizeof(machine_state_t));
            //reply.checksum = logger_checksum((uint8_t*)(&reply),HEADER_SIZE+sizeof(machine_state_t));
            send_request(&request);

            flatcc_builder_aligned_free(buf);
            //TODO load command
            if (timestamp_last_update_m_state + 10000 / portTICK_PERIOD_MS < xTaskGetTickCount())
            {
                ESP_LOGI(LOGGER_TAG, "asking for m_state");
                timestamp_last_update_m_state = xTaskGetTickCount();
                do_request_m_state(&req);
                current_request = req.request_code;
                expected_reply_size = 118; //sizeof(machine_state_t);
            }
            // else if (timestamp_last_update_ar_state + 30000 / portTICK_PERIOD_MS < xTaskGetTickCount())
            // {
            //     ESP_LOGI(LOGGER_TAG, "asking for ar_state state");
            //     (timestamp_last_update_ar_state = xTaskGetTickCount());
            //     do_request_ar_state(&req);
            //current_request = req.request_code;
            //     //expected_reply_size = sizeof(air_ref_state_t);
            // }

            // else if (timestamp_last_update_ar_conf + 50000 / portTICK_PERIOD_MS < xTaskGetTickCount())
            // {
            //     ESP_LOGI(LOGGER_TAG, "asking for ar_conf");
            //     timestamp_last_update_ar_conf = xTaskGetTickCount();
            //     do_request_ar_state(&req);
            //current_request = req.request_code;
            //     //expected_reply_size =
            // }
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
            current_request = req.request_code;
            if (req.request_code == read_routine_conf)
            {
                timestamp_last_update_ar_conf = xTaskGetTickCount();
                // memcpy ar_conf_old <- ar_conf
            }
        }
        ESP_LOGI(LOGGER_TAG, "communication tansaction");
        // if no command request, update state
        send_request(&req);

        do
        {
            //TODO manage buffer
            length = uart_read_bytes(uart_num, data, LOGGER_BUF_SIZE, 20 / portTICK_RATE_MS);
            if (length > 0)
            {
                access_message_buffer(data);
                tot_length += length;
                ESP_LOGI(LOGGER_TAG, "received len = %d (expected %d )", length, expected_reply_size);
                if (tot_length == expected_reply_size)
                {

                    ESP_LOG_BUFFER_HEX("RECVED:", data, tot_length);
                    if (receive_reply(&reply, data, length))
                    {
                        ESP_LOGI(LOGGER_TAG, "receive success");
                    }
                    else
                    {
                        ESP_LOGI(LOGGER_TAG, "receive error");
                    }
                    done = true;
                }
                else
                {
                    ESP_LOGI(LOGGER_TAG, "wrong size error");
                    vTaskDelay(50 / portTICK_PERIOD_MS);
                }
            }
        } while (done == false);

        if (current_request == read_machine_state)
        {
            ESP_LOGI(LOGGER_TAG, "correctly received %d bytes. size is %d", expected_reply_size, sizeof(machine_state_t));
            ESP_LOGI(LOGGER_TAG, "sizeof error_report_t is %d", sizeof(error_report_t));
            ESP_LOGI(LOGGER_TAG, "sizeof air_ref_status_t is %d", sizeof(air_ref_status_t));
        }
    }
}

void logger_init()
{
    //TODO prepare buffers

    xTaskCreate(logger_task, "logger_task", LOGGER_TASK_STACK_SIZE, NULL, 10, &(xLoggerTask));
    command_queue = xQueueCreate(5, 8);
}