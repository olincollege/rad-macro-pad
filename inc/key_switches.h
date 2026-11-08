#ifndef KEY_SWITCHES_H
#define KEY_SWITCHES_H

#include <stdbool.h>

#define NUM_KEYS 13 // GPIO 0 to 12

// Callback function signature: receives key index and whether it is pressed
// (true) or released (false)
typedef void (*key_callback_t)(unsigned int key_index, bool is_pressed);

// Initialize GPIO for key switches and set previous state to unpressed
int key_switch_init(void);

// Register (assign) a callback function to handle key inputs
void key_switch_register_callback(key_callback_t cb);

// Check the state of each key's GPIO. If an edge is detected, call the callback function
int poll_key_switch_press(void);

#endif // KEY_SWITCHES_H
