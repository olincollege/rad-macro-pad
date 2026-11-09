/**
 * @file screen_prompt.c
 * @brief PROMPT ENGINEER mode's screen, splashes and fade.
 */
#include "screen_prompt.h"

#include <stddef.h>
#include <stdint.h>

#include "board_state.h"
#include "key_grid.h"
#include "keystroke_injection.h"
#include "oled_canvas.h"
#include "pico/time.h"
#include "pico/types.h"
#include "sprites.h"

#define SPLASH_HOLD_MS 1000 // logo stays this long after the macro finishes

#define NO_MISTAKES_FADE_IN_MS 500
#define NO_MISTAKES_HOLD_MS 500
#define NO_MISTAKES_FADE_OUT_MS 500
#define FADE_STEP_MS 15 // time between brightness changes
#define NO_MISTAKES_LINE_GAP 6

#define KEY_NO_MISTAKES 1

/** @brief What a key shows. Keys left out of PROMPT_KEYS are empty cells. */
typedef struct prompt_key_t {
  const uint8_t *icon;   // Grid icon, or NULL
  const char *label;     // Grid text when there's no icon, or NULL
  const uint8_t *splash; // Full screen logo, or NULL for no splash
  const char *name;      // Shown next to the splash logo
} PromptKey;

// keys 1 to 5 match macro_id_t in keystroke_injection.h
static const PromptKey PROMPT_KEYS[KEY_GRID_LAST_KEY + 1] = {
    [1] = {.icon = sprite_eye_small},
    [2] = {.icon = sprite_gemini_small,
           .splash = sprite_gemini_large,
           .name = "GEMINI"},
    [3] = {.icon = sprite_claude_small,
           .splash = sprite_claude_large,
           .name = "CLAUDE"},
    [4] = {.icon = sprite_chatgpt_small,
           .splash = sprite_chatgpt_large,
           .name = "CHATGPT"},
    [5] = {.icon = sprite_grok_small,
           .splash = sprite_grok_large,
           .name = "GROK"},
    [12] = {.label = "ENTR"},
};

/** @brief What currently owns the screen instead of the grid. */
typedef enum overlay_t {
  OVERLAY_NONE,     // The grid is showing
  OVERLAY_SPLASH,   // A tool's logo, while its macro runs
  OVERLAY_FADE_IN,  // "MAKE NO MISTAKES." brightening
  OVERLAY_HOLD,     // "MAKE NO MISTAKES." at full brightness
  OVERLAY_FADE_OUT, // "MAKE NO MISTAKES." dimming
} Overlay;

static Overlay overlay = OVERLAY_NONE;
static uint32_t phase_start_ms;
static uint32_t last_fade_step_ms;
static uint32_t splash_until_ms;

/**
 * @brief Milliseconds since boot.
 *
 * @return Current time in ms.
 */
static uint32_t now_ms(void) { return to_ms_since_boot(get_absolute_time()); }

/**
 * @brief Draws one key's cell from its current state, with its icon or label.
 *
 * @param key Key number, 1 to 12.
 */
static void draw_key(uint key) {
  bool pressed = board_state_key(key);
  const PromptKey *k = &PROMPT_KEYS[key];

  key_grid_draw_frame(key, pressed);
  if (k->icon != NULL)
    key_grid_draw_icon(key, k->icon, SPRITE_SMALL_WIDTH, SPRITE_SMALL_HEIGHT,
                       pressed);
  else if (k->label != NULL)
    key_grid_draw_label(key, k->label, TEXT_1X, pressed);
}

void screen_prompt_draw(void) {
  canvas_clear();
  for (uint key = KEY_GRID_FIRST_KEY; key <= KEY_GRID_LAST_KEY; key++)
    draw_key(key);
  canvas_present();
}

/**
 * @brief Shows a tool's splash: logo on the left half, name centered in the
 * right half.
 *
 * @param key Key number. Its PROMPT_KEYS entry must have a splash and name.
 */
static void show_splash(uint key) {
  const PromptKey *k = &PROMPT_KEYS[key];
  int16_t half = CANVAS_WIDTH / 2;

  canvas_clear();
  canvas_draw_bitmap(0, 0, k->splash, SPRITE_LARGE_WIDTH, SPRITE_LARGE_HEIGHT,
                     true);
  int16_t x = half + (half - canvas_text_width(k->name, TEXT_TALL)) / 2;
  int16_t y = (CANVAS_HEIGHT - canvas_text_height(TEXT_TALL)) / 2;
  canvas_draw_text(x, y, k->name, TEXT_TALL, true);
  canvas_present();

  overlay = OVERLAY_SPLASH;
  splash_until_ms = now_ms() + SPLASH_HOLD_MS;
}

