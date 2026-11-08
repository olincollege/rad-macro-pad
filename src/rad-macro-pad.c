
#include "rp2040-oled-types.h"
#include "rp2040-oled.h"

#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "pico/stdio.h"
#include "pico/time.h"

#include "key_switches.h"
#include "keystroke_injection.h"
#include "usb_cdc_comms.h"
#include "usb_hid_keyboard.h"

#include "rotary-dial.h"

#include "board_state.h"
#include "debug_telemetry.h"
#include "ui.h"

// Callback invoked on any switch press or release
void on_key_event(unsigned int key_index, bool is_pressed) {
  if (key_index >= NUM_KEYS)
    return;

  // remember the key and update the screen before sending anything
  board_state_set_key(key_index, is_pressed);
  ui_on_key(key_index, is_pressed);

  switch (state) {
  case (NUMPAD):
    if (is_pressed) {
      usb_hid_send_key(KEY_MAP[key_index].modifier, KEY_MAP[key_index].keycode);
    } else {
      usb_hid_release_key();
    }
    break;
  case (PROMPT_ENGINEER):
    // bottom right key (12) is enter
    if (key_index == 12) {
      if (is_pressed) {
        usb_hid_send_key(0, HID_KEY_ENTER);
      } else {
        usb_hid_release_key();
      }
    }
    if (is_pressed) {
      if (key_index >= 1 && key_index <= 5) {
        inject_macro((macro_id_t)key_index);
      }
    }
    break;
  default: // state = DEBUG
           // TODO
  }
}

/**
 * @brief Brings up every peripheral.
 *
 * Starts stdio, the dial and its interrupt, the display and UI, the key
 * switches, and the USB HID and CDC stacks.
 *
 * @param oled Display to start. Its config fields must already be set.
 */
static void init(rp2040_oled_t *oled) {
  stdio_init_all();
  dial_init();
  // Record initial state of Rot A
  last_state_a = gpio_get(ROTARY_DIAL_ROT_A);

  gpio_set_irq_enabled_with_callback(ROTARY_DIAL_ROT_A,
                                     GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
                                     true, &encoder_callback);

  rp2040_oled_init(oled);
  ui_init(oled);

  // Initialize hardware drivers
  key_switch_init();
  key_switch_register_callback(on_key_event);

  // Initialize USB HID stack
  usb_hid_init();
  usb_cdc_init();
}

int main(void) {

  // the macropad's screen wiring. fields left out start at 0
  rp2040_oled_t oled = {
      .spi = spi1,
      .sck_pin = DISPLAY_SCLK,
      .mosi_pin = DISPLAY_MOSI,
      .cs_pin = DISPLAY_CHIP_SELECT,
      .dc_pin = DISPLAY_DC,
      .reset_pin = DISPLAY_RESET,
      .baudrate = DISPLAY_FREQ,
      .use_doublebuf = false,
  };

  init(&oled);

  uint32_t last_key_poll = 0;
  const uint32_t poll_interval_ms = 5; // Poll every 5ms

  while (true) {
    // redraws the screen when the mode changes
    ui_task();

    // sends the running macro a step at a time, in every mode
    macro_task();

    switch (state) {
    case NUMPAD: {
      // fall through
    }
    case PROMPT_ENGINEER: {
      // Run TinyUSB background engine continuously with 0ms latency
      usb_hid_task();

      // Non-blocking timer check for key switch polling
      uint32_t now = to_ms_since_boot(get_absolute_time());
      if (now - last_key_poll >= poll_interval_ms) {
        last_key_poll = now;
        poll_key_switch_press();
      }
      break;
    }
    case DEBUG: {
      usb_cdc_task();

      // keys are polled here too, so the debug screen and telemetry
      // see them (they don't type in this mode)
      uint32_t now = to_ms_since_boot(get_absolute_time());
      if (now - last_key_poll >= poll_interval_ms) {
        last_key_poll = now;
        poll_key_switch_press();
      }

      // prints the board's state when something changes
      debug_telemetry_task();
      break;
    }
    }
  }
  return 0;
}
