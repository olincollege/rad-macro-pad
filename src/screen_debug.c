/**
 * @file screen_debug.c
 * @brief DEBUG mode's screen.
 */
#include "screen_debug.h"

#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "board_state.h"
#include "debug_telemetry.h"
#include "key_grid.h"
#include "oled_canvas.h"
#include "pico/time.h"
#include "pico/types.h"
#include "rotary-dial.h"

// set by the dial's interrupt in rotary-dial.c
extern volatile Direction curr_dir;

#define REFRESH_MS 50 // at most 20 redraws a second
#define DIAL_BUTTON_KEY 0

#define TITLE_Y 2
#define MINI_COLUMNS 3
#define MINI_BOX 9 // key map squares
#define MINI_GAP 2
#define MINI_X 4
#define MINI_Y 20
#define INFO_X 44
#define INFO_Y 20
#define INFO_LINE_STEP 11

/** @brief Everything this screen shows, to tell when a redraw is needed. */
typedef struct debug_view_t {
  uint32_t keys_version; // Includes the dial button
  bool terminal_open;    // A terminal has the serial port open
  uint8_t dial_steps;    // Steps toward the next mode change
  Direction dial_dir;    // Last direction the dial turned
  uint32_t dropped;      // Delayed telemetry prints
} DebugView;

static DebugView shown;
static uint32_t last_refresh_ms;

/**
 * @brief Milliseconds since boot.
 *
 * @return Current time in ms.
 */
static uint32_t now_ms(void) { return to_ms_since_boot(get_absolute_time()); }

/**
 * @brief Reads everything this screen shows.
 *
 * @return The current view.
 */
static DebugView read_view(void) {
  DebugView v = {
      .keys_version = board_state_version(),
      .terminal_open = debug_telemetry_terminal_open(),
      .dial_steps = turns_to_state_change,
      .dial_dir = curr_dir,
      .dropped = debug_telemetry_dropped(),
  };
  return v;
}

/**
 * @brief Compares two views.
 *
 * @param a First view.
 * @param b Second view.
 * @return true if nothing differs.
 */
static bool same_view(const DebugView *a, const DebugView *b) {
  return a->keys_version == b->keys_version &&
         a->terminal_open == b->terminal_open &&
         a->dial_steps == b->dial_steps && a->dial_dir == b->dial_dir &&
         a->dropped == b->dropped;
}

/**
 * @brief Draws one line of the status column on the right.
 *
 * @param line Line number, 0 at the top.
 * @param text Null-terminated text.
 */
static void draw_info_line(uint8_t line, const char *text) {
  canvas_draw_text(INFO_X, INFO_Y + line * INFO_LINE_STEP, text, TEXT_1X, true);
}

/**
 * @brief Draws the whole screen and sends it.
 *
 * @param v View to draw the status lines from. The key map is read live from
 *          board_state.
 */
static void draw(const DebugView *v) {
  char text[16];

  canvas_clear();

  const char *title = "DEBUG";
  canvas_draw_text((CANVAS_WIDTH - canvas_text_width(title, TEXT_2X)) / 2,
                   TITLE_Y, title, TEXT_2X, true);

  // a small copy of the key layout. filled squares are held keys
  for (uint key = KEY_GRID_FIRST_KEY; key <= KEY_GRID_LAST_KEY; key++) {
    uint index = key - KEY_GRID_FIRST_KEY;
    int16_t x =
        MINI_X + (int16_t)(index % MINI_COLUMNS) * (MINI_BOX + MINI_GAP);
    int16_t y =
        MINI_Y + (int16_t)(index / MINI_COLUMNS) * (MINI_BOX + MINI_GAP);
    if (board_state_key(key))
      canvas_fill_rect(x, y, MINI_BOX, MINI_BOX, true);
    else
      canvas_outline_rect(x, y, MINI_BOX, MINI_BOX, true);
  }

  snprintf(text, sizeof(text), "TERM %s", v->terminal_open ? "OPEN" : "SHUT");
  draw_info_line(0, text);
  snprintf(text, sizeof(text), "DIAL %u/%u %c", (uint)v->dial_steps,
           (uint)MAX_STEPS, v->dial_dir == CLOCKWISE ? '>' : '<');
  draw_info_line(1, text);
  snprintf(text, sizeof(text), "BTN  %s",
           board_state_key(DIAL_BUTTON_KEY) ? "DN" : "UP");
  draw_info_line(2, text);
  snprintf(text, sizeof(text), "DROP %" PRIu32, v->dropped);
  draw_info_line(3, text);

  canvas_present();
}

void screen_debug_draw(void) {
  shown = read_view();
  last_refresh_ms = now_ms();
  draw(&shown);
}

void screen_debug_task(void) {
  uint32_t now = now_ms();
  if (now - last_refresh_ms < REFRESH_MS)
    return;
  last_refresh_ms = now;

  DebugView current = read_view();
  if (same_view(&current, &shown))
    return;

  shown = current;
  draw(&shown);
}
