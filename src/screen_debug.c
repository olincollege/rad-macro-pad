#include "screen_debug.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "board_state.h"
#include "debug_telemetry.h"
#include "key_grid.h"
#include "oled_canvas.h"
#include "pico/time.h"
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

// everything this screen shows, to tell when a redraw is needed
typedef struct {
  uint32_t keys_version;
  bool terminal_open;
  uint8_t dial_steps;
  Direction dial_dir;
  uint32_t dropped;
} debug_view_t;

static debug_view_t shown;
static uint32_t last_refresh_ms;

static uint32_t now_ms(void) { return to_ms_since_boot(get_absolute_time()); }

static debug_view_t read_view(void) {
  debug_view_t v = {
      .keys_version = board_state_version(),
      .terminal_open = debug_telemetry_terminal_open(),
      .dial_steps = turns_to_state_change,
      .dial_dir = curr_dir,
      .dropped = debug_telemetry_dropped(),
  };
  return v;
}

static bool same_view(const debug_view_t *a, const debug_view_t *b) {
  return a->keys_version == b->keys_version &&
         a->terminal_open == b->terminal_open &&
         a->dial_steps == b->dial_steps && a->dial_dir == b->dial_dir &&
         a->dropped == b->dropped;
}

static void draw_info_line(int line, const char *text) {
  canvas_draw_text(INFO_X, INFO_Y + line * INFO_LINE_STEP, text, TEXT_1X, true);
}

static void draw(const debug_view_t *v) {
  char text[16];

  canvas_clear();

  const char *title = "DEBUG";
  canvas_draw_text((CANVAS_WIDTH - canvas_text_width(title, TEXT_2X)) / 2,
                   TITLE_Y, title, TEXT_2X, true);

  // a small copy of the key layout. filled squares are held keys
  for (unsigned key = KEY_GRID_FIRST_KEY; key <= KEY_GRID_LAST_KEY; key++) {
    unsigned index = key - KEY_GRID_FIRST_KEY;
    int x = MINI_X + (int)(index % MINI_COLUMNS) * (MINI_BOX + MINI_GAP);
    int y = MINI_Y + (int)(index / MINI_COLUMNS) * (MINI_BOX + MINI_GAP);
    if (board_state_key(key))
      canvas_fill_rect(x, y, MINI_BOX, MINI_BOX, true);
    else
      canvas_outline_rect(x, y, MINI_BOX, MINI_BOX, true);
  }

  snprintf(text, sizeof(text), "TERM %s", v->terminal_open ? "OPEN" : "SHUT");
  draw_info_line(0, text);
  snprintf(text, sizeof(text), "DIAL %u/%u %c", (unsigned)v->dial_steps,
           (unsigned)MAX_STEPS, v->dial_dir == CLOCKWISE ? '>' : '<');
  draw_info_line(1, text);
  snprintf(text, sizeof(text), "BTN  %s",
           board_state_key(DIAL_BUTTON_KEY) ? "DN" : "UP");
  draw_info_line(2, text);
  snprintf(text, sizeof(text), "DROP %lu", (unsigned long)v->dropped);
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

  debug_view_t current = read_view();
  if (same_view(&current, &shown))
    return;

  shown = current;
  draw(&shown);
}
