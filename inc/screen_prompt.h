/**
 * @file screen_prompt.h
 * @brief PROMPT ENGINEER mode's screen.
 *
 * A grid of AI tool icons. Pressing a tool shows its logo full screen while
 * its macro runs, and key 1 shows "MAKE NO MISTAKES." fading in and out.
 */
#pragma once

#include "pico/types.h"
#include <stdbool.h>

/** @brief Draws the grid from the current key states and sends it. */
void screen_prompt_draw(void);

/**
 * @brief Updates the screen for a key press or release.
 *
 * A press on a tool key starts its splash (or the fade for key 1), unless a
 * macro is already running. Otherwise the key's cell is redrawn. Ignored while
 * a splash or fade is showing.
 *
 * Call from the key callback while this screen is up, before the macro starts.
 *
 * @param key Key number, 1 to 12. Other keys are ignored.
 * @param pressed true if the key went down.
 */
void screen_prompt_on_key(uint key, bool pressed);

/**
 * @brief Runs the splash and fade timers.
 *
 * Call on every main loop pass while this screen is up. Returns to the grid
 * when the splash or fade ends.
 *
 * @return true while a splash or fade owns the screen.
 */
bool screen_prompt_task(void);

/**
 * @brief Stops any splash or fade right away and restores full brightness.
 *
 * Used when the dial leaves this mode. Doesn't redraw anything.
 */
void screen_prompt_cancel(void);
