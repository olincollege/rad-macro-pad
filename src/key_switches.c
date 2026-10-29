#include "pico/stdlib.h"
#include "inc/key_switches.h"

// Single global callback for all keys
static key_callback_t key_callback = NULL;
static bool key_last_state[NUM_KEYS] = {false}; // false = unpressed, true = pressed

// Register the single callback function to handle all keys
void key_switch_register_callback(key_callback_t cb) {
    key_callback = cb;
}

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

        // Edge detection: only fire when state transitions from released -> pressed
        if (is_pressed && !key_last_state[i]) {
            if (key_callback != NULL) {
                key_callback(i); // Call the shared callback with the key index
            }
        }

        key_last_state[i] = is_pressed;
    }
    return PICO_OK;
}