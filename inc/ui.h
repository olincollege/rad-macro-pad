/**
 * @file ui.h
 * @brief The screen's coordinator.
 *
 * Watches the dial's mode, plays the transition when it changes, and draws the
 * right screen for it.
 */
#pragma once

#include "pico/types.h"
#include <stdbool.h>

#include "rp2040-oled-types.h"

/**
 * @brief Sets up the canvas and draws the current mode's screen.
 *
 * Call once, after rp2040_oled_init().
 *
 * @param oled The display to draw on.
 */
void ui_init(rp2040_oled_t *oled);

/**
 * @brief Keeps the screen in step with the mode.
 *
 * Call on every main loop pass. Starts the transition when the mode changes,
 * draws the new screen when it ends, and runs the current screen's task.
 */
void ui_task(void);

/**
 * @brief Passes a key change to the current screen.
 *
 * Call from the key callback, after board_state_set_key(). Ignored during the
 * transition; the screen catches up when it ends.
 *
 * @param key Key number, 0 to 12.
 * @param pressed true if the key went down.
 */
void ui_on_key(uint key, bool pressed);
