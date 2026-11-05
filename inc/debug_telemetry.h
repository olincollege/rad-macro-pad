#pragma once

#include <stdbool.h>
#include <stdint.h>

// DEBUG mode's serial output: prints the board's state as a block of lines
// whenever something changes, when DEBUG mode starts, and when a terminal
// opens

// call on every main loop pass while in DEBUG mode
void debug_telemetry_task(void);

// true while a terminal has the serial port open
bool debug_telemetry_terminal_open(void);

// how many state changes couldn't be sent right away because the print queue
// was full. each one is retried, so this counts delays, not lost changes
uint32_t debug_telemetry_dropped(void);
