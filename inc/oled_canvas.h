/**
 * @file oled_canvas.h
 * @brief A 128x64 drawing surface kept in our own RAM.
 *
 * Everything is drawn here first, then sent to the screen with
 * canvas_present() or canvas_present_rect(). The layout matches the screen's:
 * pages 8 pixels tall, one byte per column, bit 0 on top. Coordinates are in
 * pixels with (0, 0) at the top left, and anything outside the canvas is
 * clipped.
 */
#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "rp2040-oled-types.h"

/** @brief Canvas width in pixels. */
#define CANVAS_WIDTH 128
/** @brief Canvas height in pixels. */
#define CANVAS_HEIGHT 64
/** @brief Number of 8-pixel pages. */
#define CANVAS_PAGES (CANVAS_HEIGHT / 8)

/** @brief Text sizes, all from the screen's 6x8 font. */
typedef enum canvas_text_size_t {
  TEXT_1X,   // 6 x 8 per character
  TEXT_2X,   // 12 x 16 per character
  TEXT_TALL, // 6 x 16 per character (1x wide, 2x high)
} CanvasTextSize;

/**
 * @brief Remembers which screen to send to and clears the canvas.
 *
 * @param oled Display that's already been set up with rp2040_oled_init().
 */
void canvas_init(rp2040_oled_t *oled);

/** @brief Turns every pixel off. */
void canvas_clear(void);

/**
 * @brief Turns one pixel on or off.
 *
 * @param x Column.
 * @param y Row.
 * @param on true for lit, false for dark.
 */
void canvas_set_pixel(int16_t x, int16_t y, bool on);

/**
 * @brief Reads one pixel.
 *
 * @param x Column.
 * @param y Row.
 * @return true if lit. false if dark or outside the canvas.
 */
bool canvas_get_pixel(int16_t x, int16_t y);

/**
 * @brief Fills a rectangle.
 *
 * @param x Left edge.
 * @param y Top edge.
 * @param w Width.
 * @param h Height.
 * @param on true for lit, false for dark.
 */
void canvas_fill_rect(int16_t x, int16_t y, int16_t w, int16_t h, bool on);

/**
 * @brief Flips every pixel in a rectangle.
 *
 * @param x Left edge.
 * @param y Top edge.
 * @param w Width.
 * @param h Height.
 */
void canvas_invert_rect(int16_t x, int16_t y, int16_t w, int16_t h);

/**
 * @brief Draws a 1-pixel outline of a rectangle.
 *
 * @param x Left edge.
 * @param y Top edge.
 * @param w Width. Nothing is drawn if 0 or less.
 * @param h Height. Nothing is drawn if 0 or less.
 * @param on true for lit, false for dark.
 */
void canvas_outline_rect(int16_t x, int16_t y, int16_t w, int16_t h, bool on);

/**
 * @brief Draws text with its top left at (x, y).
 *
 * Only the lit parts of each character are drawn, so the background shows
 * through. Characters the font doesn't have are drawn as '?'.
 *
 * @param x Left edge.
 * @param y Top edge.
 * @param text Null-terminated text.
 * @param size Text size.
 * @param on Color of the characters: true for lit, false for dark.
 */
void canvas_draw_text(int16_t x, int16_t y, const char *text,
                      CanvasTextSize size, bool on);

/**
 * @brief Width of the visible part of some text, for centering.
 *
 * Leaves out the blank column after the last character.
 *
 * @param text Null-terminated text.
 * @param size Text size.
 * @return Width in pixels. 0 for empty text.
 */
int16_t canvas_text_width(const char *text, CanvasTextSize size);

/**
 * @brief Height of the visible part of a line of text, for centering.
 *
 * Counts the rows that digits and capitals use, without descenders.
 *
 * @param size Text size.
 * @return Height in pixels.
 */
int16_t canvas_text_height(CanvasTextSize size);

/**
 * @brief Draws a bitmap in page format.
 *
 * Page format means w bytes per page, one byte per column, bit 0 on top. Only
 * the bitmap's 1 bits are drawn.
 *
 * @param x Left edge.
 * @param y Top edge.
 * @param bitmap Bitmap data, `w * ceil(h / 8)` bytes.
 * @param w Width in pixels.
 * @param h Height in pixels.
 * @param on Color of the 1 bits: true for lit, false for dark.
 */
void canvas_draw_bitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w,
                        int16_t h, bool on);

/** @brief Sends the whole canvas to the screen. */
void canvas_present(void);

/**
 * @brief Sends only the part of the screen this rectangle touches.
 *
 * Whole pages are sent, so the rows above and below the rectangle in the same
 * pages go too. Does nothing before canvas_init().
 *
 * @param x Left edge.
 * @param y Top edge.
 * @param w Width.
 * @param h Height.
 */
void canvas_present_rect(int16_t x, int16_t y, int16_t w, int16_t h);

/** @brief Full screen brightness, for canvas_set_contrast(). */
#define CANVAS_CONTRAST_FULL 0xff

/**
 * @brief Sets the screen's brightness. Leaves the picture alone.
 *
 * Does nothing before canvas_init().
 *
 * @param contrast Brightness, 0x00 (dimmest) to CANVAS_CONTRAST_FULL.
 */
void canvas_set_contrast(uint8_t contrast);
