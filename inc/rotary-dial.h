/**
 * @file rotary-dial.h
 * @brief The rotary dial: its pins, and the mode it selects.
 *
 * Turning the dial MAX_STEPS steps in a row in one direction moves to the
 * next or previous mode. The mode stops at NUMPAD and DEBUG and doesn't wrap.
 */
#pragma once

#include "pico/types.h"

/** @brief GPIO for the dial's push button (active low). */
#define ROTARY_DIAL_BUTTON 0
/** @brief GPIO for the encoder's A channel. */
#define ROTARY_DIAL_ROT_A 17
/** @brief GPIO for the encoder's B channel. */
#define ROTARY_DIAL_ROT_B 18

/** @brief Steps in a row in one direction needed to change mode. */
#define MAX_STEPS 3

/**
 * @brief The modes, in dial order.
 *
 * Constants for `state`, which is an int8_t so it stays a single byte for the
 * interrupt to write.
 */
enum {
  NUMPAD = -1,         // Keys type digits, copy and paste
  PROMPT_ENGINEER = 0, // Keys run AI prompt macros
  DEBUG = 1            // Board state over serial and on screen
};

/** @brief Which way the dial last turned. */
typedef enum direction_t {
  CLOCKWISE = 0,        // Clockwise
  COUNTER_CLOCKWISE = 1 // Counter-clockwise
} Direction;

/** @brief Current mode (NUMPAD, PROMPT_ENGINEER or DEBUG). Set by the IRQ. */
extern volatile int8_t state;
/** @brief Steps taken in a row toward the next mode change. */
extern volatile uint8_t turns_to_state_change;
/** @brief Level of the A channel at the last interrupt. */
extern volatile bool last_state_a;

/**
 * @brief GPIO interrupt callback for the rotary dial.
 *
 * Reads the encoder on each edge of the A channel, counts steps in the same
 * direction, and changes `state` after MAX_STEPS of them. A step in the other
 * direction restarts the count.
 *
 * @param gpio Pin that triggered the interrupt (unused).
 * @param event_mask Which edges triggered it (unused).
 * @note Matches gpio_irq_callback_t, so it can be passed to
 *       gpio_set_irq_enabled_with_callback().
 */
void encoder_callback(uint gpio, uint32_t event_mask);

/** @brief Sets up the button and encoder pins as inputs with pull-ups. */
void dial_init(void);
