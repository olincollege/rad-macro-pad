#include <stdio.h>
#include <stdbool.h>
#include "pico/stdlib.h"

bool flag_key_1 = false;
bool flag_key_2 = false;
bool flag_key_3 = false;
bool flag_key_4 = false;
bool flag_key_5 = false;
bool flag_key_6 = false;
bool flag_key_7 = false;
bool flag_key_8 = false;
bool flag_key_9 = false;
bool flag_key_10 = false;
bool flag_key_11 = false;
bool flag_key_12 = false;

void gpio_callback(uint gpio, uint32_t event_mask){
    switch(gpio){
        case 1:
            flag_key_1 = true;
            break;
        case 2:
            flag_key_2 = true;
            break;
        case 3:
            flag_key_3 = true;
            break;
        case 4:
            flag_key_4 = true;
            break;
        case 5:
            flag_key_5 = true;
            break;
        case 6:
            flag_key_6 = true;
            break;
        case 7:
            flag_key_7 = true;
            break;
        case 8:
            flag_key_8 = true;
            break;
        case 9:
            flag_key_9 = true;
            break;
        case 10:
            flag_key_10 = true;
            break;
        case 11:
            flag_key_11 = true;
            break;
        case 12:
            flag_key_12 = true;
            break;
    }
}


int key_switch_init(void) {
    // Apply to GPIO1 to GPIO12
    for (uint i = 1; i <= 12; i++){
        gpio_init(i); // Enable I/O and set function to SIO
        gpio_set_dir(i, false); // Set direction as input
        gpio_pull_up(i); // Enable the internal pull up resistor
        gpio_set_irq_enabled(i, GPIO_IRQ_EDGE_FALL, true); // Configure and enable the interrupt
    }
    gpio_set_irq_callback(&gpio_callback);
    irq_set_enabled(IO_IRQ_BANK0, true);
    
    return PICO_OK;
}