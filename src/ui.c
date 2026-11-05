#include "ui.h"

#include <stdint.h>

#include "oled_canvas.h"
#include "rotary-dial.h"
#include "screen_numpad.h"

// the mode currently on the screen. it can lag `state` by one loop pass
static int8_t shown_mode;

// placeholder until the prompt and debug screens exist
static void draw_mode_name(const char *name) {
  canvas_clear();
  int x = (CANVAS_WIDTH - canvas_text_width(name, TEXT_2X)) / 2;
  int y = (CANVAS_HEIGHT - canvas_text_height(TEXT_2X)) / 2;
  canvas_draw_text(x, y, name, TEXT_2X, true);
  canvas_present();
}

static void draw_mode(int8_t mode) {
  switch (mode) {
  case NUMPAD:
    screen_numpad_draw();
    break;
  case PROMPT_ENGINEER:
    draw_mode_name("PROMPT");
    break;
  default:
    draw_mode_name("DEBUG");
    break;
  }
}

void ui_init(rp2040_oled_t *oled) {
  canvas_init(oled);
  shown_mode = state;
  draw_mode(shown_mode);
}

void ui_task(void) {
  int8_t current_mode = state; // read the dial's mode once
  if (current_mode == shown_mode)
    return;

  shown_mode = current_mode;
  draw_mode(shown_mode);
}

void ui_on_key(unsigned key, bool pressed) {
  if (shown_mode == NUMPAD)
    screen_numpad_on_key(key, pressed);
}
