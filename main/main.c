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

#include "esp_log.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include <string.h>

#include "driver/gpio.h"

#include "driver/uart.h"

#include "air_ref/logger.h"
#include "peripherals/i2c_devices.h"
#include "peripherals/led.h"
#include "peripherals/spi_sd.h"
#include "peripherals/uart.h"

#include <sys/time.h>
#include <time.h>

#include "ble/ble.h"
#define GATTS_TABLE_TAG "GATTS_TABLE_DEMO"
#define TAG GATTS_TABLE_TAG

#define SENDER_TASK_STACK_SIZE (32800 / sizeof(portSTACK_TYPE))
#define TASK_SENDER_STACK_PRIORITY (tskIDLE_PRIORITY + 1)

///////// I2C RTCC ///////////
#include "i2c_common/i2c_common.h"
#include "i2c_devices/rtc/MCP7940/mcp7940.h"
#include "i2c_ports/esp-idf/esp_idf_i2c_port.h"

#include "cJSON.h"

#include "utility/utility.h"

uint8_t led_blink[8][3] = {{0, 0, 0}, {0, 0, 1}, {0, 1, 1},
                           {0, 1, 0}, {1, 1, 0}, {1, 1, 1},
                           {1, 0, 1}, {1, 0, 0}

};

// static void blink_task(void *arg) {
//   rtc_time_t pTime;
//   // rtc_time_t pTime_set;
//   // pTime_set.year = 20;
//   // pTime_set.hour = 17;
//   // pTime_set.month = 10;
//   // pTime_set.day = 16;
//   // pTime_set.min = 20;

//   // set time?
//   uint8_t counter = 5; // ALL 1

//   while (1) {
//     // read RTCC. print time

//     gpio_set_level(LED_R, led_blink[counter][0]);
//     gpio_set_level(LED_G, led_blink[counter][1]);
//     gpio_set_level(LED_B, led_blink[counter][2]);
//     vTaskDelay(500 / portTICK_PERIOD_MS);
//     // counter++;
//     counter %= 8;
//     // ESP_LOGI("TEST_CS", "doing: %d %d %d", led_blink[counter][0],
//     //         led_blink[counter][1], led_blink[counter][2]);
//     mcp7940_get_time(rtc_driver, &pTime);
//     // ESP_LOGI("TIME","%d/%d/%d -
//     // %d:%d:%d",pTime.day,pTime.month,pTime.year,pTime.hour, pTime.min,
//     // pTime.sec);
//   }
// }

void prepare_time(bool tc_works) {
  // init time
  rtc_time_t time_read;
  struct timeval tv;
  struct timezone tz;
  struct tm ciao;

  mcp7940_get_time(rtc_driver, &time_read);

  ciao.tm_hour = time_read.hour;

  ciao.tm_sec= time_read.sec;   /* Seconds (0-60) */
  ciao.tm_min= time_read.min;   /* Minutes (0-59) */
  ciao.tm_hour= time_read.hour;  /* Hours (0-23) */
  ciao.tm_mday= time_read.day;  /* Day of the month (1-31) */
  ciao.tm_mon= time_read.month;   /* Month (0-11) */
  ciao.tm_year= time_read.year+2000-1900;  /* Year - 1900 */
  ciao.tm_wday= time_read.wday;  /* Day of the week (0-6, Sunday = 0) */
  //ciao.tm_yday= time_read;  /* Day in the year (0-365, 1 Jan = 0) */
  //ciao.tm_isdst= time_read.; /* Daylight saving time */
  tv.tv_sec=mktime(&ciao);
  settimeofday(&tv, &tz);

  //if rtc does not work
  //open last file
  //search for last timestamp
  //start from here
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

  // init SD
  sdmmc_card_t *card;
  spi_sd_init(&card);

  if (i2c_master_init()) {
    ESP_LOGI("TEST_CS", "i2c INIT NOT GOOD ----------------------------");
  } else {
    ESP_LOGI("TEST_CS", "i2c INIT GOOD/////////////////////////");
  }
  // mcp7940_set_time(rtc_driver,&pTime_set);

  if (mcp7940_init(rtc_driver, 1)) {
    ESP_LOGI("TEST_CS", "MCP INIT NOT GOOD ----------------------------");
  } else {
    ESP_LOGI("TEST_CS", "MCP INIT GOOD/////////////////////////");
  }

  // xTaskCreate(blink_task, "blink_task", SENDER_TASK_STACK_SIZE, NULL, 2,
  // NULL);

  configure_serial();

  BLE_init();
  logger_init();
}
