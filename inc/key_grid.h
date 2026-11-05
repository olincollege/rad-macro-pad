#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "oled_canvas.h"

// the screen split into 3 columns x 4 rows, one cell per key, laid out like
// the physical keys: key 1 is top left and key 12 is bottom right
#define KEY_GRID_FIRST_KEY 1
#define KEY_GRID_LAST_KEY 12

typedef struct {
  int x;
  int y;
  int w;
  int h;
} key_grid_rect_t;

// fills `rect` with the key's cell. false if the key isn't 1 to 12
bool key_grid_cell(unsigned key, key_grid_rect_t *rect);

// clears the cell and draws its frame: corner brackets when idle, filled when
// pressed
void key_grid_draw_frame(unsigned key, bool pressed);

// draws a label or an icon centered in the cell, dark on a pressed cell
void key_grid_draw_label(unsigned key, const char *label,
                         canvas_text_size_t size, bool pressed);
void key_grid_draw_icon(unsigned key, const uint8_t *bitmap, int w, int h,
                        bool pressed);

// sends just this cell to the screen
void key_grid_present(unsigned key);
