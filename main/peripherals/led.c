#include "driver/gpio.h"
#include "peripherals/led.h"

void configure_led(void) {
  gpio_reset_pin(LED_R);
  gpio_reset_pin(LED_G);
  gpio_reset_pin(LED_B);
  /* Set the GPIO as a push/pull output */
  gpio_set_direction(LED_R, GPIO_MODE_OUTPUT);
  gpio_set_direction(LED_G, GPIO_MODE_OUTPUT);
  gpio_set_direction(LED_B, GPIO_MODE_OUTPUT);
  gpio_set_level(LED_R, 1);
  gpio_set_level(LED_G, 1);
  gpio_set_level(LED_B, 1);

}