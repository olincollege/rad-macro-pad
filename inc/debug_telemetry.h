/**
 * @file debug_telemetry.h
 * @brief DEBUG mode's serial output.
 *
 * Prints the board's state as a block of lines over USB CDC whenever something
 * changes, when DEBUG mode starts, and when a terminal opens.
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Prints a new state block if one is due.
 *
 * Call on every main loop pass while in DEBUG mode. A gap of more than 100 ms
 * between calls counts as DEBUG mode starting again.
 */
void debug_telemetry_task(void);

/**
 * @brief Tells whether a terminal has the serial port open.
 *
 * @return true while a terminal is connected.
 */
bool debug_telemetry_terminal_open(void);

/**
 * @brief Counts state changes that couldn't be sent right away.
 *
 * A change isn't sent right away when the print queue is full. Each one is
 * retried, so this counts delays, not lost changes.
 *
 * @return Number of delayed prints since boot.
 */
uint32_t debug_telemetry_dropped(void);
