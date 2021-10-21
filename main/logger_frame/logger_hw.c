#include "logger_hw.h"
#include "driver/uart.h"
#include "esp_err.h"

extern const uart_port_t uart_num;

int send_buffer(uint8_t* data, int lenght){
    esp_err_t ret;
    ret=uart_write_bytes(uart_num,data,lenght);
    if (ret<0){
        return ret;
    }
     vTaskDelay(1 / portTICK_PERIOD_MS);// delay 1ms

    ret=uart_wait_tx_done(uart_num, 100 / portTICK_PERIOD_MS);
    if(ret != ESP_OK){
        return ret;
    }
    return 0;
}