#pragma once

// DEBUG mode's screen: the title, a live map of the keys, and the terminal,
// dial, button and dropped-print status

// draws the whole screen and sends it
void screen_debug_draw(void);

// call on every main loop pass while this screen is up. redraws when
// something changed, at most 20 times a second
void screen_debug_task(void);
