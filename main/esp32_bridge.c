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

#include "termostatica.h"

#include "air_ref/logger.h"
#include "peripherals/i2c_devices.h"
#include "peripherals/led.h"
#include "peripherals/spi_sd.h"


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

  //READ THINGS FROM FLASH



  // init SD
  // sdmmc_card_t *card;
  // spi_sd_init(&card);

  // xTaskCreate(blink_task, "blink_task", SENDER_TASK_STACK_SIZE, NULL, 2,
  // NULL);

  // configure_serial();

  BLE_init();
  //logger_init();
}


//avviare lettura configurazione (e primo stato) (led giallo)
//se stato err -> avviare BLE senza servizi conf e status (led rosso)
//se stato reading -> provare a riconoscere device, avviare conf e status (led verde)

//se connesso a cell+dev -> blu , se solo cell -> viola

