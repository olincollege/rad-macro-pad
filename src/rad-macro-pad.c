#include "gfx.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "pico/stdio.h"
#include "rotary-dial.h"
#include "rp2040-oled-types.h"
#include "rp2040-oled.h"
#include <pico/time.h>

extern volatile int8_t state;
extern volatile uint8_t turns_to_state_change;
extern volatile bool last_state_a;

#define DISPLAY_SCLK 26
#define DISPLAY_MOSI 27
#define DISPLAY_CHIP_SELECT 22
#define DISPLAY_DC 24
#define DISPLAY_RESET 23
#define DISPLAY_FREQ 10 * 1000 * 1000 // 10 MHz

void init(void);

void init(void) {
  stdio_init_all();
  gpio_init(ROTARY_DIAL_BUTTON);
  gpio_set_dir(ROTARY_DIAL_BUTTON, GPIO_IN);
  gpio_init(ROTARY_DIAL_ROT_A);
  gpio_set_dir(ROTARY_DIAL_ROT_A, GPIO_IN);
  gpio_init(ROTARY_DIAL_ROT_B);
  gpio_set_dir(ROTARY_DIAL_ROT_B, GPIO_IN);
  gpio_pull_up(ROTARY_DIAL_BUTTON);
  gpio_pull_up(ROTARY_DIAL_ROT_A);
  gpio_pull_up(ROTARY_DIAL_ROT_B);
}

int main() {
  init();
  // Record initial state of Rot A
  last_state_a = gpio_get(ROTARY_DIAL_ROT_A);

  gpio_set_irq_enabled_with_callback(ROTARY_DIAL_ROT_A,
                                     GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL,
                                     true, &encoder_callback);

  bool last_button_state = true;

  // the macropad's screen wiring. fields left out start at 0
  rp2040_oled_t oled = {
      .spi = spi1,
      .sck_pin = DISPLAY_SCLK,
      .mosi_pin = DISPLAY_MOSI,
      .cs_pin = DISPLAY_CHIP_SELECT,
      .dc_pin = DISPLAY_DC,
      .reset_pin = DISPLAY_RESET,
      .baudrate = DISPLAY_FREQ / 10,
      .use_doublebuf = false,
  };

  rp2040_oled_init(&oled);

  // a 40x40 outline in the middle of the screen. render = true sends it now
  rp2040_oled_draw_rectangle(&oled, 44, 12, 83, 51, OLED_COLOR_WHITE, false,
                             true);

  while (true) {

    // Small delay to prevent tight-looping the core
    sleep_ms(10);
  }
}