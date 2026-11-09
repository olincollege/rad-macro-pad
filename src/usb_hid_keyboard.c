/*
 * USB HID Keyboard implementation using TinyUSB
 */

#include "usb_hid_keyboard.h"
#include "pico/stdlib.h"
#include "tusb.h"
#include "usb_descriptors.h"

void usb_hid_init(void) { tusb_init(); }

void usb_hid_task(void) { tud_task(); }

bool usb_hid_is_ready(void) { return tud_hid_ready(); }

void usb_hid_send_key(uint8_t modifier, uint8_t keycode) {
  if (!tud_hid_ready())
    return;

  // Remote wakeup if suspended
  if (tud_suspended()) {
    tud_remote_wakeup();
    return;
  }

  // Send a single keycode (with modifier). USB standard can support up to 6 simultaneous keycodes, but this implmentation only permits one at a time.
  uint8_t keycodes[6] = {keycode, 0, 0, 0, 0, 0};
  tud_hid_keyboard_report(REPORT_ID_KEYBOARD, modifier, keycodes); // actual command to send USB message to host
}

void usb_hid_release_key(void) {
  if (!tud_hid_ready())
    return;

  tud_hid_keyboard_report(REPORT_ID_KEYBOARD, 0, NULL); // send an empty message to host to inform all keys are released
}

//--------------------------------------------------------------------+
// TinyUSB HID Callbacks (not used in this implementation, but tinyusb expects the functions to exist.)
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

// required by TinyUSB. the computer uses it to send lock-key lights (caps lock
// and so on), which this keyboard doesn't use
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id,
                           hid_report_type_t report_type, uint8_t const *buffer,
                           uint16_t bufsize) {
  (void)instance;
  (void)report_id;
  (void)report_type;
  (void)buffer;
  (void)bufsize;
}
