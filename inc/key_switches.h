#ifndef KEY_SWITCHES_H
#define KEY_SWITCHES_H

#define NUM_KEYS 13  // GPIO 0 to 12

// Single callback function signature receiving the key that was pressed
typedef void (*key_callback_t)(unsigned int key_index);

int key_switch_init(void);
void key_switch_register_callback(key_callback_t cb);
int poll_key_switch_press(void);

#endif // KEY_SWITCHES_H

