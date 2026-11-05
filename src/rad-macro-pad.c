
#include <stdio.h>

#include "gfx.h"
#include "rp2040-oled-types.h"
#include "rp2040-oled.h"

#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "pico/stdio.h"
#include "pico/stdlib.h"
#include "pico/time.h"

#include "key_switches.h"
#include "usb_hid_keyboard.h"

#include "rotary-dial.h"

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

// Key mapping struct for each switch index
typedef struct {
  uint8_t modifier; // e.g. KEYBOARD_MODIFIER_LEFTCTRL, or 0
  uint8_t keycode;  // TinyUSB HID keycode (HID_KEY_*)
} key_mapping_t;

#define SIZE_SCRIPT_NO_MISTAKES 17
key_mapping_t script_no_mistakes[SIZE_SCRIPT_NO_MISTAKES] = {
    {0, HID_KEY_M},
    {0, HID_KEY_A},
    {0, HID_KEY_K},
    {0, HID_KEY_E},
    {0, HID_KEY_SPACE},
    {0, HID_KEY_N},
    {0, HID_KEY_O},
    {0, HID_KEY_SPACE},
    {0, HID_KEY_M},
    {0, HID_KEY_I},
    {0, HID_KEY_S},
    {0, HID_KEY_T},
    {0, HID_KEY_A},
    {0, HID_KEY_K},
    {0, HID_KEY_E},
    {0, HID_KEY_S},
    {0, HID_KEY_PERIOD},
    
};

#define SIZE_SCRIPT_GEMINI 28
key_mapping_t script_gemini[SIZE_SCRIPT_GEMINI] = {
    {0, HID_KEY_GUI_LEFT},
    {0, HID_KEY_F},
    {0, HID_KEY_I},
    {0, HID_KEY_R},
    {0, HID_KEY_E},
    {0, HID_KEY_F},
    {0, HID_KEY_O},
    {0, HID_KEY_X}, 
    {0, HID_KEY_ENTER},
    {KEYBOARD_MODIFIER_LEFTCTRL, HID_KEY_T},
    {0, HID_KEY_G},
    {0, HID_KEY_E},
    {0, HID_KEY_M},
    {0, HID_KEY_I},
    {0, HID_KEY_N},
    {0, HID_KEY_I},
    {0, HID_KEY_PERIOD},
    {0, HID_KEY_G},
    {0, HID_KEY_O},
    {0, HID_KEY_O},
    {0, HID_KEY_G},
    {0, HID_KEY_L},
    {0, HID_KEY_E},
    {0, HID_KEY_PERIOD},
    {0, HID_KEY_C},
    {0, HID_KEY_O},
    {0, HID_KEY_M},
    {0, HID_KEY_ENTER},
};

#define SIZE_SCRIPT_CLAUDE 20
const key_mapping_t script_claude[SIZE_SCRIPT_CLAUDE] = {
    {0, HID_KEY_GUI_LEFT},
    {0, HID_KEY_F},
    {0, HID_KEY_I},
    {0, HID_KEY_R},
    {0, HID_KEY_E},
    {0, HID_KEY_F},
    {0, HID_KEY_O},
    {0, HID_KEY_X},
    {0, HID_KEY_ENTER},
    {KEYBOARD_MODIFIER_LEFTCTRL, HID_KEY_T},
    {0, HID_KEY_C},
    {0, HID_KEY_L},
    {0, HID_KEY_A},
    {0, HID_KEY_U},
    {0, HID_KEY_D},
    {0, HID_KEY_E},
    {0, HID_KEY_PERIOD},
    {0, HID_KEY_A},
    {0, HID_KEY_I},
    {0, HID_KEY_ENTER},
};

#define SIZE_SCRIPT_CHATGPT 22
const key_mapping_t script_chatgpt[SIZE_SCRIPT_CHATGPT] = {
    {0, HID_KEY_GUI_LEFT},
    {0, HID_KEY_F},
    {0, HID_KEY_I},
    {0, HID_KEY_R},
    {0, HID_KEY_E},
    {0, HID_KEY_F},
    {0, HID_KEY_O},
    {0, HID_KEY_X},
    {0, HID_KEY_ENTER},
    {KEYBOARD_MODIFIER_LEFTCTRL, HID_KEY_T},
    {0, HID_KEY_C},
    {0, HID_KEY_H},
    {0, HID_KEY_A},
    {0, HID_KEY_T},
    {0, HID_KEY_G},
    {0, HID_KEY_P},
    {0, HID_KEY_T},
    {0, HID_KEY_PERIOD},
    {0, HID_KEY_C},
    {0, HID_KEY_O},
    {0, HID_KEY_M},
    {0, HID_KEY_ENTER},
};

