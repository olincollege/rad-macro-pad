#include "keystroke_injection.h"
#include "usb_hid_keyboard.h"
#include "pico/time.h"

#define SIZE_SCRIPT_NO_MISTAKES 17
static const key_mapping_t script_no_mistakes[SIZE_SCRIPT_NO_MISTAKES] = {
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
static const key_mapping_t script_gemini[SIZE_SCRIPT_GEMINI] = {
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
static const key_mapping_t script_claude[SIZE_SCRIPT_CLAUDE] = {
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
static const key_mapping_t script_chatgpt[SIZE_SCRIPT_CHATGPT] = {
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
static const key_mapping_t script_grok[SIZE_SCRIPT_GROK] = {
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

void send_macro_script(const key_mapping_t *script, size_t length) {
    for (size_t i = 0; i < length; i++) {
        // Wait until USB endpoint is ready
        while (!tud_hid_ready()) {
            tud_task();
        }
        usb_hid_send_key(script[i].modifier, script[i].keycode);
        tud_task();
        sleep_ms(15);

        while (!tud_hid_ready()) {
            tud_task();
        }
        usb_hid_release_key();
        tud_task();
        sleep_ms(15);
        // Additional sleep after Windows/GUI or ENTER button press
        if ((script[i].keycode == HID_KEY_GUI_LEFT) || 
            (script[i].keycode == HID_KEY_ENTER)) {
            sleep_ms(2000);
        }
    }
}

void inject_macro(macro_id_t macro_id) {
    switch (macro_id) {
        case MACRO_NO_MISTAKES:
            send_macro_script(script_no_mistakes, SIZE_SCRIPT_NO_MISTAKES);
            break;
        case MACRO_GEMINI:
            send_macro_script(script_gemini, SIZE_SCRIPT_GEMINI);
            break;
        case MACRO_CLAUDE:
            send_macro_script(script_claude, SIZE_SCRIPT_CLAUDE);
            break;
        case MACRO_CHATGPT:
            send_macro_script(script_chatgpt, SIZE_SCRIPT_CHATGPT);
            break;
        case MACRO_GROK:
            send_macro_script(script_grok, SIZE_SCRIPT_GROK);
            break;
        default:
            break;
    }
}

