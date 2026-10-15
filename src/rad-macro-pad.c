#include "pico/stdlib.h"
#include <stdio.h>

#ifndef LED_DELAY_MS
#define LED_DELAY_MS 250
#endif

int pico_led_init(void) {
  gpio_init(13);
  gpio_set_dir(13, GPIO_OUT);
  return PICO_OK;
}

// Turn the led on or off
void pico_set_led(bool led_on) {

  // Just set the GPIO on or off
  gpio_put(13, led_on);
}

int main() {
  stdio_init_all();
  int rc = pico_led_init();
  hard_assert(rc == PICO_OK);
  while (true) {
    pico_set_led(true);
    sleep_ms(LED_DELAY_MS);
    pico_set_led(false);
    sleep_ms(LED_DELAY_MS);
  }
}