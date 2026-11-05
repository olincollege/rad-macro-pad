#pragma once

#include <stdint.h>

// pixel art in page format (see oled_canvas.h), drawn with
// canvas_draw_bitmap

// grid icons for PROMPT ENGINEER mode
#define SPRITE_SMALL_WIDTH 13
#define SPRITE_SMALL_HEIGHT 11
extern const uint8_t sprite_eye_small[];
extern const uint8_t sprite_gemini_small[];
extern const uint8_t sprite_claude_small[];
extern const uint8_t sprite_chatgpt_small[];
extern const uint8_t sprite_grok_small[];

// full height logos for the splash screens
#define SPRITE_LARGE_WIDTH 64
#define SPRITE_LARGE_HEIGHT 64
extern const uint8_t sprite_gemini_large[];
extern const uint8_t sprite_claude_large[];
extern const uint8_t sprite_chatgpt_large[];
extern const uint8_t sprite_grok_large[];
