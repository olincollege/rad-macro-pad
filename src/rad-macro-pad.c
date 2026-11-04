#include "hardware/gpio.h"
#include "pico/stdio.h"
#include "rotary-dial.h"
#include <pico/time.h>

extern volatile int8_t state;
extern volatile uint8_t turns_to_state_change;
extern volatile bool last_state_a;

void init(void);

void init(void) {
  stdio_init_all();
  gpio_init(ROTARY_DIAL_BUTTON);
  gpio_set_dir(ROTARY_DIAL_BUTTON, GPIO_IN);
  gpio_init(ROTARY_DIAL_ROT_A);
  gpio_set_dir(ROTARY_DIAL_ROT_A, GPIO_IN);
  gpio_init(ROTARY_DIAL_ROT_B);
  gpio_set_dir(ROTARY_DIAL_ROT_B, GPIO_IN);
  gpio_pull_up(ROTARY_DIAL_BUTTON);
  gpio_pull_up(ROTARY_DIAL_ROT_A);
  gpio_pull_up(ROTARY_DIAL_ROT_B);
}

int main() {
  init();
  // Record initial state of Rot A
  last_state_a = gpio_get(ROTARY_DIAL_ROT_A);

  gpio_set_irq_enabled_with_callback(ROTARY_DIAL_ROT_A,
                                     GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
                                     true, &encoder_callback);

  bool last_button_state = true;

  while (true) {

    // Small delay to prevent tight-looping the core
    sleep_ms(10);
  }
}