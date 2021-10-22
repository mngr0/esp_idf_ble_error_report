/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

/****************************************************************************
 *
 * This demo showcases creating a GATT database using a predefined attribute
 *table. It acts as a GATT server and can send adv data, be connected by client.
 * Run the gatt_client demo, the client demo will automatically connect to the
 *gatt_server_service_table demo. Client demo will enable GATT server's notify
 *after connection. The two devices will then exchange data.
 *
 ****************************************************************************/

#include "string.h"


#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "nvs_flash.h"

#include "driver/gpio.h"
#include "driver/i2c.h"
#include "driver/uart.h"

#include "air_ref/air_ref.h"
#include "ble/ble.h"
#define GATTS_TABLE_TAG "GATTS_TABLE_DEMO"
#define TAG GATTS_TABLE_TAG

#define SENDER_TASK_STACK_SIZE (32800 / sizeof(portSTACK_TYPE))
#define TASK_SENDER_STACK_PRIORITY (tskIDLE_PRIORITY + 1)

/////////////// SPI SD /////////////////

#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include <string.h>
#include <sys/stat.h>
#include <sys/unistd.h>

#define PIN_NUM_MISO 2
#define PIN_NUM_MOSI 15
#define PIN_NUM_CLK 14
#define PIN_NUM_CS 13
#define MOUNT_POINT "/sdcard"
#define SPI_DMA_CHAN 1

///////// I2C RTCC ///////////
#include "i2c_ports/esp-idf/esp_idf_i2c_port.h"
#include "i2c_common/i2c_common.h"
#include "i2c_devices/rtc/MCP7940/mcp7940.h"

#include "peripherals/i2c_devices.h"

#define I2C_MASTER_SCL_IO 22 /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO 21 /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM                                                         \
  0 /*!< I2C master i2c port number, the number of i2c peripheral interfaces   \
       available will depend on the chip */
#define I2C_MASTER_FREQ_HZ 100000   /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS 1000


#include "utility/utility.h"
//////////////////////////

const uart_port_t uart_num = UART_NUM_2;
// status
#define BUF_SIZE 1024
#define ECHO_TEST_TXD (GPIO_NUM_17)
#define ECHO_TEST_RXD (GPIO_NUM_16)
#define ECHO_TEST_RTS (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS (UART_PIN_NO_CHANGE)

#define LED_R 33
#define LED_G 32
#define LED_B 25

// TODO SPI -> SD
// I2C -> RTCC
#define BTN_GPIO 3

static void configure_led(void) {
  gpio_reset_pin(LED_R);
  gpio_reset_pin(LED_G);
  gpio_reset_pin(LED_B);
  // gpio_reset_pin(BTN_GPIO);
  /* Set the GPIO as a push/pull output */
  gpio_set_direction(LED_R, GPIO_MODE_OUTPUT);
  gpio_set_direction(LED_G, GPIO_MODE_OUTPUT);
  gpio_set_direction(LED_B, GPIO_MODE_OUTPUT);
  gpio_set_level(LED_R, 1);
  gpio_set_level(LED_G, 1);
  gpio_set_level(LED_B, 1);
  //  gpio_set_direction(BTN_GPIO, GPIO_MODE_INPUT);
}

static void configure_serial() {

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

uint8_t led_blink[8][3] = {{0, 0, 0}, {0, 0, 1}, {0, 1, 1},
                           {0, 1, 0}, {1, 1, 0}, {1, 1, 1},
                           {1, 0, 1}, {1, 0, 0}

};

static esp_err_t i2c_master_init(void) {
  int i2c_master_port = I2C_MASTER_NUM;

  i2c_config_t conf = {
      .mode = I2C_MODE_MASTER,
      .sda_io_num = I2C_MASTER_SDA_IO,
      .scl_io_num = I2C_MASTER_SCL_IO,
      .sda_pullup_en = GPIO_PULLUP_ENABLE,
      .scl_pullup_en = GPIO_PULLUP_ENABLE,
      .master.clk_speed = I2C_MASTER_FREQ_HZ,
  };

  esp_err_t err = i2c_param_config(i2c_master_port, &conf);
  if (err != ESP_OK) {
    return err;
  }
  return i2c_driver_install(i2c_master_port, conf.mode,
                            I2C_MASTER_RX_BUF_DISABLE,
                            I2C_MASTER_TX_BUF_DISABLE, 0);
}


/////////////////////// UART


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

//////////////////////////////////


static void blink_task(void *arg) {
  rtc_time_t pTime;
  rtc_time_t pTime_set;
  pTime_set.year = 20;
  pTime_set.hour = 17;
  pTime_set.month = 10;
  pTime_set.day = 16;
  pTime_set.min = 20;

  if (i2c_master_init()) {
    ESP_LOGI("TEST_CS", "i2c INIT NOT GOOD ----------------------------");
  } else {
    ESP_LOGI("TEST_CS", "i2c INIT GOOD/////////////////////////");
  }
  //mcp7940_set_time(rtc_driver,&pTime_set);
  do_test_spi_sd();
  if (mcp7940_init(rtc_driver,1)) {
    ESP_LOGI("TEST_CS", "MCP INIT NOT GOOD ----------------------------");
  } else {
    ESP_LOGI("TEST_CS", "MCP INIT GOOD/////////////////////////");
  }
  // set time?
  uint8_t counter = 5; // ALL 1
  
  while (1) {
    // read RTCC. print time

    gpio_set_level(LED_R, led_blink[counter][0]);
    gpio_set_level(LED_G, led_blink[counter][1]);
    gpio_set_level(LED_B, led_blink[counter][2]);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    //counter++;
    counter %= 8;
    ESP_LOGI("TEST_CS", "doing: %d %d %d", led_blink[counter][0],
             led_blink[counter][1], led_blink[counter][2]);
    mcp7940_get_time(rtc_driver,&pTime);
    ESP_LOGI("TIME","%d/%d/%d - %d:%d:%d",pTime.day,pTime.month,pTime.year,pTime.hour, pTime.min,  pTime.sec);
  }
}

void app_main(void) {
  esp_err_t ret;

  /* Initialize NVS. */
  ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  configure_led();

  xTaskCreate(blink_task, "blink_task", SENDER_TASK_STACK_SIZE, NULL, 2, NULL);

  configure_serial();

  BLE_init();
  logger_init();
}
