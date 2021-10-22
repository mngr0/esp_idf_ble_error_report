#include "peripherals/uart.h"
#include "driver/uart.h"
#include "driver/gpio.h"

const uart_port_t uart_num = UART_NUM_2;
// status
#define BUF_SIZE 1024
#define ECHO_TEST_TXD (GPIO_NUM_17)
#define ECHO_TEST_RXD (GPIO_NUM_16)
#define ECHO_TEST_RTS (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS (UART_PIN_NO_CHANGE)


void configure_serial() {

  int intr_alloc_flags = 0;
  uart_config_t uart_config = {
      .baud_rate = 9600,
      .data_bits = UART_DATA_8_BITS,
      .parity = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
      .source_clk = UART_SCLK_APB,
  };

  ESP_ERROR_CHECK(uart_driver_install(uart_num, BUF_SIZE * 2, 0, 0, NULL,
                                      intr_alloc_flags));
  ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));
  ESP_ERROR_CHECK(uart_set_pin(uart_num, ECHO_TEST_TXD, ECHO_TEST_RXD,
                               UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
}


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
