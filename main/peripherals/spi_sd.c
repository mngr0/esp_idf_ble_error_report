#include "spi_sd.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include <bits/types/struct_timespec.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/unistd.h>

#include <dirent.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "utility/utility.h"

#define TAG "SD_SPI"

// scan_dir
// search more recent
int search_file(char *directory_to_scan, char *file_path, bool newest) {

  // IT MUST START WITH LOG
  ESP_LOGI(TAG, "STARTED SEARCH FILE in %s", directory_to_scan);
  DIR *d;
  struct dirent *dp;
  struct stat properties;

  time_t last_found;

  if ((d = opendir(directory_to_scan)) == NULL) {
    // fprintf(stderr, "Cannot open %s directory\n", directory_to_scan);
    return -1;
  }
  while ((dp = readdir(d)) != NULL) {

    if (strstr(dp->d_name, "LOG")) {

      char full_name[80];

      strcpy(full_name, directory_to_scan);
      strcat(full_name, dp->d_name);
      if (stat(full_name, &properties) < 0) {
        return -1;
      }
      // ESP_LOGI(TAG,
      //          "\t st_atime:%ld \n\t st_mtime:%ld \n\t st_ctime:%ld \n\t, "
      //          "st_size:%ld \n\t ",
      //          properties.st_atime, properties.st_mtime, properties.st_ctime,
      //          properties.st_size);

      if (newest) {
        if (properties.st_mtime > last_found) {
          last_found = properties.st_mtime;
          strcpy(file_path, full_name);
        }
      } else {
        if (properties.st_mtime < last_found) {
          last_found = properties.st_mtime;
          strcpy(file_path, full_name);
        }
      }
    }
  }
  closedir(d);
  return 0;
}

void example_get_fatfs_usage(uint64_t *out_total_bytes,
                             uint64_t *out_free_bytes) {
  FATFS *fs;
  size_t free_clusters;
  int res = f_getfree("0:", &free_clusters, &fs);
  assert(res == FR_OK);
  if (out_total_bytes != NULL) {
    *out_total_bytes = (uint64_t)(fs->n_fatent - 2) * fs->csize * fs->ssize;
  }
  if (out_free_bytes != NULL) {
    *out_free_bytes = (uint64_t)(free_clusters)*fs->csize * fs->ssize;
  }
}

void spi_sd_init(sdmmc_card_t **card) {
  esp_err_t ret;
  esp_vfs_fat_sdmmc_mount_config_t mount_config = {
      .format_if_mount_failed = true,
      .max_files = 5,
      .allocation_unit_size = 16 * 1024};

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
                                card);

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
}

int create_new_file(char *dir_path, char *new_file_name) {
  uint64_t out_total_bytes;
  uint64_t out_free_bytes;
  char tmp_str[MAX_FILENAME_SIZE];
  example_get_fatfs_usage(&out_total_bytes, &out_free_bytes);
  if (out_total_bytes < MAX_FILE_SIZE) {
    search_file(dir_path, tmp_str, false);
    remove(tmp_str);
  }
  strncpy(tmp_str, dir_path, MAX_FILENAME_SIZE);

  // FILE FIRST
  strncat(tmp_str, new_file_name, MAX_FILENAME_SIZE - strlen(dir_path));
  FILE *f;
  f = fopen(tmp_str, "w");
  if (f == NULL) {
    return -1;
  }

  // touch it

  // snprintf(tmp_str, MAX_FILENAME_SIZE, "LOG_2%3d%2d%2d%2d%2d.TXT",
  // time->year,
  //          time->month, time->day, time->hour, time->min);

  return 0;
}