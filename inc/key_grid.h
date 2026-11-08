/**
 * @file key_grid.h
 * @brief The screen split into one cell per key.
 *
 * 3 columns x 4 rows, laid out like the physical keys: key 1 is top left and
 * key 12 is bottom right. Everything is drawn on the canvas (oled_canvas.h).
 */
#pragma once

#include "pico/types.h"
#include <stdbool.h>
#include <stdint.h>

#include "oled_canvas.h"

/** @brief First key with a cell (top left). */
#define KEY_GRID_FIRST_KEY 1
/** @brief Last key with a cell (bottom right). */
#define KEY_GRID_LAST_KEY 12

/** @brief A rectangle on the canvas, in pixels. */
typedef struct key_grid_rect_t {
  int16_t x; // Left edge
  int16_t y; // Top edge
  int16_t w; // Width
  int16_t h; // Height
} KeyGridRect;

/**
 * @brief Gets the rectangle of a key's cell.
 *
 * @param key Key number, 1 to 12.
 * @param[out] rect Filled with the cell's position and size.
 * @return false if the key isn't 1 to 12. @p rect is left alone then.
 */
bool key_grid_cell(uint key, KeyGridRect *rect);

/**
 * @brief Clears a key's cell and draws its frame.
 *
 * Idle cells get corner brackets. Pressed cells are filled, with the corner
 * pixels cut off like a keycap.
 *
 * @param key Key number, 1 to 12. Other keys are ignored.
 * @param pressed true to draw the pressed look.
 */
void key_grid_draw_frame(uint key, bool pressed);

/**
 * @brief Draws text centered in a key's cell.
 *
 * @param key Key number, 1 to 12. Other keys are ignored.
 * @param label Text to draw. NULL draws nothing.
 * @param size Text size.
 * @param pressed true to draw dark text for a pressed (filled) cell.
 */
void key_grid_draw_label(uint key, const char *label, CanvasTextSize size,
                         bool pressed);

/**
 * @brief Draws a bitmap centered in a key's cell.
 *
 * @param key Key number, 1 to 12. Other keys are ignored.
 * @param bitmap Bitmap in page format (see canvas_draw_bitmap()). NULL draws
 *               nothing.
 * @param w Bitmap width in pixels.
 * @param h Bitmap height in pixels.
 * @param pressed true to draw a dark icon for a pressed (filled) cell.
 */
void key_grid_draw_icon(uint key, const uint8_t *bitmap, int16_t w, int16_t h,
                        bool pressed);

/**
 * @brief Sends just this key's cell to the screen.
 *
 * @param key Key number, 1 to 12. Other keys are ignored.
 */
void key_grid_present(uint key);
