
#include "serial_logger.h"

#include "esp_log.h"

uint16_t logger_checksum(uint8_t* data, uint16_t length){
	uint16_t chk_sum=0;
	for (int i=0;i <length;i=i+2){
		chk_sum-=((uint16_t*)data)[i/2];
		
	}
	if (length % 2 == 1){
		chk_sum-=((data[length-1]<<8)) & 0xFFFF;
	}
	return chk_sum;
}

//TODO ACCEPT ALL REPLIES

void parse_reply(logger_reply_t* reply, uint8_t* logger_buffer){
	reply->protocol_version=logger_buffer[0];
	reply->frame_size = (logger_buffer[1] |(logger_buffer[2]<<8)) & 0xffff;
	reply->reply_code=logger_buffer[3];
	for (int i=0;i<reply->frame_size;i++){
		reply->buffer[i]=logger_buffer[HEADER_SIZE+i];
        //ESP_LOGI("BUFFER:","%02X", reply->buffer[i]);
	}
	reply->checksum = (logger_buffer[HEADER_SIZE+reply->frame_size] | (logger_buffer[HEADER_SIZE+reply->frame_size+1]<<8))  & 0xffff;
}

int8_t receive_reply(logger_reply_t* reply, uint8_t* data,int length){
    if(length>=HEADER_SIZE+CHECKSUM_SIZE){
        parse_reply(reply,data);
        ESP_LOGI("LENGTH", "LEN:%d, frame size: %d",length,reply->frame_size );
        int16_t chksum = logger_checksum((uint8_t*)data,reply->frame_size+HEADER_SIZE );
        if (chksum != reply->checksum ){
            ESP_LOGI("receive", "wrong chksum, expected: %2X, but found: %2X",chksum,reply->checksum );
        }
        //TODO check protocol version
        //TODO check length coerent with dataframe
        //TODO check checksum
        return 1;
    }
	return 0;
}

void send_request(logger_request_t *request){
    uint8_t tmp_buffer_out[LOGGER_PACKET_SIZE];
    tmp_buffer_out[0]=request->protocol_version;
	tmp_buffer_out[1]=request->frame_size & 0xff;
	tmp_buffer_out[2]=(request->frame_size >> 8 ) &0xff;
	tmp_buffer_out[3]=request->request_code;
    //send
    //busy wait for receive
}

void do_request_machine_state(){
    logger_request_t  req;
    req.protocol_version = PROTOCOL_VERSION;
    req.request_code = read_machine_state;
    req.frame_size = 0;
}

machine_state_t m_state;
air_ref_conf_t ar_conf;
air_ref_state_t ar_state;

machine_state_t* get_m_state(){
    return &m_state;
}

air_ref_conf_t* get_ar_conf(){
    return &ar_conf;
}


air_ref_state_t* get_ar_state(){
    return &ar_state;
}


//commands:
// send new ar_conf
// update ar_conf


static void  logger_task(void *arg)
{
    //TODO list of things to check: M_STATE AR_STATE 
    // initialize AR_CONF

    while (1){
        
    // check for command request in Q

    // if no command request, update state

        

        vTaskDelay(500 / portTICK_PERIOD_MS);

    }
}


void logger_init(){
    xTaskCreate(logger_task, "logger_task", ECHO_TASK_STACK_SIZE, NULL, 10, NULL); 
    //TODO pass task object
}