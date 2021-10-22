

#include "utility/utility.h"

#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include <string.h>
#include <sys/stat.h>
#include <sys/unistd.h>

#include "peripherals/spi_sd.h"

#define TAG "UTILITY"

FILE *file_pointer_to_last_used = NULL; //after first search it points to last file



void log_received_message(rtc_time_t *time, packet_received_t packet){
  //search for valid file
  if (file_pointer_to_last_used == NULL){
    file_pointer_to_last_used = search_last();
  }
  //if last is full, create new one
  //if stat. size > MAX_FILE_SIZE
  //  create new file
  //  check space left on SD

  //open it
  //append packet, with no idle space(in buffer)

}


void do_test_spi_sd() {
  esp_err_t ret;
  esp_vfs_fat_sdmmc_mount_config_t mount_config = {
      .format_if_mount_failed = true,
      .max_files = 5,
      .allocation_unit_size = 16 * 1024};
  sdmmc_card_t *card;

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
                                &card);

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

  // Card has been initialized, print its properties
  sdmmc_card_print_info(stdout, card);

  // Use POSIX and C standard library functions to work with files.

  // First create a file.
  const char *file_hello = MOUNT_POINT "/hello.txt";


  search_file();


  ESP_LOGI(TAG, "Opening file %s", file_hello);
  FILE *f = fopen(file_hello, "w");
  if (f == NULL) {
    ESP_LOGE(TAG, "Failed to open file for writing");
    return;
  }
  fprintf(f, "Hello %s!\n", card->cid.name);
  fclose(f);
  ESP_LOGI(TAG, "File written");

  const char *file_foo = MOUNT_POINT "/foo.txt";

  // Check if destination file exists before renaming
  struct stat st;
  if (stat(file_foo, &st) == 0) {
    // Delete it if it exists
    unlink(file_foo);
  }

  // Rename original file
  ESP_LOGI(TAG, "Renaming file %s to %s", file_hello, file_foo);
  if (rename(file_hello, file_foo) != 0) {
    ESP_LOGE(TAG, "Rename failed");
    return;
  }

  // Open renamed file for reading
  ESP_LOGI(TAG, "Reading file %s", file_foo);
  f = fopen(file_foo, "r");
  if (f == NULL) {
    ESP_LOGE(TAG, "Failed to open file for reading");
    return;
  }

  // Read a line from file
  char line[64];
  fgets(line, sizeof(line), f);
  fclose(f);

  // Strip newline
  char *pos = strchr(line, '\n');
  if (pos) {
    *pos = '\0';
  }
  ESP_LOGI(TAG, "Read from file: '%s'", line);

  // All done, unmount partition and disable SPI peripheral
  esp_vfs_fat_sdcard_unmount(mount_point, card);
  ESP_LOGI(TAG, "Card unmounted");

  // deinitialize the bus after all devices are removed
  spi_bus_free(host.slot);
}
//https://gist.github.com/mws-rmain/2ba434cd2a3f32d6d343c1c60fbd65c8
