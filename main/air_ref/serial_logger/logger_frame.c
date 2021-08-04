#include "logger_hw.h"
#include "logger_frame.h"
#include "logger_air_ref.h"
//#include "../air_ref.h"
#include "airref_builder.h"

#define LOGGER_TAG "LOGGER"

extern machine_state_t m_state;
extern air_ref_conf_t ar_conf;
extern air_ref_conf_t ar_conf_old;
extern air_ref_state_t ar_state;

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

void parse_reply(logger_frame_t *reply, uint8_t *logger_buffer)
{
    reply->protocol_version = logger_buffer[2];
    reply->frame_size = (logger_buffer[3] | (logger_buffer[4] << 8)) & 0xffff;
    for (int i = 0; i < reply->frame_size; i++)
    {
        reply->buffer[i] = logger_buffer[HEADER_SIZE + i];
    }
    reply->checksum = (logger_buffer[HEADER_SIZE + reply->frame_size] | (logger_buffer[HEADER_SIZE + reply->frame_size + 1] << 8)) & 0xffff;
}

int8_t receive_reply(logger_frame_t *reply, uint8_t *data, int length)
{
    ESP_LOGI("LENGTH", "LEN:%d", length);
    if (length >= HEADER_SIZE + 2)
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






void build_frame(logger_frame_t *frame, flatcc_builder_t* builder )
{
    size_t size;
    void *buf;

    buf = flatcc_builder_finalize_buffer(builder, &size); 
    FRAME_AS_REQUEST( (frame) );
    frame->protocol_version = PROTOCOL_VERSION;

    memcpy(frame->buffer, buf, size);
    frame->frame_size = size;
    flatcc_builder_aligned_free(buf);

    //access_message_buffer(frame->buffer);
}

void send_frame(logger_frame_t *frame)
{
    uint8_t tmp_buffer_out[LOGGER_PACKET_SIZE];
    size_t size = frame->frame_size;
    tmp_buffer_out[0] = frame->start_of_frame[0];
    tmp_buffer_out[1] = frame->start_of_frame[1];
    tmp_buffer_out[2] = frame->protocol_version;
    tmp_buffer_out[3] = frame->frame_size & 0xff;
    tmp_buffer_out[4] = (frame->frame_size >> 8) & 0xff;

    for (int i=0; i< size ; i++){
        tmp_buffer_out[i+HEADER_SIZE] = frame->buffer[i];
    }

    uint16_t chksum = logger_checksum(tmp_buffer_out, HEADER_SIZE+size ); //4?
    tmp_buffer_out[HEADER_SIZE+size+0] = chksum & 0xff;
    tmp_buffer_out[HEADER_SIZE+size+1] = (chksum >> 8) & 0xff;
    tmp_buffer_out[HEADER_SIZE+size+2] = frame->end_of_frame[0];
    tmp_buffer_out[HEADER_SIZE+size+3] = frame->end_of_frame[1];
    //send
    //ESP_LOGI("receive", "message to send");  
    //ESP_LOG_BUFFER_HEX(LOGGER_TAG,tmp_buffer_out,5+size +4 );
    //uart_send
    uart_write_bytes(uart_num, (const char *)tmp_buffer_out, 5+size +4);
}



//ringbuffer management

// ringbuffer_enqueue (data, size)

// ringbuffer_dequeue (&frame)
//{ search for start_reply ... end_reply }
