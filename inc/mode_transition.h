/**
 * @file mode_transition.h
 * @brief The animation played when the dial changes mode.
 *
 * The old mode's name slides out as the new one slides in, in the direction
 * the dial turned, then holds. Three dots along the bottom show which mode is
 * next.
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Starts (or restarts) the animation and draws its first frame.
 *
 * @param from_mode Mode being left (NUMPAD, PROMPT_ENGINEER or DEBUG).
 * @param to_mode Mode being entered.
 */
void mode_transition_start(int8_t from_mode, int8_t to_mode);

/**
 * @brief Draws the next frame when it's due.
 *
 * Call on every main loop pass.
 *
 * @return true while the animation is still playing.
 */
bool mode_transition_task(void);

/**
 * @brief Tells whether the animation is playing.
 *
 * @return true from mode_transition_start() until the animation ends.
 */
bool mode_transition_running(void);
