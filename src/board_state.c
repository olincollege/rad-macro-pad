/**
 * @file board_state.c
 * @brief Key state tracking.
 */
#include "board_state.h"

#include "key_switches.h"
#include "pico/types.h"

static bool key_pressed[NUM_KEYS] = {false};
static uint32_t version = 0;

void board_state_set_key(uint key, bool pressed) {
  if (key >= NUM_KEYS || key_pressed[key] == pressed)
    return;

  key_pressed[key] = pressed;
  version++;
}

bool board_state_key(uint key) {
  return key < NUM_KEYS ? key_pressed[key] : false;
}

uint32_t board_state_version(void) { return version; }