#define SIZE_SCRIPT_GROK 19
const key_mapping_t script_grok[SIZE_SCRIPT_GROK] = {
    {0, HID_KEY_GUI_LEFT},
    {0, HID_KEY_F},
    {0, HID_KEY_I},
    {0, HID_KEY_R},
    {0, HID_KEY_E},
    {0, HID_KEY_F},
    {0, HID_KEY_O},
    {0, HID_KEY_X},
    {0, HID_KEY_ENTER},
    {KEYBOARD_MODIFIER_LEFTCTRL, HID_KEY_T},
    {0, HID_KEY_G},
    {0, HID_KEY_R},
    {0, HID_KEY_O},
    {0, HID_KEY_K},
    {0, HID_KEY_PERIOD},
    {0, HID_KEY_C},
    {0, HID_KEY_O},
    {0, HID_KEY_M},
    {0, HID_KEY_ENTER},
};

// Map GPIO 1 to 12 to USB keys
static const key_mapping_t KEY_MAP[NUM_KEYS] = {
    [1] = {0, HID_KEY_7},
    [2] = {0, HID_KEY_8},
    [3] = {0, HID_KEY_9},
    [4] = {0, HID_KEY_4},
    [5] = {0, HID_KEY_5},
    [6] = {0, HID_KEY_6},
    [7] = {0, HID_KEY_1},
    [8] = {0, HID_KEY_2},
    [9] = {0, HID_KEY_3},
    [10] = {KEYBOARD_MODIFIER_LEFTCTRL, HID_KEY_C}, // Crtl + C
    [11] = {0, HID_KEY_0},
    [12] = {KEYBOARD_MODIFIER_LEFTCTRL, HID_KEY_V}, // Ctrl + V
};

// Send sequence of keystrokes
void send_macro_script(const key_mapping_t *script, size_t length) {
    for (size_t i = 0; i < length; i++) {
        // Wait until USB endpoint is ready
        while (!tud_hid_ready()) {
            tud_task();
        }
        usb_hid_send_key(script[i].modifier, script[i].keycode);
        tud_task();
        // additional sleep after windows/GUI or ENTER button press
        if (
            (script[i].keycode == HID_KEY_GUI_LEFT) || 
            (script[i].keycode == HID_KEY_ENTER))
        {
            sleep_ms(2500);
        }
        sleep_ms(10);
        while (!tud_hid_ready()) {
            tud_task();
        }
        usb_hid_release_key();
        tud_task();
        sleep_ms(10);
    }
}

// Callback invoked on any switch press or release
void on_key_event(unsigned int key_index, bool is_pressed) {
  if (key_index >= NUM_KEYS)
    return;

  switch(state) {
    case(NUMPAD):
      if (is_pressed) {
        usb_hid_send_key(KEY_MAP[key_index].modifier, KEY_MAP[key_index].keycode);
      } else {
        usb_hid_release_key();
      }
      break;
        case(PROMPT_ENGINEER):
      if (is_pressed) {
        if (key_index == 1) {
          send_macro_script(script_no_mistakes, SIZE_SCRIPT_NO_MISTAKES);
        } else if (key_index == 2) {
          send_macro_script(script_gemini, SIZE_SCRIPT_GEMINI); // Gemini
        } else if (key_index == 3) {
          send_macro_script(script_claude, SIZE_SCRIPT_CLAUDE); // Claude
        } else if (key_index == 4) {
          send_macro_script(script_chatgpt, SIZE_SCRIPT_CHATGPT); // ChatGPT
        } else if (key_index == 5) {
          send_macro_script(script_grok, SIZE_SCRIPT_GROK); // Grok
        }
      }
      break;
    default: // state = DEBUG
      // TODO
  }  
}

int main(void) {

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

  // Initialize hardware drivers
  key_switch_init();
  key_switch_register_callback(on_key_event);

  // Initialize USB HID stack
  usb_hid_init();

  uint32_t last_key_poll = 0;
  const uint32_t poll_interval_ms = 5; // Poll every 5ms
  while (true) {
    // Run TinyUSB background engine continuously with 0ms latency
    usb_hid_task();
    // Non-blocking timer check for key switch polling
    uint32_t now = to_ms_since_boot(get_absolute_time());
    if (now - last_key_poll >= poll_interval_ms) {
      last_key_poll = now;
      poll_key_switch_press();
    }
  }
  return 0;
}
