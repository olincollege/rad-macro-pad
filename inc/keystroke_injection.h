#ifndef KEYSTROKE_INJECTION_H_
#define KEYSTROKE_INJECTION_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "tusb.h"

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

// Send an arbitrary sequence of key mappings over USB HID
void send_macro_script(const key_mapping_t *script, size_t length);

// Helper function to dispatch defined macro scripts by ID
void inject_macro(macro_id_t macro_id);

#endif /* KEYSTROKE_INJECTION_H_ */

