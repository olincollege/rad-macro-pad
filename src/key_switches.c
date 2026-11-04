#include "inc/key_switches.h"
#include "pico/stdlib.h"

// Callback for key press/release
static key_callback_t key_callback = NULL;
static bool key_last_state[NUM_KEYS] = {
    false}; // false = unpressed, true = pressed

// Register the callback function
void key_switch_register_callback(key_callback_t cb) { key_callback = cb; }

int key_switch_init(void) {
  for (uint i = 0; i < NUM_KEYS; i++) {
    gpio_init(i);
    gpio_set_dir(i, GPIO_IN);
    gpio_pull_up(i);
    key_last_state[i] = false;
  }
  return PICO_OK;
}

int poll_key_switch_press(void) {
  for (uint i = 0; i < NUM_KEYS; i++) {
    // Active-low: pressed when pin is LOW (0)
    bool is_pressed = !gpio_get(i);

    // Edge detection: fire when state transitions (press or release)
    if (is_pressed != key_last_state[i]) {
      if (key_callback != NULL) {
        key_callback(i, is_pressed);
      }
      key_last_state[i] = is_pressed;
    }
  }
  return PICO_OK;
}
