#include "logger_hw.h"
#include "logger_frame.h"

#include "esp_log.h"

#define LOGGER_TAG "LOGGER_FRAME"

logger_device_t io_logger;


uint16_t logger_checksum(uint8_t *data, uint16_t length)
{
    uint16_t chk_sum = 0;
    for (int i = 0; i < length - 1; i = i + 2)
    {
        chk_sum -= (data[i] & 0xFFFF) | ((data[i + 1] << 8) & 0xFFFF);
    }
    if (length % 2 == 1)
    {
        chk_sum -= ((data[length - 1] << 8)) & 0xFFFF;
    }
    return chk_sum & 0xFFFF;
}

void parse_reply(logger_frame_t *reply, uint8_t *logger_buffer)
{
    reply->protocol_version = logger_buffer[DELIMITER_SIZE];
    reply->frame_size = (logger_buffer[DELIMITER_SIZE+1] | (logger_buffer[DELIMITER_SIZE+2] << 8)) & 0xffff;
    for (int i = 0; i < reply->frame_size; i++)
    {
        reply->buffer[i] = logger_buffer[HEADER_SIZE + i];
    }
    reply->checksum = (logger_buffer[HEADER_SIZE + reply->frame_size] | (logger_buffer[HEADER_SIZE + reply->frame_size + 1] << 8)) & 0xffff;
}


int8_t receive_frame(logger_frame_t *reply, uint8_t *data, int length)
{
    if (length >= HEADER_SIZE + 2)
    {
        parse_reply(reply, data);
        int16_t chksum = logger_checksum((uint8_t *)data, reply->frame_size + HEADER_SIZE);
        if ((0xFFFF & chksum) != (0xFFFF & reply->checksum))
        {
            ESP_LOGI("receive", "wrong chksum, expected: %2X, but found: %2X", chksum, reply->checksum);
            return 0;
        }
        else
        {
            return 1;
        }
        //TODO check protocol version
        //TODO check length coerent with dataframe
        return 1;
    }
    return 0;
}

void build_frame(logger_frame_t *frame, flatcc_builder_t *builder)
{
    size_t size;
    void *buf;

    buf = flatcc_builder_finalize_buffer(builder, &size);
    DELIMITER_FRAME((frame));
    frame->protocol_version = PROTOCOL_VERSION;

    memcpy(frame->buffer, buf, size);
    frame->frame_size = size;
    flatcc_builder_aligned_free(buf);
}

static uint8_t tmp_buffer_out[LOGGER_BUF_SIZE];

void send_frame(logger_frame_t *frame)
{
    size_t size = frame->frame_size;
    tmp_buffer_out[0] = frame->start_of_frame[0];
    tmp_buffer_out[1] = frame->start_of_frame[1];
    tmp_buffer_out[2] = frame->start_of_frame[2];
    tmp_buffer_out[3] = frame->protocol_version;
    tmp_buffer_out[4] = frame->frame_size & 0xff;
    tmp_buffer_out[5] = (frame->frame_size >> 8) & 0xff;

    for (int i = 0; i < size; i++)
    {
        tmp_buffer_out[i + HEADER_SIZE] = frame->buffer[i];
    }

    uint16_t chksum = logger_checksum(tmp_buffer_out, HEADER_SIZE + size);
    tmp_buffer_out[HEADER_SIZE + size + 0] = chksum & 0xff;
    tmp_buffer_out[HEADER_SIZE + size + 1] = (chksum >> 8) & 0xff;
    tmp_buffer_out[HEADER_SIZE + size + 2] = frame->end_of_frame[0];
    tmp_buffer_out[HEADER_SIZE + size + 3] = frame->end_of_frame[1];
    tmp_buffer_out[HEADER_SIZE + size + 4] = frame->end_of_frame[2];

    send_buffer( tmp_buffer_out, HEADER_SIZE + size + DELIMITER_SIZE +2);

}


void init_logger(){
    io_logger.cnt_in_begin=0;
    io_logger.cnt_in_end=0;
}


void add_to_ringbuffer(uint8_t *new_buf, int16_t length)
{
    //CS BEGIN
    for (int i = 0; i < length; i++)
    {
        io_logger.buffer_in[LOGGER_SUM_MOD(i, io_logger.cnt_in_end)] = new_buf[i];
    }
    io_logger.cnt_in_end = LOGGER_SUM_MOD(io_logger.cnt_in_end , length);
    //ESP_LOGI("ADD_TO_RINGBUFFER", "cnt_in_begin: %d\t cnt_in_end: %d",io_logger.cnt_in_begin,io_logger.cnt_in_end);
    //CS END
}

int16_t take_frame(uint8_t *frame_buf)
{

    //CS BEGIN
    //search for SOF, if not found cancel any data in the buffer.
    //any data after SOF is valid, EOF might still be in travel.

    
    bool sof_found = false;
    bool eof_found = false;
    int cnt = io_logger.cnt_in_begin;
    int cnt_sof = 0;
    int cnt_eof = 0;
    size_t frame_size = -1;
    while ((LOGGER_SUM_MOD(cnt, 0) != io_logger.cnt_in_end) && (!sof_found))
    {
        if ((io_logger.buffer_in[LOGGER_SUM_MOD(cnt, 0)] == START_OF_FRAME) && (io_logger.buffer_in[LOGGER_SUM_MOD(cnt, 1)] == START_OF_FRAME) && (io_logger.buffer_in[LOGGER_SUM_MOD(cnt, 2)] == START_OF_FRAME)  )
        {
            sof_found = true;
        }
        else
        {
            cnt++;
            // INGORE ANYTHING BEFORE SOF
            io_logger.cnt_in_begin++;
        }
    }
    cnt_sof = LOGGER_SUM_MOD(cnt, 0);
    
    // after SOF found, if there are still data ( == SOF found) search for EOF
    // TODO check for unexpected SOF 
    while ((LOGGER_SUM_MOD(cnt, 0) != io_logger.cnt_in_end) && (!eof_found))
    {
        if ((io_logger.buffer_in[LOGGER_SUM_MOD(cnt, 0)] == END_OF_FRAME) && (io_logger.buffer_in[LOGGER_SUM_MOD(cnt, 1)] == END_OF_FRAME)&& (io_logger.buffer_in[LOGGER_SUM_MOD(cnt, 2)] == END_OF_FRAME)   )
        {
            eof_found = true;
        }
        else
        {
            cnt++;
        }
    }
    if ((eof_found) && (sof_found))
    {
        cnt_eof = LOGGER_SUM_MOD(cnt, DELIMITER_SIZE);
        frame_size = LOGGER_SUM_MOD(cnt_eof, 0 - cnt_sof);
        for (int i = 0; i < frame_size; i++)
        {
            frame_buf[i] = io_logger.buffer_in[LOGGER_SUM_MOD(i, cnt_sof)];
        }
        io_logger.cnt_in_begin = LOGGER_SUM_MOD(cnt_eof, 0);
    }
    //CS END

    return frame_size;
}
