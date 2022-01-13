

#include "utility/utility.h"

#include "esp_vfs_fat.h"
#include "i2c_common/i2c_common.h"
#include "sdmmc_cmd.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/unistd.h>

#include "peripherals/spi_sd.h"

#define TAG "UTILITY"

#define MAX_FILENAME_SIZE 256

char last_used_file_path[256] = {
    0, 0}; // after first search it points to last file
const char *dir_path = MOUNT_POINT "/PROVA/";

int log_received_message(rtc_time_t *time, packet_received_t *packet) {
  // search for valid file

  if (last_used_file_path[0] == 0) { // if first time log, go search for last used
    // check equals to string not empty
    search_file(dir_path, last_used_file_path, true);//if most_recent is false this will get the least recent
  }

  if (last_used_file_path[0] != 0) { 
    // if something found, check if size is good
    struct stat properties;

    if (stat(last_used_file_path, &properties) < 0) {
      // HOW TO MANAGE THIS ERROR?
    }
    if (properties.st_size > MAX_FILE_SIZE) {
      last_used_file_path[0] = 0;
    }
  }

  if (last_used_file_path[0] != 0) { // if no valid file exists, must create one
    char new_file_name[MAX_FILENAME_SIZE];
    snprintf(new_file_name, MAX_FILENAME_SIZE, "LOG_2%3d%2d%2d%2d%2d.TXT",
             time->year, time->month, time->day, time->hour, time->min);
    create_new_file((char *)dir_path, new_file_name);
  }
  FILE *f;
  // now last_used_file_path is a valid filepath
  f = fopen(last_used_file_path, "a");
  if (f == NULL) {
    return -1;
  }

  uint8_t separator[] = SEPARATOR_DEF;
  fwrite((uint8_t *)separator, sizeof(uint8_t), sizeof(separator), f);
  fwrite((uint8_t *)time, sizeof(uint8_t), sizeof(rtc_time_t), f);
  fwrite((uint8_t *)packet, sizeof(uint8_t), HEADER_SIZE, f);
  fwrite((uint8_t *)(&packet[HEADER_SIZE]), sizeof(uint8_t), packet->frame_size,
         f);
  fwrite((uint8_t *)(&packet[sizeof(packet_received_t) - FOOTER_SIZE]),
         sizeof(uint8_t), FOOTER_SIZE, f);
  fclose(f);
  // TODO consider if putting the structure sizes on a separate file
  return 0;
}


int log_received_messageHR(rtc_time_t *time, const char *message) {
  // search for valid file

  if (last_used_file_path[0] == 0) { // if first time log, go search for last used
    // check equals to string not empty
    search_file(dir_path, last_used_file_path, true);//if most_recent is false this will get the least recent
  }

  if (last_used_file_path[0] != 0) { 
    // if something found, check if size is good
    struct stat properties;

    if (stat(last_used_file_path, &properties) < 0) {
      // HOW TO MANAGE THIS ERROR?
    }
    if (properties.st_size > MAX_FILE_SIZE) {
      last_used_file_path[0] = 0;
    }
  }

  if (last_used_file_path[0] != 0) { // if no valid file exists, must create one
    char new_file_name[MAX_FILENAME_SIZE];
    snprintf(new_file_name, MAX_FILENAME_SIZE, "LOG_2%3d%2d%2d%2d%2d.TXT",
             time->year, time->month, time->day, time->hour, time->min);
    create_new_file((char *)dir_path, new_file_name);
  }
  FILE *f;
  // now last_used_file_path is a valid filepath
  f = fopen(last_used_file_path, "a");
  if (f == NULL) {
    return -1;
  }

  //
  fprintf(f, "[2%3d/%2d/%2d-%2d:%2d]:", time->year, time->month, time->day, time->hour, time->min);
  fprintf(f, message);
  fprintf(f, "\n");
  fclose(f);
  // TODO consider if putting the structure sizes on a separate file
  return 0;
}



// void do_test_spi_sd() {
//   // esp_err_t ret;
//   sdmmc_card_t *card;
//   const char mount_point[] = MOUNT_POINT;
//   sdmmc_host_t host = SDSPI_HOST_DEFAULT();
//   spi_sd_init(&card);

//   // Card has been initialized, print its properties
//   sdmmc_card_print_info(stdout, card);

//   uint64_t out_total_bytes;
//   uint64_t out_free_bytes;
//   example_get_fatfs_usage(&out_total_bytes, &out_free_bytes);
//   // Use POSIX and C standard library functions to work with files.
//   // First create a file.
//   const char *file_hello = MOUNT_POINT "/hello.txt";

//   ESP_LOGI(TAG, "Opening file %s", file_hello);
//   FILE *f = fopen(file_hello, "w");
//   if (f == NULL) {
//     ESP_LOGE(TAG, "Failed to open file for writing");
//     return;
//   }
//   fprintf(f, "Hello %s!\n", card->cid.name);
//   fclose(f);
//   ESP_LOGI(TAG, "File written");

//   const char *file_foo = MOUNT_POINT "/foo.txt";

//   // Check if destination file exists before renaming
//   struct stat st;
//   if (stat(file_foo, &st) == 0) {
//     // Delete it if it exists
//     unlink(file_foo);
//   }

//   // Rename original file
//   ESP_LOGI(TAG, "Renaming file %s to %s", file_hello, file_foo);
//   if (rename(file_hello, file_foo) != 0) {
//     ESP_LOGE(TAG, "Rename failed");
//     return;
//   }

//   // Open renamed file for reading
//   ESP_LOGI(TAG, "Reading file %s", file_foo);
//   f = fopen(file_foo, "r");
//   if (f == NULL) {
//     ESP_LOGE(TAG, "Failed to open file for reading");
//     return;
//   }

//   // Read a line from file
//   char line[64];
//   fgets(line, sizeof(line), f);
//   fclose(f);

//   // Strip newline
//   char *pos = strchr(line, '\n');
//   if (pos) {
//     *pos = '\0';
//   }
//   ESP_LOGI(TAG, "Read from file: '%s'", line);

//   // All done, unmount partition and disable SPI peripheral
//   esp_vfs_fat_sdcard_unmount(mount_point, card);
//   ESP_LOGI(TAG, "Card unmounted");

//   // deinitialize the bus after all devices are removed
//   spi_bus_free(host.slot);
// }
// https://gist.github.com/mws-rmain/2ba434cd2a3f32d6d343c1c60fbd65c8
