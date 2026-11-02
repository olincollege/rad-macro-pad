#include "pico/stdlib.h"
#include <stdio.h>
#include "inc/key_switches.h"
#include "inc/usb_hid_keyboard.h"

// Key mapping struct for each switch index
typedef struct {
    uint8_t modifier; // e.g. KEYBOARD_MODIFIER_LEFTCTRL, or 0
    uint8_t keycode;  // TinyUSB HID keycode (HID_KEY_*)
} key_mapping_t;

// Map GPIO 1 to 12 to USB keys
static const key_mapping_t KEY_MAP[NUM_KEYS] = {
    [1]  = { 0, HID_KEY_7 },
    [2]  = { 0, HID_KEY_8 },
    [3]  = { 0, HID_KEY_9 },
    [4]  = { 0, HID_KEY_4 },
    [5]  = { 0, HID_KEY_5 },
    [6]  = { 0, HID_KEY_6 },
    [7]  = { 0, HID_KEY_1 },
    [8]  = { 0, HID_KEY_2 },
    [9]  = { 0, HID_KEY_3 },
    [10] = { KEYBOARD_MODIFIER_LEFTCTRL, HID_KEY_C},  // Crtl + C
    [11] = { 0, HID_KEY_0 }, 
    [12] = { KEYBOARD_MODIFIER_LEFTCTRL, HID_KEY_V }, // Ctrl + V
};

// Callback invoked on any switch press or release
void on_key_event(unsigned int key_index, bool is_pressed) {
    if (key_index >= NUM_KEYS) return;

    // TODO: add conditional logic to handle encoder presses (GPIO 0) 
    if (is_pressed) {
        usb_hid_send_key(KEY_MAP[key_index].modifier, KEY_MAP[key_index].keycode);
    } else {
        usb_hid_release_key();
    }
}

int main(void) {
    // Initialize hardware drivers
    key_switch_init();
    key_switch_register_callback(on_key_event);

    // Initialize USB HID stack
    usb_hid_init();

    uint32_t last_key_poll = 0;
    const uint32_t poll_interval_ms = 5; // Poll every 5ms 
    while (true) {
        // Run TinyUSB background engine continuously with 0ms latency
        usb_hid_task();
        // Non-blocking timer check for key switch polling
        uint32_t now = to_ms_since_boot(get_absolute_time());
        if (now - last_key_poll >= poll_interval_ms) {
            last_key_poll = now;
            poll_key_switch_press();
        }
    }
    return 0;
}
