/**
 * @file screen_numpad.h
 * @brief NUMPAD mode's screen: 12 labeled cells. A held key is shown inverted.
 */
#pragma once

#include "pico/types.h"
#include <stdbool.h>

/** @brief Draws the whole grid from the current key states and sends it. */
void screen_numpad_draw(void);

/**
 * @brief Redraws one key's cell after it's pressed or released.
 *
 * Reads the key's state from board_state, so call this after
 * board_state_set_key().
 *
 * @param key Key number, 1 to 12. Other keys are ignored.
 * @param pressed Unused; board_state already has it.
 */
void screen_numpad_on_key(uint key, bool pressed);
