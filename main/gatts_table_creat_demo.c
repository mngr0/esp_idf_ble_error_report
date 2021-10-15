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

#include "esp_bt.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "nvs_flash.h"

#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"

#include "driver/gpio.h"
#include "driver/i2c.h"
#include "driver/uart.h"
#include "esp_gatt_common_api.h"

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

#include "rtcc/mcp7940.h"

#define I2C_MASTER_SCL_IO 22 /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO 21 /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM                                                         \
  0 /*!< I2C master i2c port number, the number of i2c peripheral interfaces   \
       available will depend on the chip */
#define I2C_MASTER_FREQ_HZ 10000    /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE 0 /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS 1000

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

void do_test_spi_sd() {
  esp_err_t ret;
  esp_vfs_fat_sdmmc_mount_config_t mount_config = {
      .format_if_mount_failed = true,
      .max_files = 5,
      .allocation_unit_size = 16 * 1024};
  sdmmc_card_t *card;

  const char mount_point[] = MOUNT_POINT;
  ESP_LOGI(TAG, "Initializing SD card");
  sdmmc_host_t host = SDSPI_HOST_DEFAULT();
  spi_bus_config_t bus_cfg = {
      .mosi_io_num = PIN_NUM_MOSI,
      .miso_io_num = PIN_NUM_MISO,
      .sclk_io_num = PIN_NUM_CLK,
      .quadwp_io_num = -1,
      .quadhd_io_num = -1,
      .max_transfer_sz = 4000,
  };
  ret = spi_bus_initialize(host.slot, &bus_cfg, SPI_DMA_CHAN);
  if (ret != ESP_OK) {
    ESP_LOGE(TAG, "Failed to initialize bus.");
    return;
  }

  // This initializes the slot without card detect (CD) and write protect (WP)
  // signals. Modify slot_config.gpio_cd and slot_config.gpio_wp if your board
  // has these signals.
  sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
  slot_config.gpio_cs = PIN_NUM_CS;
  slot_config.host_id = host.slot;

  ESP_LOGI(TAG, "Mounting filesystem");
  ret = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config,
                                &card);

  if (ret != ESP_OK) {
    if (ret == ESP_FAIL) {
      ESP_LOGE(TAG, "Failed to mount filesystem. "
                    "If you want the card to be formatted, set the "
                    "EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
    } else {
      ESP_LOGE(TAG,
               "Failed to initialize the card (%s). "
               "Make sure SD card lines have pull-up resistors in place.",
               esp_err_to_name(ret));
    }
    return;
  }
  ESP_LOGI(TAG, "Filesystem mounted");

  // Card has been initialized, print its properties
  sdmmc_card_print_info(stdout, card);

  // Use POSIX and C standard library functions to work with files.

  // First create a file.
  const char *file_hello = MOUNT_POINT "/hello.txt";

  ESP_LOGI(TAG, "Opening file %s", file_hello);
  FILE *f = fopen(file_hello, "w");
  if (f == NULL) {
    ESP_LOGE(TAG, "Failed to open file for writing");
    return;
  }
  fprintf(f, "Hello %s!\n", card->cid.name);
  fclose(f);
  ESP_LOGI(TAG, "File written");

  const char *file_foo = MOUNT_POINT "/foo.txt";

  // Check if destination file exists before renaming
  struct stat st;
  if (stat(file_foo, &st) == 0) {
    // Delete it if it exists
    unlink(file_foo);
  }

  // Rename original file
  ESP_LOGI(TAG, "Renaming file %s to %s", file_hello, file_foo);
  if (rename(file_hello, file_foo) != 0) {
    ESP_LOGE(TAG, "Rename failed");
    return;
  }

  // Open renamed file for reading
  ESP_LOGI(TAG, "Reading file %s", file_foo);
  f = fopen(file_foo, "r");
  if (f == NULL) {
    ESP_LOGE(TAG, "Failed to open file for reading");
    return;
  }

  // Read a line from file
  char line[64];
  fgets(line, sizeof(line), f);
  fclose(f);

  // Strip newline
  char *pos = strchr(line, '\n');
  if (pos) {
    *pos = '\0';
  }
  ESP_LOGI(TAG, "Read from file: '%s'", line);

  // All done, unmount partition and disable SPI peripheral
  esp_vfs_fat_sdcard_unmount(mount_point, card);
  ESP_LOGI(TAG, "Card unmounted");

  // deinitialize the bus after all devices are removed
  spi_bus_free(host.slot);
}

static void blink_task(void *arg) {
  if (i2c_master_init()) {
    ESP_LOGI("TEST_CS", "i2c INIT NOT GOOD ----------------------------");
  } else {
    ESP_LOGI("TEST_CS", "i2c INIT GOOD/////////////////////////");
  }

  // do_test_spi_sd();
  if (mcp7940_init(1)) {
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
    // counter++;
    counter %= 8;
    ESP_LOGI("TEST_CS", "doing: %d %d %d", led_blink[counter][0],
             led_blink[counter][1], led_blink[counter][2]);
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

  // configure_serial();

  // BLE_init();
  // logger_init();
}
