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

#define MAX_FILENAME_SIZE 256

void search_last(char* dir_path, char* last_used_file_path);

int search_file(const char *directory_to_scan ,char *file_path, bool newest) ; //newest ... XOR oldest

void spi_sd_init(sdmmc_card_t **card);

int create_new_file(char* dir_path, char *new_file_name);

void example_get_fatfs_usage(uint64_t *out_total_bytes,
                                    uint64_t *out_free_bytes);

#endif