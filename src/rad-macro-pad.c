#include "pico/stdlib.h"
#include <stdio.h>
#include "inc/key_switches.h"

// One callback to handle any key event
void on_key_pressed(uint key_index) {
    printf("Key %u pressed!\n", key_index);

    switch (key_index) {
        case 0:
            // Rotary encoder push-button
            printf("Rotary button pressed\n");
            break;
        case 1:
            // Key 1 action
            gpio_put(13, !gpio_get(13));
            break;
        case 2:
            // Key 2 action
            break;
        default:
            break;
    }
}

int main() {
    stdio_init_all();

    // Initialize LED pin
    gpio_init(13);
    gpio_set_dir(13, GPIO_OUT);

    key_switch_init();
    key_switch_register_callback(on_key_pressed); // Register once

    while (true) {
        poll_key_switch_press();
        sleep_ms(2);
    }
}