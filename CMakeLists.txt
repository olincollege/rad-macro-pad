# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.3.1)
set(toolchainVersion 15_2_Rel1)
set(picotoolVersion 2.3.1)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(rad-macro-pad C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

add_subdirectory(rp2040-oled-spi)

add_subdirectory(src)

# Add executable. Default name is the project name, version 0.1

add_executable(rad-macro-pad
        src/rad-macro-pad.c
)

target_include_directories(rad-macro-pad
    PUBLIC
        inc
        rp2040-oled-spi/include
)

pico_set_program_name(rad-macro-pad "rad-macro-pad")
pico_set_program_version(rad-macro-pad "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(rad-macro-pad 0)
pico_enable_stdio_usb(rad-macro-pad 0)

# Add the standard library to the build

#include "gfx.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "inc/key_switches.h"
#include "inc/usb_hid_keyboard.h"
#include "pico/stdio.h"
#include "pico/stdlib.h"
#include "rotary-dial.h"
#include "rp2040-oled-types.h"
#include "rp2040-oled.h"
#include <pico/time.h>
#include <stdio.h>

target_link_libraries(rad-macro-pad
    PUBLIC
        oled-gfx
        rp2040-oled
        hardware_gpio
        hardware_spi
        pico_stdio
        pico_stdlib
        key_switches
        usb_hid_keyboard
        usb_cdc_comms
        keystroke_injection
        rotary-dial
        pico_time
)

pico_add_extra_outputs(rad-macro-pad)
