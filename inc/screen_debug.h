/**
 * @file screen_debug.h
 * @brief DEBUG mode's screen.
 *
 * Shows the title, a live map of the keys, and the terminal, dial, button and
 * dropped-print status.
 */
#pragma once

/** @brief Draws the whole screen and sends it. */
void screen_debug_draw(void);

/**
 * @brief Redraws the screen when something changed.
 *
 * Call on every main loop pass while this screen is up. Redraws at most 20
 * times a second.
 */
void screen_debug_task(void);
