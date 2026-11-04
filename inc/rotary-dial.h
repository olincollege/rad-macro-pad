#pragma once

#include "pico/types.h"

// GPIO pins
#define ROTARY_DIAL_BUTTON 0
#define ROTARY_DIAL_ROT_A 17
#define ROTARY_DIAL_ROT_B 18

// constants
#define MAX_STEPS 4

enum { NUMPAD = -1, PROMPT_ENGINEER = 0, DEBUG = 1 };

typedef enum direction_t { CLOCKWISE = 0, COUNTER_CLOCKWISE = 1 } Direction;

// global variables
extern volatile int8_t state;
extern volatile uint8_t turns_to_state_change;
extern volatile bool last_state_a;

/***
 * Callback for the rotary dial. Performs state changes based
 * on an amount of steps in a certain direction it takes.
 * Parameters are based on a GPIO IRQ callback (gpio_irq_callback_t).
 */
void encoder_callback(uint gpio, uint32_t event_mask);