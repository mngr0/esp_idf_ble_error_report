#include "spi_sd.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
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

void search_last(char *dir_path, char *last_used_file_path) {}

int8_t stat_compare_is_gt(char *name1, char *name2) {
  struct stat properties1;
  struct stat properties2;

  if (stat(name1, &properties1) < 0) {
    return -1;
  }

  ESP_LOGI(TAG,
           "\t%s st_atime:%ld \n\t st_mtime:%ld \n\t st_ctime:%ld \n\t, "
           "st_size:%ld \n\t ",
           name1, properties1.st_atime, properties1.st_mtime,
           properties1.st_ctime, properties1.st_size);

  if (stat(name2, &properties2) < 0) {
    return -1;
  }

  ESP_LOGI(TAG,
           "\t%s st_atime:%ld \n\t st_mtime:%ld \n\t st_ctime:%ld \n\t, "
           "st_size:%ld \n\t ",
           name2, properties2.st_atime, properties2.st_mtime,
           properties2.st_ctime, properties2.st_size);
  return properties1.st_mtime > properties2.st_mtime;
}

// scan_dir
// search more recent

// return code: -1: error / 0: not_found / 1: found
int search_file(
    const char *directory_to_scan, char *file_path,  int8_t (*is_gt)(char *name1,
                  char *name2)) { // if most_recent is false this will get the
                                  // least recent

  // IT MUST START WITH LOG
  ESP_LOGI(TAG, "STARTED SEARCH FILE in %s", directory_to_scan);
  DIR *d;
  struct dirent *dp;
  bool found = false;
  char memory_name[80];
  memset(memory_name, 0, 80);
  char full_name[80];
  int8_t ret_value;

  if ((d = opendir(directory_to_scan)) == NULL) {
    ESP_LOGE(TAG, "Cannot open %s directory", directory_to_scan);
    return -1;
  }
  while ((dp = readdir(d)) != NULL) {

    if (strstr(dp->d_name, "LOG")) {

      strcpy(full_name, directory_to_scan);
      strcat(full_name, dp->d_name);
      if(memory_name[0]==0){
        strcpy(memory_name, full_name);
      }
      else{
        ret_value = is_gt(full_name, memory_name);
        if(ret_value<0){
          found = false;
          break;
        }
        strcpy(memory_name, full_name);
      }
      found = true;
    }
  }
  closedir(d);
  if (found) {
    ESP_LOGI(TAG, "FOUND %s", memory_name);
      strcpy(file_path, memory_name);
    return 1;
  } else {
    ESP_LOGI(TAG, "NOT FOUND");
    return 0;
  }
}

// void example_get_fatfs_usage(uint64_t *out_total_bytes,
//                              uint64_t *out_free_bytes) {
//   FATFS *fs;
//   size_t free_clusters;
//   // int res = f_getfree("0:", &free_clusters, &fs);
//   // assert(res == FR_OK);
//   if (out_total_bytes != NULL) {
//     *out_total_bytes = (uint64_t)(fs->n_fatent - 2) * fs->csize * fs->ssize;
//   }
//   if (out_free_bytes != NULL) {
//     *out_free_bytes = (uint64_t)(free_clusters)*fs->csize * fs->ssize;
//   }
// }

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
  if (out_free_bytes < MAX_FILE_SIZE) {
    search_file(dir_path, tmp_str, false);
    remove(tmp_str);
  }
  strncpy(tmp_str, dir_path, MAX_FILENAME_SIZE);

  // FILE FIRST
  strncat(tmp_str, new_file_name, MAX_FILENAME_SIZE - strlen(dir_path));
  FILE *f;
  f = fopen(tmp_str, "w+");
  if (f == NULL) {
    return -1;
  }
  fclose(f);
  // touch it

  // snprintf(tmp_str, MAX_FILENAME_SIZE, "LOG_2%3d%2d%2d%2d%2d.TXT",
  // time->year,
  //          time->month, time->day, time->hour, time->min);

  return 0;
}

int create_new_file_raw(char *dir_path, char *new_file_name) {

  char tmp_str[MAX_FILENAME_SIZE];

  strncpy(tmp_str, dir_path, MAX_FILENAME_SIZE);
  strncat(tmp_str, new_file_name, MAX_FILENAME_SIZE - strlen(dir_path));
  FILE *f;
  ESP_LOGI(TAG, "CREATING FILE  %s", tmp_str);
  f = fopen(tmp_str, "w");
  if (f == NULL) {
    ESP_LOGI(TAG, "ERRORE IN CREAZIONE");
    perror("creazione:");
    return -1;
  }
  fprintf(f, "\n");
  fclose(f);
  return 0;
}