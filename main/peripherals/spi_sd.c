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

#define TAG "SD_SPI"

FILE *search_last() { return NULL; }

// void create_new_file(){

//     FILE *f = fopen(file_hello, "w");
// }

// scan_dir
// search more recent
int search_file() {
  DIR *d;
  struct dirent *dp;
 // int dfd;
  struct stat properties;

  if ((d = opendir("./tmp")) == NULL) {
    fprintf(stderr, "Cannot open ./tmp directory\n");
    return -1;
  }
  while ((dp = readdir(d)) != NULL) {
    ESP_LOGI(TAG, "name: %s", dp->d_name);
    // fopen??
    int f = open(dp->d_name, O_RDONLY);
    if (fstat(f, &properties) < 0) {
      return -1;
    }
    ESP_LOGI(TAG,
             "\t st_atime:%ld \n\t st_mtime:%ld \n\t st_ctime:%ld \n\t, "
             "st_size:%ld \n\t ",
             properties.st_atime, properties.st_mtime, properties.st_ctime,
             properties.st_size);
    close(f);
  }
  closedir(d);
  return 0;
}

// check_free_space

// append_data_to_file

// static void example_get_fatfs_usage(size_t *out_total_bytes,
//                                     size_t *out_free_bytes) {
//   FATFS *fs;
//   size_t free_clusters;
//   int res = f_getfree("0:", &free_clusters, &fs);
//   assert(res == FR_OK);
//   size_t total_sectors = (fs->n_fatent - 2) * fs->csize;
//   size_t free_sectors = free_clusters * fs->csize;

//   // assuming the total size is < 4GiB, should be true for SPI Flash
//   if (out_total_bytes != NULL) {
//     *out_total_bytes = total_sectors * fs->ssize;
//   }
//   if (out_free_bytes != NULL) {
//     *out_free_bytes = free_sectors * fs->ssize;
//   }
// }
