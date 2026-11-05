#pragma once

#include "tusb.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct line_unit_t {
  const char *character_data;
  uint32_t len;
} LineUnit;

void usb_cdc_task(void);

void usb_cdc_init(void);

bool println_cdc(const char *print_data);

bool printf_cdc(const char *format, ...);
