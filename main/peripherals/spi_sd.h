#ifndef SPI_H_
#define SPI_H_


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

#endif