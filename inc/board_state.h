#pragma once

#include <stdbool.h>
#include <stdint.h>

// which keys are held down right now. key 0 is the dial button, and keys 1 to
// 12 are the key switches. fed from main's key callback
void board_state_set_key(unsigned key, bool pressed);

bool board_state_key(unsigned key);

// goes up by one every time any key changes, so other code can tell when
// something is new
uint32_t board_state_version(void);
