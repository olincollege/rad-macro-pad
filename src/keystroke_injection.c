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

#define KEY_HOLD_MS 15    // how long each key stays pressed
#define KEY_GAP_MS 15     // pause after each release
#define LONG_PAUSE_MS 2000 // extra pause after GUI or ENTER, so apps can open

// each key goes through these steps in order, then the next key starts
typedef enum {
    STEP_PRESS,   // waiting for the endpoint, then press
    STEP_HOLD,    // holding the key for KEY_HOLD_MS
    STEP_RELEASE, // waiting for the endpoint, then release
    STEP_GAP,     // waiting before the next key
} macro_step_t;

static const key_mapping_t *active_script = NULL; // NULL = no macro running
static size_t active_length = 0;
static size_t active_index = 0;
static macro_step_t active_step = STEP_PRESS;
static absolute_time_t wait_until;

// starts a macro. macro_task() sends it over the next few passes of the loop
void send_macro_script(const key_mapping_t *script, size_t length) {
    if (active_script != NULL || script == NULL || length == 0)
        return; // a macro is already running, or there's nothing to send

    active_script = script;
    active_length = length;
    active_index = 0;
    active_step = STEP_PRESS;
}

bool macro_is_running(void) { return active_script != NULL; }

// does at most one step and returns, so the main loop never waits here
void macro_task(void) {
    if (active_script == NULL)
        return;

    const key_mapping_t *key = &active_script[active_index];

    switch (active_step) {
    case STEP_PRESS:
        if (!tud_hid_ready())
            return;
        usb_hid_send_key(key->modifier, key->keycode);
        wait_until = make_timeout_time_ms(KEY_HOLD_MS);
        active_step = STEP_HOLD;
        break;

    case STEP_HOLD:
        if (!time_reached(wait_until))
            return;
        active_step = STEP_RELEASE;
        break;

    case STEP_RELEASE: {
        if (!tud_hid_ready())
            return;
        usb_hid_release_key();
        uint32_t gap_ms = KEY_GAP_MS;
        // Additional pause after Windows/GUI or ENTER button press
        if ((key->keycode == HID_KEY_GUI_LEFT) ||
            (key->keycode == HID_KEY_ENTER)) {
            gap_ms += LONG_PAUSE_MS;
        }
        wait_until = make_timeout_time_ms(gap_ms);
        active_step = STEP_GAP;
        break;
    }

    case STEP_GAP:
        if (!time_reached(wait_until))
            return;
        active_index++;
        if (active_index >= active_length)
            active_script = NULL; // finished
        active_step = STEP_PRESS;
        break;
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

