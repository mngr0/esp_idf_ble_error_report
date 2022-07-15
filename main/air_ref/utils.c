#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include "esp_log.h"
#include "logger.h"

#include "peripherals/i2c_devices.h"
#include "peripherals/spi_sd.h"

#include "i2c_ports/esp-idf/esp_idf_i2c_port.h"
#include "i2c_common/i2c_common.h"
#include "i2c_devices/rtc/MCP7940/mcp7940.h"

#include "utility/utility.h"

void save_json_to_sd(char* json_string){
   rtc_time_t pTime;
  mcp7940_get_time(rtc_driver,&pTime);
  log_received_messageHR(&pTime, json_string);
}








