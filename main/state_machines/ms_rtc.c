#include <stdbool.h>
#include <sys/time.h>
#include <time.h>

#include "peripherals/i2c_devices.h"


#include "i2c_common/i2c_common.h"
#include "i2c_devices/rtc/MCP7940/mcp7940.h"
#include "i2c_ports/esp-idf/esp_idf_i2c_port.h"



  // if (i2c_master_init()) {
  //   ESP_LOGI("TEST_CS", "i2c INIT NOT GOOD ----------------------------");
  // } else {
  //   ESP_LOGI("TEST_CS", "i2c INIT GOOD/////////////////////////");
  // }
  // // mcp7940_set_time(rtc_driver,&pTime_set);

  // if (mcp7940_init(rtc_driver, 1)) {
  //   ESP_LOGI("TEST_CS", "MCP INIT NOT GOOD ----------------------------");
  // } else {
  //   ESP_LOGI("TEST_CS", "MCP INIT GOOD/////////////////////////");
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