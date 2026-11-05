#pragma once

#include <stdbool.h>

// NUMPAD mode: 12 labeled cells. a held key is shown inverted

// draws the whole grid from the current key states and sends it
void screen_numpad_draw(void);

// redraws one key's cell after it's pressed or released
void screen_numpad_on_key(unsigned key, bool pressed);
