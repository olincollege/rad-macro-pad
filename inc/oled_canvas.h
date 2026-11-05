#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "rp2040-oled-types.h"

// a 128x64 drawing surface kept in our own RAM. everything is drawn here
// first, then sent to the screen with canvas_present or canvas_present_rect.
// the layout matches the screen's: pages 8 pixels tall, one byte per column,
// bit 0 on top
#define CANVAS_WIDTH 128
#define CANVAS_HEIGHT 64
#define CANVAS_PAGES (CANVAS_HEIGHT / 8)

// text sizes, all from the screen's 6x8 font
typedef enum {
  TEXT_1X,   // 6 x 8 per character
  TEXT_2X,   // 12 x 16 per character
  TEXT_TALL, // 6 x 16 per character (1x wide, 2x high)
} canvas_text_size_t;

// remembers which screen to send to and clears the canvas
void canvas_init(rp2040_oled_t *oled);

// turns every pixel off
void canvas_clear(void);

// on = lit, off = dark. pixels outside the canvas are ignored
void canvas_set_pixel(int x, int y, bool on);
bool canvas_get_pixel(int x, int y);

// rectangles are given by their top left corner, width and height
void canvas_fill_rect(int x, int y, int w, int h, bool on);
void canvas_invert_rect(int x, int y, int w, int h);
void canvas_outline_rect(int x, int y, int w, int h, bool on);

// draws text with its top left at (x, y). only the lit parts of each character
// are drawn, in the color `on`
void canvas_draw_text(int x, int y, const char *text, canvas_text_size_t size,
                      bool on);

// width and height of the visible part of the text, for centering
int canvas_text_width(const char *text, canvas_text_size_t size);
int canvas_text_height(canvas_text_size_t size);

// draws a bitmap in page format (w bytes per page, bit 0 on top). only its 1
// bits are drawn, in the color `on`
void canvas_draw_bitmap(int x, int y, const uint8_t *bitmap, int w, int h,
                        bool on);

// sends the whole canvas to the screen
void canvas_present(void);

// sends only the pages and columns this rectangle touches
void canvas_present_rect(int x, int y, int w, int h);
