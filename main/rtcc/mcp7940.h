#ifndef MCP7940_H_INCLUDED
#define MCP7940_H_INCLUDED

#include <stdint.h>

#define MCP7940_DEFAULT_ADDRESS 0x6F

#define MCP7940_MODE_12H 1
#define MCP7940_MODE_24H 0

typedef struct {
    uint8_t sec;
    uint8_t min;
    uint8_t hour;
    uint8_t day;
    uint8_t wday;
    uint8_t month;
    uint8_t year;
} rtc_time_t;

int mcp7940_init(uint8_t cap_size, uint8_t battery_control);
int mcp7940_set_time( const rtc_time_t *pTime);
int mcp7940_get_time(rtc_time_t *pTime);


#endif



