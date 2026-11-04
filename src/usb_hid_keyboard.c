/*
 * USB HID Keyboard implementation using TinyUSB
 */

#include "inc/usb_hid_keyboard.h"
#include "pico/stdlib.h"
#include "tusb.h"
#include "usb_descriptors.h"

#define STATUS_LED_PIN 13

// LED Blink pattern
enum {
  BLINK_NOT_MOUNTED = 250, // device not mounted (fast blink)
  BLINK_MOUNTED = 1000,    // device mounted (slow pulse)
  BLINK_SUSPENDED = 2500,  // device suspended
};

static uint32_t blink_interval_ms = BLINK_NOT_MOUNTED;

static void led_blinking_task(void) {
  static uint32_t start_ms = 0;

  if (!blink_interval_ms)
    return;

  uint32_t now = to_ms_since_boot(get_absolute_time());
  if (now - start_ms < blink_interval_ms)
    return;
  start_ms += blink_interval_ms;

  gpio_put(STATUS_LED_PIN, !gpio_get(STATUS_LED_PIN));
}

void usb_hid_init(void) {
  gpio_init(STATUS_LED_PIN);
  gpio_set_dir(STATUS_LED_PIN, GPIO_OUT);

  tusb_init();
}

void usb_hid_task(void) {
  tud_task(); // TinyUSB device background task
  led_blinking_task();
}

bool usb_hid_is_ready(void) { return tud_hid_ready(); }

void usb_hid_send_key(uint8_t modifier, uint8_t keycode) {
  if (!tud_hid_ready())
    return;

  // Remote wakeup if suspended
  if (tud_suspended()) {
    tud_remote_wakeup();
    return;
  }

  uint8_t keycodes[6] = {keycode, 0, 0, 0, 0, 0};
  tud_hid_keyboard_report(REPORT_ID_KEYBOARD, modifier, keycodes);
}

void usb_hid_release_key(void) {
  if (!tud_hid_ready())
    return;

  tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL);
}

//--------------------------------------------------------------------+
// TinyUSB HID Callbacks
//--------------------------------------------------------------------+

void tud_hid_report_complete_cb(uint8_t instance, uint8_t const *report,
                                uint16_t len) {
  (void)instance;
  (void)report;
  (void)len;
}

uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id,
                               hid_report_type_t report_type, uint8_t *buffer,
                               uint16_t reqlen) {
  (void)instance;
  (void)report_id;
  (void)report_type;
  (void)buffer;
  (void)reqlen;
  return 0;
}

void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id,
                           hid_report_type_t report_type, uint8_t const *buffer,
                           uint16_t bufsize) {
  (void)instance;

  if (report_type == HID_REPORT_TYPE_OUTPUT &&
      report_id == REPORT_ID_KEYBOARD) {
    if (bufsize < 1)
      return;

    uint8_t const kbd_leds = buffer[0];
    if (kbd_leds & KEYBOARD_LED_CAPSLOCK) {
      blink_interval_ms = 0;
      gpio_put(STATUS_LED_PIN, true);
    } else {
      gpio_put(STATUS_LED_PIN, false);
      blink_interval_ms = BLINK_MOUNTED;
    }
  }
}

//--------------------------------------------------------------------+
// TinyUSB Device Callbacks
//--------------------------------------------------------------------+

void tud_mount_cb(void) { blink_interval_ms = BLINK_MOUNTED; }

void tud_umount_cb(void) { blink_interval_ms = BLINK_NOT_MOUNTED; }

void tud_suspend_cb(bool remote_wakeup_en) {
  (void)remote_wakeup_en;
  blink_interval_ms = BLINK_SUSPENDED;
}

void tud_resume_cb(void) { blink_interval_ms = BLINK_MOUNTED; }
