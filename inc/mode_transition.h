#pragma once

#include <stdbool.h>
#include <stdint.h>

// the animation played when the dial changes mode: the old mode's name slides
// out as the new one slides in, in the direction the dial turned, then holds.
// three dots along the bottom show which mode is next

// starts (or restarts) the animation and draws its first frame
void mode_transition_start(int8_t from_mode, int8_t to_mode);

// call on every main loop pass. draws the next frame when it's due. returns
// true while the animation is still playing
bool mode_transition_task(void);

bool mode_transition_running(void);
