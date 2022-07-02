

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
const char *dir_path = MOUNT_POINT "/";

// int log_received_message(rtc_time_t *time, packet_received_t *packet) {
//   // search for valid file

//   if (last_used_file_path[0] == 0) { // if first time log, go search for last
//   used
//     // check equals to string not empty
//     search_file(dir_path, last_used_file_path, true);//if most_recent is
//     false this will get the least recent
//   }

//   if (last_used_file_path[0] != 0) {
//     // if something found, check if size is good
//     struct stat properties;

//     if (stat(last_used_file_path, &properties) < 0) {
//       // HOW TO MANAGE THIS ERROR?
//     }
//     if (properties.st_size > MAX_FILE_SIZE) {
//       last_used_file_path[0] = 0;
//     }
//   }

//   if (last_used_file_path[0] != 0) { // if no valid file exists, must create
//   one
//     char new_file_name[MAX_FILENAME_SIZE];
//     ESP_LOGI(TAG, "CREATING NEW FILE");
//     snprintf(new_file_name, MAX_FILENAME_SIZE,
//     "LOG_2%03d%02d%02d%02d%02d%02d.TXT",
//              time->year, time->month, time->day, time->hour, time->min,
//              time->sec);
//     create_new_file((char *)dir_path, new_file_name);
//   }
//   FILE *f;
//   // now last_used_file_path is a valid filepath
//   f = fopen(last_used_file_path, "a");
//   if (f == NULL) {
//     return -1;
//   }
//   ESP_LOGI(TAG, "WRITING CONTENT");
//   uint8_t separator[] = SEPARATOR_DEF;
//   fwrite((uint8_t *)separator, sizeof(uint8_t), sizeof(separator), f);
//   fwrite((uint8_t *)time, sizeof(uint8_t), sizeof(rtc_time_t), f);
//   fwrite((uint8_t *)packet, sizeof(uint8_t), HEADER_SIZE, f);
//   fwrite((uint8_t *)(&packet[HEADER_SIZE]), sizeof(uint8_t),
//   packet->frame_size,
//          f);
//   fwrite((uint8_t *)(&packet[sizeof(packet_received_t) - FOOTER_SIZE]),
//          sizeof(uint8_t), FOOTER_SIZE, f);
//   fclose(f);
//   // TODO consider if putting the structure sizes on a separate file
//   return 0;
// }

void time2name(char *name, rtc_time_t *time) {
  snprintf(name, MAX_FILENAME_SIZE, "LOG_2%03dy%02dm%02dd%02dh%02dm%02ds.TXT",
           time->year, time->month, time->day, time->hour, time->min,
           time->sec);
}

void name2time(char *name, rtc_time_t *time) {
  sscanf(name, "LOG_2%sy%2sm%2sd%2sh%2sm%2ss.TXT", &time->year, &time->month,
         &time->day, &time->hour, &time->min, &time->sec);
}

bool rtc_time_time_compare_newer(rtc_time_t *time1, rtc_time_t *time2) {
  if (time1->year > time2->year) {
    return true;
  } 
  if (time1->year < time2->year) {
    return false;
  }

  if (time1->month > time2->month) {
    return true;
  } 
  if (time1->month < time2->month) {
    return false;
  }

  if (time1->day > time2->day) {
    return true;
  } 
  if (time1->day < time2->day) {
    return false;
  }

  if (time1->hour > time2->hour) {
    return true;
  } 
  if (time1->hour < time2->hour) {
    return false;
  }

  if (time1->min > time2->min) {
    return true;
  } 
  if (time1->min < time2->min) {
    return false;
  }
  if (time1->sec > time2->sec) {
    return true;
  } 
  if (time1->sec < time2->sec) {
    return false;
  }
  return false;
}

int8_t is_gt(char *name1, char *name2) {
  rtc_time_t time1, time2;
  name2time(name1, &time1);
  name2time(name2, &time2);
  return rtc_time_time_compare_newer(&time1,&time2);
}


int log_received_messageHR(rtc_time_t *time, const char *message) {
  // search for valid file
  bool found = false;
  if (last_used_file_path[0] == 0) { // if first time log, go search for last
                                     // used check equals to string not empty
    int ret = search_file(dir_path, last_used_file_path, is_gt); // if most_recent
    // is false this will get the least recent
    // int ret = 0; // TODO NOT FOUND BECAUSE I NEED to set time on SD
    // operations
    // https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/system_time.html
    ESP_LOGI(TAG, "SEARCH FILE RETURNED:%d %s", ret, last_used_file_path);
    if (ret == -1) {
      return -1;
    } else {
      found = (ret == 1);
    }
  } else {
    found = true;
  }
  ESP_LOGI(TAG, "FOUND VALUE:%d", found);
  if (found) {
    // if something found, check if size is good
    struct stat properties;
    if (stat(last_used_file_path, &properties) < 0) {
      perror("stat");
      // HOW TO MANAGE THIS ERROR?
    }
    ESP_LOGI(TAG, "FILE SIZE:%ld", properties.st_size);
    if (properties.st_size > MAX_FILE_SIZE) {
      ESP_LOGI(TAG, "MAX_SIZE REACHED:%d", found);
      last_used_file_path[0] = 0;
      found = false;
    }
  }

  if (!found) { // if no valid file exists, must create one
    char new_file_name[MAX_FILENAME_SIZE];
    time2name(new_file_name, time);
    ESP_LOGI(TAG, "CREATING NEW FILE %s", new_file_name);
    //int ret = 
    create_new_file_raw((char *)dir_path, new_file_name);
    strncpy(last_used_file_path, dir_path, MAX_FILENAME_SIZE);
    strncat(last_used_file_path, new_file_name,
            MAX_FILENAME_SIZE - strlen(dir_path));
  }
  FILE *f;
  // now last_used_file_path is a valid filepath
  f = fopen(last_used_file_path, "a");
  if (f == NULL) {
    ESP_LOGI(TAG, "ERROR WRITING in %s", last_used_file_path);
    perror("errore fopen:");
    return -1;
  }

  //
  ESP_LOGI(TAG, "WRITING CONTENT on %s", last_used_file_path);
  fprintf(f, "[2%03d/%02d/%02d-%02d:%02d:%02d]:", time->year, time->month,
          time->day, time->hour, time->min, time->sec);
  fprintf(f, "%s", message);
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
