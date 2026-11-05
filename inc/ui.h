#pragma once

#include <stdbool.h>

#include "rp2040-oled-types.h"

// the screen's coordinator. watches the dial's mode and draws the right
// screen for it

// call once, after rp2040_oled_init
void ui_init(rp2040_oled_t *oled);

// call on every main loop pass. redraws when the mode changes
void ui_task(void);

// call from the key callback, after board_state_set_key
void ui_on_key(unsigned key, bool pressed);
