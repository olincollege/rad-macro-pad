# rad-macro-pad
A macropad, where the drivers are entirely written in C. Does 'rad' things.

![Adafruit MacroPad RP2040](media/macro_pad.png)

## Project Goals
The goal of this project was to re-program the [Adafruit RP2040 MacroPad](https://www.adafruit.com/product/5128?srsltid=AYCX9vKLo7Zc8QMYrQVxFYPImh0nbvcx8zGKFx6cK3LFHbl48LL_HUkB). Adafruit ships this macropad with a firmware build that users can modify through CircuitPython scripts. Instead of building on top of this framework, our team developed a brand-new C firmware architecture from scratch, built with the pico-sdk, and flashed directly to the onboard rp2040 microcontroller.

We aimed to re-create the core functionality one would expect from a macropad as well as add a few fun and unexpected features! The final list of features implemented is:
- Register key presses 
- Switch “modes” via the rotary dial
- Display the current mode on the screen
- Send key presses to a host via USB
- Inject keystrokes over USB
- Create a USB debug tool

## System Architecture
![System Architecture and state diagram](media/architecture.png)

## Future Work
There were a few cool features that we didn't have a chance to implement within the scope of this project:
- RGB LEDs 
- Buzzer 
- Mouse input to a host

## AI Usage
Most of the project was coded by hand. Display aesthetics were coded using generative AI. Generative AI was used for code/architecture review and debugging.
