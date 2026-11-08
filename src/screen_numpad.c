/**
 * @file screen_numpad.c
 * @brief NUMPAD mode's screen.
 */
#include "screen_numpad.h"

#include <string.h>

#include "board_state.h"
#include "key_grid.h"
#include "oled_canvas.h"
#include "pico/types.h"

// what each key types in NUMPAD mode (see KEY_MAP in keystroke_injection.c)
static const char *const NUMPAD_LABELS[KEY_GRID_LAST_KEY + 1] = {
    [1] = "7",  [2] = "8",     [3] = "9",  [4] = "4",
    [5] = "5",  [6] = "6",     [7] = "1",  [8] = "2",
    [9] = "3",  [10] = "COPY", [11] = "0", [12] = "PASTE",
};

/**
 * @brief Draws one key's cell from its current state.
 *
 * @param key Key number, 1 to 12.
 */
static void draw_key(uint key) {
  bool pressed = board_state_key(key);
  const char *label = NUMPAD_LABELS[key];

  key_grid_draw_frame(key, pressed);
  // digits are big, words have to be small to fit
  key_grid_draw_label(key, label, strlen(label) == 1 ? TEXT_2X : TEXT_1X,
                      pressed);
}

void screen_numpad_draw(void) {
  canvas_clear();
  for (uint key = KEY_GRID_FIRST_KEY; key <= KEY_GRID_LAST_KEY; key++)
    draw_key(key);
  canvas_present();
}

void screen_numpad_on_key(uint key, bool pressed) {
  (void)pressed; // board_state already has it
  if (key < KEY_GRID_FIRST_KEY || key > KEY_GRID_LAST_KEY)
    return;

  draw_key(key);
  key_grid_present(key);
}
