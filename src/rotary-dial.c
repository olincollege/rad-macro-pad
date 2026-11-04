#include "rotary-dial.h"

#include "hardware/gpio.h"
#include "pico/types.h"
#include <stdbool.h>
#include <stdint.h>

volatile int8_t state = -1;
volatile uint8_t turns_to_state_change = 0;
volatile bool last_state_a = true;
volatile Direction curr_dir = CLOCKWISE;
volatile Direction last_dir = CLOCKWISE;

void dial_init(void) {
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

void encoder_callback(uint gpio, uint32_t event_mask) {
  // read the current state of rot a
  bool current_state_a = gpio_get(ROTARY_DIAL_ROT_A);

  // filter out potential bouncing or double triggers
  if (current_state_a != last_state_a) {
    // if Rot A != Rot B -> clockwise
    if (gpio_get(ROTARY_DIAL_ROT_B) != current_state_a) {
      curr_dir = CLOCKWISE;
      if (curr_dir == last_dir) {
        turns_to_state_change++;
      } else {
        turns_to_state_change = 0;
      }

      if (turns_to_state_change == MAX_STEPS) {
        turns_to_state_change = 0;
        state++;
        if (state > 1) {
          state = DEBUG;
        }
      }
    } else {
      curr_dir = COUNTER_CLOCKWISE;
      if (curr_dir == last_dir) {
        turns_to_state_change++;
      } else {
        turns_to_state_change = 0;
      }

      if (turns_to_state_change == MAX_STEPS) {
        turns_to_state_change = 0;
        state--;
        if (state < -1) {
          state = NUMPAD;
        }
      }
    }
  }
  last_state_a = current_state_a;
  last_dir = curr_dir;
}