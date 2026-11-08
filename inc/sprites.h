/**
 * @file sprites.h
 * @brief Pixel art for PROMPT ENGINEER mode.
 *
 * All sprites are in page format (see oled_canvas.h) and drawn with
 * canvas_draw_bitmap().
 */
#pragma once

#include <stdint.h>

/** @brief Width of the small grid icons, in pixels. */
#define SPRITE_SMALL_WIDTH 13
/** @brief Height of the small grid icons, in pixels. */
#define SPRITE_SMALL_HEIGHT 11
/** @brief Grid icon for key 1 ("make no mistakes"). */
extern const uint8_t sprite_eye_small[];
/** @brief Grid icon for Gemini. */
extern const uint8_t sprite_gemini_small[];
/** @brief Grid icon for Claude. */
extern const uint8_t sprite_claude_small[];
/** @brief Grid icon for ChatGPT. */
extern const uint8_t sprite_chatgpt_small[];
/** @brief Grid icon for Grok. */
extern const uint8_t sprite_grok_small[];

/** @brief Width of the full-height splash logos, in pixels. */
#define SPRITE_LARGE_WIDTH 64
/** @brief Height of the full-height splash logos, in pixels. */
#define SPRITE_LARGE_HEIGHT 64
/** @brief Splash logo for Gemini. */
extern const uint8_t sprite_gemini_large[];
/** @brief Splash logo for Claude. */
extern const uint8_t sprite_claude_large[];
/** @brief Splash logo for ChatGPT. */
extern const uint8_t sprite_chatgpt_large[];
/** @brief Splash logo for Grok. */
extern const uint8_t sprite_grok_large[];
