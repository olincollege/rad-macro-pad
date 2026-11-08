/**
 * @file board_state.h
 * @brief Which keys are held down right now.
 *
 * Key 0 is the dial button, and keys 1 to 12 are the key switches. The state
 * is fed from main's key callback.
 */
#pragma once

#include "pico/types.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Records a key press or release.
 *
 * Bumps the version only if the key's state actually changed.
 *
 * @param key Key index, 0 (dial button) to 12. Out-of-range keys are ignored.
 * @param pressed true if the key is down.
 */
void board_state_set_key(uint key, bool pressed);

/**
 * @brief Tells whether a key is held down.
 *
 * @param key Key index, 0 (dial button) to 12.
 * @return true if the key is down. false for out-of-range keys.
 */
bool board_state_key(uint key);

/**
 * @brief Counter that goes up by one every time any key changes.
 *
 * Other code saves this and compares later to tell when something is new.
 *
 * @return The current version.
 */
uint32_t board_state_version(void);
