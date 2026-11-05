#include "ui.h"

#include <stdint.h>

#include "mode_transition.h"
#include "oled_canvas.h"
#include "rotary-dial.h"
#include "screen_numpad.h"

// the mode the screen is showing, or animating toward. it can lag `state` by
// one loop pass
static int8_t target_mode;

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
  target_mode = state;
  draw_mode(target_mode);
}

void ui_task(void) {
  int8_t current_mode = state; // read the dial's mode once

  // the dial moved: animate from wherever the screen was headed. turning again
  // mid-animation restarts it from there
  if (current_mode != target_mode) {
    mode_transition_start(target_mode, current_mode);
    target_mode = current_mode;
    return;
  }

  // when the animation finishes, show the new mode's screen
  if (mode_transition_running() && !mode_transition_task())
    draw_mode(target_mode);
}

void ui_on_key(unsigned key, bool pressed) {
  // during the animation keys still work; the screen catches up when it ends
  if (mode_transition_running())
    return;

  if (target_mode == NUMPAD)
    screen_numpad_on_key(key, pressed);
}
