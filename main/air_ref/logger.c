
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "serial_SATA_protocol/serial_SATA_protocol.h"

#include "air_ref.h"
#include "logger.h"
#include "packet_manager/packet_manager.h"

#include "driver/uart.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "i2c_devices/rtc/MCP7940/mcp7940.h"
#include "peripherals/i2c_devices.h"

#include "utility/utility.h"

#include "ble/services/gatt_handle.h"

#include "cJSON.h"

#define AIR_REF_TAG "LOGGER:"