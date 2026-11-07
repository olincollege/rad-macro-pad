#include "debug_telemetry.h"

#include <stdio.h>

#include "board_state.h"
#include "key_switches.h"
#include "pico/time.h"
#include "rotary-dial.h"
#include "tusb.h"
#include "usb_cdc_comms.h"

// set by the dial's interrupt in rotary-dial.c
extern volatile Direction curr_dir;

// if this task wasn't called for this long, DEBUG mode was just entered
#define REENTER_GAP_MS 100
#define BLOCK_SIZE 256 // the longest block, with the intro, is about 170
#define DIAL_BUTTON_KEY 0

// the parts of the board that trigger a new block when they change
typedef struct {
  uint32_t keys_version; // includes the dial button
  uint8_t dial_steps;
  Direction dial_dir;
} snapshot_t;

static snapshot_t printed;
static bool need_print = true;
static bool retrying = false;
static bool terminal_was_open = false;
static bool called_before = false;
static uint32_t last_call_ms;
static uint32_t dropped = 0;

static snapshot_t read_snapshot(void) {
  snapshot_t s = {
      .keys_version = board_state_version(),
      .dial_steps = turns_to_state_change,
      .dial_dir = curr_dir,
  };
  return s;
}

static bool same_snapshot(const snapshot_t *a, const snapshot_t *b) {
  return a->keys_version == b->keys_version &&
         a->dial_steps == b->dial_steps && a->dial_dir == b->dial_dir;
}

// writes the whole block into `out`, so it's queued as one message
static void format_block(char *out, size_t size, const snapshot_t *s,
                         uint32_t now, bool intro) {
  int n = 0;

  if (intro)
    n += snprintf(out + n, size - n,
                  "rad macro pad debug. keys are 1 (top left) to 12 "
                  "(bottom right)\r\n");

  n += snprintf(out + n, size - n, "--- %lu.%03lu s ---\r\nkeys down:",
                (unsigned long)(now / 1000), (unsigned long)(now % 1000));

  bool any = false;
  // key 0 is the dial button, printed on its own line below
  for (unsigned key = 1; key < NUM_KEYS; key++) {
    if (board_state_key(key)) {
      n += snprintf(out + n, size - n, " %u", key);
      any = true;
    }
  }
  if (!any)
    n += snprintf(out + n, size - n, " none");

  snprintf(out + n, size - n, "\r\ndial: %u/%u %s\r\nbutton: %s\r\n",
           (unsigned)s->dial_steps, (unsigned)MAX_STEPS,
           s->dial_dir == CLOCKWISE ? "cw" : "ccw",
           board_state_key(DIAL_BUTTON_KEY) ? "down" : "up");
}

void debug_telemetry_task(void) {
  uint32_t now = to_ms_since_boot(get_absolute_time());

  // print the current state each time DEBUG mode starts
  if (!called_before || now - last_call_ms > REENTER_GAP_MS)
    need_print = true;
  called_before = true;
  last_call_ms = now;

  // and when a terminal opens, with a one line intro
  bool open = tud_cdc_connected();
  bool just_opened = open && !terminal_was_open;
  terminal_was_open = open;
  if (just_opened)
    need_print = true;

  snapshot_t current = read_snapshot();
  if (!same_snapshot(&current, &printed))
    need_print = true;

  if (!need_print || !open)
    return;

  char block[BLOCK_SIZE];
  format_block(block, sizeof(block), &current, now, just_opened);

  // only marked as printed once the queue takes it. otherwise it's retried on
  // a later pass, with the latest state
  if (printf_cdc("%s", block)) {
    printed = current;
    need_print = false;
    retrying = false;
  } else if (!retrying) {
    dropped++;
    retrying = true;
  }
}

bool debug_telemetry_terminal_open(void) { return tud_cdc_connected(); }

uint32_t debug_telemetry_dropped(void) { return dropped; }
