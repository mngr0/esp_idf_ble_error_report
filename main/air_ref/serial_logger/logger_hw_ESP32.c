
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


int access_message_buffer(const void *buffer)
{
	// Note that we use the `table_t` suffix when reading a table object
	// as opposed to the `ref_t` suffix used during the construction of
	// the buffer.

	AirRef_Message_table_t message = AirRef_Message_as_root(buffer);
	test_assert(message != 0);
	ESP_LOGI("DECODED", "assert ok");
	if (AirRef_Message_content_type(message) == AirRef_Content_MachineState)
	{
		//update local machine state
		ESP_LOGI("DECODED", "right content type");
		// Note: root object pointers are NOT the same as the `buffer` pointer.

		// Make sure the buffer is accessible.

		AirRef_MachineState_table_t machineState = (AirRef_MachineState_table_t)AirRef_Message_content(message);

		parse_m_state(machineState, &m_state);

		ESP_LOGI("DECODED", "evaporation_pressure:%d\t", m_state.evaporation_pressure);
		ESP_LOGI("DECODED", "condensation_pressure:%d\t", m_state.condensation_pressure);
		ESP_LOGI("DECODED", "temperature_gas_scarico:%d\t", m_state.temperature_gas_scarico);
		ESP_LOGI("DECODED", "temperature_environment:%d\t", m_state.temperature_environment);
		ESP_LOGI("DECODED", "temperature_gas_ritorno:%d\t", m_state.temperature_gas_ritorno);
		ESP_LOGI("DECODED", "temperature_extra:%d\n", m_state.temperature_extra);
	}
	else if (AirRef_Message_content_type(message) == AirRef_Content_AirRefConf)
	{
        AirRef_AirRefConf_table_t airRefConf = (AirRef_AirRefConf_table_t)AirRef_Message_content(message);
        parse_ar_conf(airRefConf, &ar_conf);
		//update local airrefconf
	}
	else if (AirRef_Message_content_type(message) == AirRef_Content_AirRefState)
	{
		//update local airrefstate
	}
	else if (AirRef_Message_content_type(message) == AirRef_Content_Request)
	{
		AirRef_Request_table_t request = (AirRef_Request_table_t)AirRef_Message_content(message);

		//if ()
		//if request is air_ref_conf_received -> reset state to idle
	}
	return 0;
}


void routine_send_new_conf(air_ref_conf_t *ar_conf_new){
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

    bool done = false;
    int length;
    int tot_length = 0;
    logger_frame_t reply;
    uint8_t data[LOGGER_BUF_SIZE * 4];


    while (1)
    {

        logger_frame_t req;
        ESP_LOGI(LOGGER_TAG, "logging .... ");

        flatcc_builder_t builder;
        void *buf;
        size_t size;

        if (timestamp_last_update_m_state + 5000 / portTICK_PERIOD_MS < xTaskGetTickCount())
        {
            ESP_LOGI(LOGGER_TAG, "asking for m_state");
            timestamp_last_update_m_state = xTaskGetTickCount();
            do_request_m_state(&req);
        }
        // else if (timestamp_last_update_ar_state + 30000 / portTICK_PERIOD_MS < xTaskGetTickCount())
        // {
        //     ESP_LOGI(LOGGER_TAG, "asking for ar_state state");
        //     (timestamp_last_update_ar_state = xTaskGetTickCount());
        //     do_request_ar_state(&req);
        // }

        // else if (timestamp_last_update_ar_conf + 50000 / portTICK_PERIOD_MS < xTaskGetTickCount())
        // {
        //     ESP_LOGI(LOGGER_TAG, "asking for ar_conf");
        //     timestamp_last_update_ar_conf = xTaskGetTickCount();
        //     do_request_ar_conf(&req);
        // }
        else
        {
            ESP_LOGI(LOGGER_TAG, "delay .... ");
            vTaskDelay(5000 / portTICK_PERIOD_MS);
           // continue;
        }
        // if status is waiting for confirm (ar_conf update) for more than given time -> resend conf
    }
}

static void receiver_task(void *arg){


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
            //ESP_LOG_BUFFER_HEX(LOGGER_TAG, data, length);
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

}


void logger_init()
{

    xTaskCreate(logger_task, "logger_task", LOGGER_TASK_STACK_SIZE, NULL, 10, &(xLoggerTask));
    xTaskCreate(receiver_task, "logger_task", LOGGER_TASK_STACK_SIZE, NULL, 10, &(xLoggerTask));


    command_queue = xQueueCreate(5, 8);
}