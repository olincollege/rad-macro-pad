#ifndef KEYSTROKE_INJECTION_H_
#define KEYSTROKE_INJECTION_H_

#include "tusb.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define NUM_KEYS 13 // GPIO 0 to 12

// Key mapping struct for keystroke sequences and layouts
typedef struct {
  uint8_t modifier; // e.g. KEYBOARD_MODIFIER_LEFTCTRL, or 0
  uint8_t keycode;  // TinyUSB HID keycode (HID_KEY_*)
} key_mapping_t;

// Macro script identifiers
typedef enum {
  MACRO_NO_MISTAKES = 1,
  MACRO_GEMINI,
  MACRO_CLAUDE,
  MACRO_CHATGPT,
  MACRO_GROK
} macro_id_t;

extern const key_mapping_t KEY_MAP[NUM_KEYS];

// Start sending an arbitrary sequence of key mappings over USB HID. Returns
// right away; macro_task() does the sending. Ignored if a macro is running
void send_macro_script(const key_mapping_t *script, size_t length);

// Helper function to dispatch defined macro scripts by ID
void inject_macro(macro_id_t macro_id);

// Sends the running macro one step at a time. Call on every main loop pass
void macro_task(void);

// True while a macro is still being typed
bool macro_is_running(void);

#endif /* KEYSTROKE_INJECTION_H_ */
