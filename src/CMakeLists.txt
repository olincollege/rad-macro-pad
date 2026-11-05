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
    PUBLIC
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