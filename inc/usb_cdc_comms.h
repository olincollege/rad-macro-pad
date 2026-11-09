/**
 * @file usb_cdc_comms.h
 * @brief Non-blocking printing over USB CDC (the serial port).
 *
 * Messages are formatted into a 1 KB ring pool and queued (up to 8), then sent
 * a piece at a time by usb_cdc_task() as the CDC buffer has room.
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>

/** @brief One queued message: a span of the print pool. */
typedef struct line_unit_t {
  const char *character_data; // Start of the unsent text
  uint32_t len;               // Bytes left to send
} LineUnit;

/**
 * @brief Runs TinyUSB and sends queued text.
 *
 * Call on every main loop pass while CDC is in use.
 */
void usb_cdc_task(void);

/** @brief Sets up the print queue. Call once before printing. */
void usb_cdc_init(void);

/**
 * @brief Queues a line of text followed by "\r\n".
 *
 * @param print_data Null-terminated text.
 * @return Same as printf_cdc().
 */
bool println_cdc(const char *print_data);

/**
 * @brief Queues printf-style formatted text.
 *
 * Doesn't block. The text is sent later by usb_cdc_task().
 *
 * @param format printf format string.
 * @param ... Format arguments.
 * @return false if no terminal is connected, the queue is full, or the text
 *         doesn't fit in the pool right now.
 */
bool printf_cdc(const char *format, ...);
