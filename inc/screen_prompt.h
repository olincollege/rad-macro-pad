#pragma once

#include <stdbool.h>

// PROMPT ENGINEER mode: a grid of AI tool icons. pressing a tool shows its
// logo full screen while its macro runs, and key 1 shows "MAKE NO MISTAKES."
// fading in

// draws the grid from the current key states and sends it
void screen_prompt_draw(void);

// call from the key callback while this screen is up, before the macro starts
void screen_prompt_on_key(unsigned key, bool pressed);

// call on every main loop pass while this screen is up. runs the splash and
// fade timers, and returns true while one of them owns the screen
bool screen_prompt_task(void);

// stops any splash or fade right away and restores full brightness. used when
// the dial leaves this mode
void screen_prompt_cancel(void);
