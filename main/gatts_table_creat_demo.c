/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

/****************************************************************************
*
* This demo showcases creating a GATT database using a predefined attribute table.
* It acts as a GATT server and can send adv data, be connected by client.
* Run the gatt_client demo, the client demo will automatically connect to the gatt_server_service_table demo.
* Client demo will enable GATT server's notify after connection. The two devices will then exchange
* data.
*
****************************************************************************/

#include "string.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"

#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_main.h"

#include "esp_gatt_common_api.h"
#include "driver/gpio.h"
#include "driver/uart.h"

#include "air_ref/air_ref.h"
#include "air_ref/serial_logger.h"
#include "ble/ble.h"
#define GATTS_TABLE_TAG "GATTS_TABLE_DEMO"

#define BRIDGE_TASK_STACK_SIZE  (16384 / sizeof(portSTACK_TYPE))

const uart_port_t uart_num = UART_NUM_2;
//status
#define BUF_SIZE 1024
#define ECHO_TEST_TXD (GPIO_NUM_17)
#define ECHO_TEST_RXD (GPIO_NUM_16)
#define ECHO_TEST_RTS (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS (UART_PIN_NO_CHANGE)

#define BLINK_GPIO1 2
#define BLINK_GPIO2 15
#define BLINK_GPIO3 13
#define BTN_GPIO 3


static void configure_led(void)
{
    //ESP_LOGI(TAG, "Example configured to blink GPIO LED!");
    gpio_reset_pin(BLINK_GPIO1);
    gpio_reset_pin(BLINK_GPIO2);
    gpio_reset_pin(BLINK_GPIO3);
    gpio_reset_pin(BTN_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO1, GPIO_MODE_OUTPUT);
    gpio_set_direction(BLINK_GPIO2, GPIO_MODE_OUTPUT);
    gpio_set_direction(BLINK_GPIO3, GPIO_MODE_OUTPUT);
    gpio_set_direction(BTN_GPIO, GPIO_MODE_INPUT);
}


static void configure_serial(){
    
    int intr_alloc_flags = 0;
    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };


    ESP_ERROR_CHECK(uart_driver_install(uart_num, BUF_SIZE*2 , 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(uart_num, ECHO_TEST_TXD, ECHO_TEST_RXD, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));



}



static void  bridge_task(void *arg)
{
    uint8_t data[BUF_SIZE*4];

    
        //init
    //uint8_t last_btn_state = gpio_get_level(BTN_GPIO);
    logger_reply_t reply;
    machine_state_t m_state;
    int length = 50;
    while (1){
        
        length = uart_read_bytes(uart_num, data, BUF_SIZE, 20 / portTICK_RATE_MS);
        if (receive_reply(&reply,data,length)){
            memcpy(&m_state, reply.buffer, sizeof(machine_state_t));
            ESP_LOGI("RECVED:","HP=%d\tLP=%d\tTGS=%d\tTritorno=%d\tTenv=%d\tTc=%d\tExtC=%d\n",
                m_state.condensation_pressure,
                m_state.evaporation_pressure,
                m_state.temperature_gas_scarico,
                m_state.temperature_gas_ritorno,
                m_state.temperature_environment,
                m_state.temperature_extra,
                m_state.pin_enable);
        }
        // ESP_LOGI("BRIDGE TASK", "READ DATA: %d\t",length);
        // for (int i=0;i<length;i++){
        //     ESP_LOGI("BRIDGE TASK", "%d\t",data[i]);
        // }


        //update status
        //uint8_t new_btn_state = gpio_get_level(BTN_GPIO);
        // if(new_btn_state != last_btn_state){
        //     esp_ble_gatts_send_indicate(gatts_if, param->write.conn_id, heart_rate_handle_table[IDX_CHAR_VAL_A], sizeof(notify_data), notify_data, false);
        //     last_btn_state = new_btn_state;
        // }
        //consider if save on SD card
        //maybe check if data needs to be calncelled for being too old
        //os sleep
        vTaskDelay(500 / portTICK_PERIOD_MS);
        //os_sleep(500);
    }
}

void app_main(void)
{
    esp_err_t ret;

    /* Initialize NVS. */
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    configure_led();
    configure_serial();

    BLE_init();
    logger_init();
    xTaskCreate(bridge_task, "bridge_task", BRIDGE_TASK_STACK_SIZE, NULL, 10, NULL);
}
