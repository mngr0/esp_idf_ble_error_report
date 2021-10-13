
#include <stdio.h>
#include "mcp7940.h"
#include "driver/i2c.h"
#include <stdint.h>
#include <stddef.h>

#define REG_SEC 0
#define REG_MIN 1
#define REG_HOUR 2
#define REG_DAY 4
#define REG_WDAY 3
#define REG_MONTH 5
#define REG_YEAR 6
#define SEG_TIME 0x01

#define MCP7940_ST 0x80
#define MCP7940_VBATEN 0x08


#define BCD2BIN(x) ((((x) >> 4) & 0xF) * 10 + ((x)&0xF))
#define BIN2BCD(x) ((((x) / 10) << 4) | ((x) % 10))

#define I2C_MASTER_TIMEOUT_MS       1000
#define I2C_MASTER_NUM              0

static esp_err_t __attribute__((unused)) i2c_master_read_slave( uint8_t addr ,uint8_t *data_rd, size_t size)
{
    if (size == 0) {
        return ESP_OK;
    }
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (addr << 1) | READ_BIT, ACK_CHECK_EN);
    if (size > 1) {
        i2c_master_read(cmd, data_rd, size - 1, ACK_VAL);
    }
    i2c_master_read_byte(cmd, data_rd + size - 1, NACK_VAL);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

/**
 * @brief Test code to write esp-i2c-slave
 *        Master device write data to slave(both esp32),
 *        the data will be stored in slave buffer.
 *        We can read them out from slave buffer.
 *
 * ___________________________________________________________________
 * | start | slave_addr + wr_bit + ack | write n bytes + ack  | stop |
 * --------|---------------------------|----------------------|------|
 *
 * @note cannot use master write slave on esp32c3 because there is only one i2c controller on esp32c3
 */
static esp_err_t __attribute__((unused)) i2c_master_write_slave( uint8_t addr ,uint8_t *data_wr, size_t size)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (addr << 1) | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write(cmd, data_wr, size, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}


static int BCD_to_int(char bcd, uint8_t tens_len)
{
    int r = 0;
    r += bcd & 0x0f;
    r += ((bcd >> 4) & ((1 << tens_len) - 1)) * 10;
    return r;
}

static uint8_t int_to_bcd(int val, uint8_t tens_len)
{
    char c = 0;
    int i;
    for (i = 0; i < 4; i++)
    {
        c |= (val % 10) & (1 << i);
    }
    for (i = 0; i < 4; i++)
    {
        c |= ((val / 10) & (1 << i)) << 4;
    }
    return c & ((1 << (tens_len + 4)) - 1);
}

/*----------------------------------------------------------------------------*/
/* Init_RTC                                                                   */
/*----------------------------------------------------------------------------*/
int mcp7940_init(uint8_t cap_size, uint8_t battery_control)
{
    int res;
    uint8_t val;


    if ((res = i2c_master_read_slave( REG_SEC, &val, 1)))
    {
        return res;
    }

    if (!(val & MCP7940_ST))
    {
        // if frozen, issue software reset
        val |= MCP7940_ST;
        if ((res = i2c_write_byte( REG_SEC, val,1)))
        {
            return res;
        }
    }

    if ((res = i2c_master_read_slave(REG_WDAY, val,1)))
    {
        return res;
    }




    if ((val & MCP7940_VBATEN)^(battery_control)) //both casted implicitly to bool
    {
        // if frozen, issue software reset
        val |= MCP7940_VBATEN;
        if ((res = i2c_write_byte( REG_WDAY, val,1)))
        {
            return res;
        }
    }
    return 0;
}

int mcp7940_set_time( const rtc_time_t *pTime)
{
    uint8_t tmp;
    if (pTime == NULL)
        return -1;

    tmp = int_to_bcd(pTime->sec, 3);
    i2c_write_byte(  REG_SEC, tmp ,1);
    tmp = int_to_bcd(pTime->min, 3);
    i2c_write_byte(  REG_MIN, tmp ,1);
    tmp = int_to_bcd(pTime->hour, 2);
    i2c_write_byte(  REG_HOUR, tmp  ,1);
    tmp = int_to_bcd(pTime->day, 2);
    i2c_write_byte(  REG_DAY, tmp ,1 );
    tmp = int_to_bcd(pTime->wday, 0);
    i2c_write_byte(  REG_WDAY, tmp  ,1);
    tmp = int_to_bcd(pTime->month, 1);
    i2c_write_byte(  REG_MONTH, tmp ,1 );
    tmp = int_to_bcd(pTime->year, 4);
    i2c_write_byte(  REG_YEAR, tmp  ,1);

    return 0;
}

int mcp7940_get_time( rtc_time_t *pTime)
{
    uint8_t tmp;
    if (pTime == NULL)
        return -1;
    i2c_master_read_slave(  REG_SEC, &tmp, sizeof(uint8_t));
    pTime->sec = BCD_to_int(tmp, 3);
    i2c_master_read_slave(  REG_MIN, &tmp, sizeof(uint8_t));
    pTime->min = BCD_to_int(tmp, 3);
    i2c_master_read_slave(  REG_HOUR, &tmp, sizeof(uint8_t));
    pTime->hour = BCD_to_int(tmp, 2);
    i2c_master_read_slave(  REG_DAY, &tmp, sizeof(uint8_t));
    pTime->day = BCD_to_int(tmp, 2);
    i2c_master_read_slave(  REG_WDAY, &tmp, sizeof(uint8_t));
    pTime->wday = BCD_to_int(tmp, 0);
    i2c_master_read_slave(  REG_MONTH, &tmp, sizeof(uint8_t));
    pTime->month = BCD_to_int(tmp, 1);
    i2c_master_read_slave(  REG_YEAR, &tmp, sizeof(uint8_t));
    pTime->year = BCD_to_int(tmp, 4);
    return 0;
}
