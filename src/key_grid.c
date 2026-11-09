/**
 * @file key_grid.c
 * @brief Key cell layout and drawing.
 */
#include "key_grid.h"
#include "pico/types.h"

#define GRID_COLUMNS 3
#define CELL_WIDTH 42  // 3 cells + 2 gaps of 1 = 128
#define CELL_HEIGHT 15 // each row starts on a page boundary (0, 16, 32, 48)
#define CELL_STEP_X (CELL_WIDTH + 1)
#define CELL_STEP_Y (CELL_HEIGHT + 1)
#define BRACKET_ARM 4 // length of each corner bracket arm, in pixels

bool key_grid_cell(uint key, KeyGridRect *rect) {
  if (key < KEY_GRID_FIRST_KEY || key > KEY_GRID_LAST_KEY)
    return false;

  uint index = key - KEY_GRID_FIRST_KEY;
  rect->x = (int16_t)(index % GRID_COLUMNS) * CELL_STEP_X;
  rect->y = (int16_t)(index / GRID_COLUMNS) * CELL_STEP_Y;
  rect->w = CELL_WIDTH;
  rect->h = CELL_HEIGHT;
  return true;
}

void key_grid_draw_frame(uint key, bool pressed) {
  KeyGridRect r;
  if (!key_grid_cell(key, &r))
    return;

  canvas_fill_rect(r.x, r.y, r.w, r.h, false);
  int16_t right = r.x + r.w - 1;
  int16_t bottom = r.y + r.h - 1;

  if (pressed) {
    // a lit cell with its 4 corner pixels cut off, like a keycap
    canvas_fill_rect(r.x, r.y, r.w, r.h, true);
    canvas_set_pixel(r.x, r.y, false);
    canvas_set_pixel(right, r.y, false);
    canvas_set_pixel(r.x, bottom, false);
    canvas_set_pixel(right, bottom, false);
    return;
  }

  // idle: an L shaped bracket in each corner, which stays clear of the label
  canvas_fill_rect(r.x, r.y, BRACKET_ARM, 1, true);
  canvas_fill_rect(r.x, r.y, 1, BRACKET_ARM, true);
  canvas_fill_rect(right - BRACKET_ARM + 1, r.y, BRACKET_ARM, 1, true);
  canvas_fill_rect(right, r.y, 1, BRACKET_ARM, true);
  canvas_fill_rect(r.x, bottom, BRACKET_ARM, 1, true);
  canvas_fill_rect(r.x, bottom - BRACKET_ARM + 1, 1, BRACKET_ARM, true);
  canvas_fill_rect(right - BRACKET_ARM + 1, bottom, BRACKET_ARM, 1, true);
  canvas_fill_rect(right, bottom - BRACKET_ARM + 1, 1, BRACKET_ARM, true);
}

void key_grid_draw_label(uint key, const char *label, CanvasTextSize size,
                         bool pressed) {
  KeyGridRect r;
  if (!key_grid_cell(key, &r) || label == NULL)
    return;

  int16_t x = r.x + (r.w - canvas_text_width(label, size)) / 2;
  int16_t y = r.y + (r.h - canvas_text_height(size)) / 2;
  canvas_draw_text(x, y, label, size, !pressed);
}

void key_grid_draw_icon(uint key, const uint8_t *bitmap, int16_t w, int16_t h,
                        bool pressed) {
  KeyGridRect r;
  if (!key_grid_cell(key, &r) || bitmap == NULL)
    return;

  int16_t x = r.x + (r.w - w) / 2;
  int16_t y = r.y + (r.h - h) / 2;
  canvas_draw_bitmap(x, y, bitmap, w, h, !pressed);
}

void key_grid_present(uint key) {
  KeyGridRect r;
  if (key_grid_cell(key, &r))
    canvas_present_rect(r.x, r.y, r.w, r.h);
}
