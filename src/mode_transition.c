#include "mode_transition.h"

#include "oled_canvas.h"
#include "pico/time.h"
#include "rotary-dial.h"

#define SLIDE_MS 300 // how long the names take to slide
#define HOLD_MS 500  // how long the new name stays before its screen appears
#define FRAME_MS 40  // time between frames, about 25 per second

#define TITLE_AREA_HEIGHT 56 // names are centered above the dots
#define TITLE_LINE_GAP 4
#define DOT_SIZE 4
#define DOT_GAP 6
#define DOT_Y 58
#define MODE_COUNT 3

static bool running = false;
static int8_t from_mode;
static int8_t to_mode;
static uint32_t start_ms;
static uint32_t last_frame_ms;
static bool final_frame_drawn;

static uint32_t now_ms(void) { return to_ms_since_boot(get_absolute_time()); }

// the mode's name in big text, centered, moved sideways by offset_x
static void draw_title(int8_t mode, int offset_x) {
  const char *lines[2];
  int count = 1;

  switch (mode) {
  case NUMPAD:
    lines[0] = "NUMPAD";
    break;
  case PROMPT_ENGINEER:
    lines[0] = "PROMPT";
    lines[1] = "ENGINEER";
    count = 2;
    break;
  default:
    lines[0] = "DEBUG";
    break;
  }

  int line_height = canvas_text_height(TEXT_2X);
  int total_height = count * line_height + (count - 1) * TITLE_LINE_GAP;
  int y = (TITLE_AREA_HEIGHT - total_height) / 2;

  for (int i = 0; i < count; i++) {
    int x = offset_x + (CANVAS_WIDTH - canvas_text_width(lines[i], TEXT_2X)) / 2;
    canvas_draw_text(x, y + i * (line_height + TITLE_LINE_GAP), lines[i],
                     TEXT_2X, true);
  }
}

// one dot per mode, left to right. the current one is filled
static void draw_dots(int8_t mode) {
  int total_width = MODE_COUNT * DOT_SIZE + (MODE_COUNT - 1) * DOT_GAP;
  int x = (CANVAS_WIDTH - total_width) / 2;

  for (int i = 0; i < MODE_COUNT; i++) {
    if (i == mode - NUMPAD)
      canvas_fill_rect(x, DOT_Y, DOT_SIZE, DOT_SIZE, true);
    else
      canvas_outline_rect(x, DOT_Y, DOT_SIZE, DOT_SIZE, true);
    x += DOT_SIZE + DOT_GAP;
  }
}

static void draw_frame(uint32_t elapsed_ms) {
  canvas_clear();

  if (elapsed_ms >= SLIDE_MS) {
    draw_title(to_mode, 0);
  } else {
    // starts fast and slows into place
    float progress = (float)elapsed_ms / SLIDE_MS;
    float remaining = 1.0f - progress;
    float eased = 1.0f - remaining * remaining * remaining;
    int shift = (int)(eased * CANVAS_WIDTH);

    // moving forward slides everything left, moving back slides it right
    int direction = (to_mode > from_mode) ? -1 : 1;
    draw_title(from_mode, direction * shift);
    draw_title(to_mode, direction * (shift - CANVAS_WIDTH));
  }

  draw_dots(to_mode);
  canvas_present();
}

void mode_transition_start(int8_t from, int8_t to) {
  from_mode = from;
  to_mode = to;
  start_ms = now_ms();
  last_frame_ms = start_ms;
  final_frame_drawn = false;
  running = true;
  draw_frame(0);
}

bool mode_transition_task(void) {
  if (!running)
    return false;

  uint32_t now = now_ms();
  uint32_t elapsed = now - start_ms;

  if (elapsed >= SLIDE_MS + HOLD_MS) {
    running = false;
    return false;
  }

  // while holding, the picture doesn't change, so draw it only once
  if (elapsed >= SLIDE_MS) {
    if (!final_frame_drawn) {
      draw_frame(elapsed);
      final_frame_drawn = true;
    }
    return true;
  }

  if (now - last_frame_ms >= FRAME_MS) {
    last_frame_ms = now;
    draw_frame(elapsed);
  }
  return true;
}

bool mode_transition_running(void) { return running; }
