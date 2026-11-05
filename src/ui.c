#include "ui.h"

#include <stdint.h>

#include "mode_transition.h"
#include "oled_canvas.h"
#include "rotary-dial.h"
#include "screen_debug.h"
#include "screen_numpad.h"
#include "screen_prompt.h"

// the mode the screen is showing, or animating toward. it can lag `state` by
// one loop pass
static int8_t target_mode;

static void draw_mode(int8_t mode) {
  switch (mode) {
  case NUMPAD:
    screen_numpad_draw();
    break;
  case PROMPT_ENGINEER:
    screen_prompt_draw();
    break;
  default:
    screen_debug_draw();
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

  // the dial moved: stop any splash or fade, then animate from wherever the
  // screen was headed. turning again mid-animation restarts it from there
  if (current_mode != target_mode) {
    screen_prompt_cancel();
    mode_transition_start(target_mode, current_mode);
    target_mode = current_mode;
    return;
  }

  // when the animation finishes, show the new mode's screen
  if (mode_transition_running()) {
    if (!mode_transition_task())
      draw_mode(target_mode);
    return;
  }

  if (target_mode == PROMPT_ENGINEER)
    screen_prompt_task();
  else if (target_mode == DEBUG)
    screen_debug_task();
}

void ui_on_key(unsigned key, bool pressed) {
  // during the animation keys still work; the screen catches up when it ends
  if (mode_transition_running())
    return;

  // the debug screen picks up key changes on its own refresh
  if (target_mode == NUMPAD)
    screen_numpad_on_key(key, pressed);
  else if (target_mode == PROMPT_ENGINEER)
    screen_prompt_on_key(key, pressed);
}
