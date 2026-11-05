add_library(rotary-dial
    rotary-dial.c
)

target_include_directories(rotary-dial
    PUBLIC
        ../inc
)

target_link_libraries(rotary-dial
    PUBLIC
        pico_stdlib
    PRIVATE
        hardware_gpio
)

add_library(key_switches
    key_switches.c
)

target_include_directories(key_switches
    PUBLIC
        ../inc
)

target_link_libraries(key_switches
    PRIVATE
        pico_stdlib
)

add_library(usb_descriptors
    usb_descriptors.c
)

target_include_directories(usb_descriptors
    PUBLIC
        ../inc
)

target_link_libraries(usb_descriptors
    PRIVATE
        tinyusb_device
)

add_library(usb_hid_keyboard
    usb_hid_keyboard.c
)

target_include_directories(usb_hid_keyboard
    PUBLIC
        ../inc
)

target_link_libraries(usb_hid_keyboard
    PUBLIC
        tinyusb_device
    PRIVATE
        pico_stdlib
        usb_descriptors
)

add_library(keystroke_injection
    keystroke_injection.c
)

target_include_directories(keystroke_injection
    PUBLIC
        ../inc
)

target_link_libraries(keystroke_injection
    PRIVATE
        usb_hid_keyboard
        pico_time
        pico_stdlib
)


add_library(usb_cdc_comms
    usb_cdc_comms.c
)

target_include_directories(usb_cdc_comms
    PUBLIC
        ../inc
)

target_link_libraries(usb_cdc_comms
    PUBLIC
        tinyusb_device
    PRIVATE
        pico_util
        usb_descriptors
)

add_library(board_state
    board_state.c
)

target_include_directories(board_state
    PUBLIC
        ../inc
)

add_library(oled_canvas
    oled_canvas.c
)

target_include_directories(oled_canvas
    PUBLIC
        ../inc
        ../rp2040-oled-spi/include
)

target_link_libraries(oled_canvas
    PUBLIC
        oled-gfx
    PRIVATE
        oled-font
        rp2040-oled
)

add_library(key_grid
    key_grid.c
)

target_include_directories(key_grid
    PUBLIC
        ../inc
        ../rp2040-oled-spi/include
)

target_link_libraries(key_grid
    PUBLIC
        oled_canvas
)

add_library(screen_numpad
    screen_numpad.c
)

target_include_directories(screen_numpad
    PUBLIC
        ../inc
        ../rp2040-oled-spi/include
)

target_link_libraries(screen_numpad
    PRIVATE
        board_state
        key_grid
        oled_canvas
)

add_library(mode_transition
    mode_transition.c
)

target_include_directories(mode_transition
    PUBLIC
        ../inc
        ../rp2040-oled-spi/include
)

target_link_libraries(mode_transition
    PRIVATE
        oled_canvas
        rotary-dial
        pico_time
)

add_library(sprites
    sprites.c
)

target_include_directories(sprites
    PUBLIC
        ../inc
)

add_library(screen_prompt
    screen_prompt.c
)

target_include_directories(screen_prompt
    PUBLIC
        ../inc
        ../rp2040-oled-spi/include
)

target_link_libraries(screen_prompt
    PRIVATE
        board_state
        key_grid
        oled_canvas
        sprites
        keystroke_injection
        usb_hid_keyboard
        pico_time
)

add_library(debug_telemetry
    debug_telemetry.c
)

target_include_directories(debug_telemetry
    PUBLIC
        ../inc
)

target_link_libraries(debug_telemetry
    PRIVATE
        board_state
        usb_cdc_comms
        rotary-dial
        pico_time
)

add_library(screen_debug
    screen_debug.c
)

target_include_directories(screen_debug
    PUBLIC
        ../inc
        ../rp2040-oled-spi/include
)

target_link_libraries(screen_debug
    PRIVATE
        board_state
        debug_telemetry
        key_grid
        oled_canvas
        rotary-dial
        pico_time
)

add_library(ui
    ui.c
)

target_include_directories(ui
    PUBLIC
        ../inc
        ../rp2040-oled-spi/include
)

target_link_libraries(ui
    PUBLIC
        oled-gfx
    PRIVATE
        oled_canvas
        screen_numpad
        screen_prompt
        screen_debug
        mode_transition
        rotary-dial
)