/**
 * @brief Starts the "MAKE NO MISTAKES." fade.
 *
 * The text is drawn at the lowest brightness, then screen_prompt_task() fades
 * it in.
 */
static void show_no_mistakes(void) {
  static const char *const lines[2] = {"MAKE NO", "MISTAKES."};
  int16_t line_height = canvas_text_height(TEXT_2X);
  int16_t y = (CANVAS_HEIGHT - (2 * line_height + NO_MISTAKES_LINE_GAP)) / 2;

  canvas_set_contrast(0);
  canvas_clear();
  for (int i = 0; i < 2; i++) {
    int16_t x = (CANVAS_WIDTH - canvas_text_width(lines[i], TEXT_2X)) / 2;
    canvas_draw_text(x, y + i * (line_height + NO_MISTAKES_LINE_GAP), lines[i],
                     TEXT_2X, true);
  }
  canvas_present();

  overlay = OVERLAY_FADE_IN;
  phase_start_ms = now_ms();
  last_fade_step_ms = phase_start_ms;
}

/**
 * @brief Ends a splash or fade and goes back to the grid at full brightness.
 *
 * The grid is drawn first, so a dimmed screen never flashes the old picture.
 */
static void end_overlay(void) {
  overlay = OVERLAY_NONE;
  screen_prompt_draw();
  canvas_set_contrast(CANVAS_CONTRAST_FULL);
}

void screen_prompt_on_key(uint key, bool pressed) {
  if (key < KEY_GRID_FIRST_KEY || key > KEY_GRID_LAST_KEY)
    return;

  // a splash or the fade owns the screen. the grid is redrawn from the key
  // states when it ends
  if (overlay != OVERLAY_NONE)
    return;

  // a tool key starts its splash, unless a macro is still typing (the macro
  // player ignores that press too)
  if (pressed && !macro_is_running()) {
    if (key == KEY_NO_MISTAKES) {
      show_no_mistakes();
      return;
    }
    if (PROMPT_KEYS[key].splash != NULL) {
      show_splash(key);
      return;
    }
  }

  draw_key(key);
  key_grid_present(key);
}

/**
 * @brief Brightness for a point in a fade.
 *
 * Squared, so it starts slow and creeps up.
 *
 * @param elapsed Time into the fade.
 * @param total Length of the fade.
 * @return 0 at the start, up to CANVAS_CONTRAST_FULL at @p total.
 */
static uint8_t fade_level(uint32_t elapsed, uint32_t total) {
  if (elapsed >= total)
    return CANVAS_CONTRAST_FULL;
  return (uint8_t)((uint32_t)CANVAS_CONTRAST_FULL * elapsed * elapsed /
                   (total * total));
}

bool screen_prompt_task(void) {
  if (overlay == OVERLAY_NONE)
    return false;

  uint32_t now = now_ms();
  uint32_t elapsed = now - phase_start_ms;

  switch (overlay) {
  case OVERLAY_SPLASH:
    // hold while the macro types, then SPLASH_HOLD_MS more
    if (macro_is_running())
      splash_until_ms = now + SPLASH_HOLD_MS;
    else if ((int32_t)(now - splash_until_ms) >= 0)
      end_overlay();
    break;

  case OVERLAY_FADE_IN:
    if (elapsed >= NO_MISTAKES_FADE_IN_MS) {
      canvas_set_contrast(CANVAS_CONTRAST_FULL);
      overlay = OVERLAY_HOLD;
      phase_start_ms = now;
    } else if (now - last_fade_step_ms >= FADE_STEP_MS) {
      last_fade_step_ms = now;
      canvas_set_contrast(fade_level(elapsed, NO_MISTAKES_FADE_IN_MS));
    }
    break;

  case OVERLAY_HOLD:
    if (elapsed >= NO_MISTAKES_HOLD_MS && !macro_is_running()) {
      overlay = OVERLAY_FADE_OUT;
      phase_start_ms = now;
      last_fade_step_ms = now;
    }
    break;

  case OVERLAY_FADE_OUT:
    if (elapsed >= NO_MISTAKES_FADE_OUT_MS) {
      end_overlay();
    } else if (now - last_fade_step_ms >= FADE_STEP_MS) {
      last_fade_step_ms = now;
      canvas_set_contrast(CANVAS_CONTRAST_FULL -
                          fade_level(elapsed, NO_MISTAKES_FADE_OUT_MS));
    }
    break;

  default:
    break;
  }

  return overlay != OVERLAY_NONE;
}

void screen_prompt_cancel(void) {
  if (overlay == OVERLAY_NONE)
    return;

  overlay = OVERLAY_NONE;
  canvas_set_contrast(CANVAS_CONTRAST_FULL);
}
