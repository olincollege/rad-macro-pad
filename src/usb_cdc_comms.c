#include "usb_cdc_comms.h"

#include "pico/util/queue.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

#include <stdlib.h>
#include <string.h>

#include "tusb.h"

#define QUEUE_SIZE 8
#define PRINT_DATA_STORAGE_SIZE 1024

static queue_t print_queue;
static LineUnit active_packet;
static bool partial_packet = false;
// static size_t drop_counter = 0;
static char print_data_pool[PRINT_DATA_STORAGE_SIZE];
static size_t current_pool_index = 0;

void usb_cdc_init(void) {
  queue_init(&print_queue, (uint)sizeof(LineUnit), QUEUE_SIZE);
}

static bool cdc_write_process(void) {
  if (!partial_packet) {
    if (!partial_packet && queue_is_empty(&print_queue)) {
      current_pool_index = 0;
      return true;
    }

    queue_try_remove(&print_queue, &active_packet);
  }

  size_t cdc_queue_free_space = tud_cdc_write_available();
  if (cdc_queue_free_space == 0) {
    partial_packet = true;
    return false;
  }

  if (active_packet.len <= cdc_queue_free_space) {
    tud_cdc_write(active_packet.character_data, active_packet.len);
    partial_packet = false;
  } else {
    // idea here: write as much as can (cdc_queue_free_space)
    tud_cdc_write(active_packet.character_data, cdc_queue_free_space);
    active_packet.character_data += cdc_queue_free_space;
    active_packet.len -= cdc_queue_free_space;

    partial_packet = true;
  }

  tud_cdc_write_flush();

  return true;
}

static size_t advance_line_queue(const char *str, size_t len) {
  int a;
  return 0;
}

bool printf_cdc(const char *format, ...) {
  if (!tud_cdc_connected() || queue_is_full(&print_queue))
    return false;

  va_list args;
  va_start(args, format);

  size_t space_left_in_pool = PRINT_DATA_STORAGE_SIZE - current_pool_index;
  char *pool_data = &print_data_pool[current_pool_index];
  int length = vsnprintf(pool_data, space_left_in_pool, format, args);
  if (length < 0 || (size_t)length >= space_left_in_pool) // fail
    return false;

  va_end(args);

  LineUnit unit = {.character_data = pool_data, .len = (uint32_t)length};

  if (queue_try_add(&print_queue, &unit)) {
    current_pool_index += length;
    return true;
  }
  return false;
}

bool println_cdc(const char *print_data) {
  return printf_cdc("%s\r\n", print_data);
}

void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts) {
  (void)itf;
  (void)dtr;
  (void)rts;
  // great spot for funsies
  return;
}

void usb_cdc_task(void) {
  tud_task();
  cdc_write_process();
}
