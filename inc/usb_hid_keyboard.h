#ifndef USB_HID_KEYBOARD_H_
#define USB_HID_KEYBOARD_H_

#include <stdbool.h>
#include <stdint.h>
#include "tusb.h"

// Initialize USB device stack and status LED
void usb_hid_init(void);

// Must be called repeatedly in the main loop to handle USB stack events and LED blinking
void usb_hid_task(void);

// Send a keyboard keypress report (with optional modifier, e.g. KEYBOARD_MODIFIER_LEFTCTRL)
void usb_hid_send_key(uint8_t modifier, uint8_t keycode);

// Release all currently held keys
void usb_hid_release_key(void);

// Check if USB HID is ready to accept a new report
bool usb_hid_is_ready(void);

#endif /* USB_HID_KEYBOARD_H_ */

