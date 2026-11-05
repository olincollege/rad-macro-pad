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

static char print_data_pool[PRINT_DATA_STORAGE_SIZE];
// pointer doesnt deadvance for finished messages
static size_t current_pool_index = 0;
static size_t oldest_message_index = 0;

void usb_cdc_init(void) {
  queue_init(&print_queue, (uint)sizeof(LineUnit), QUEUE_SIZE);
}

static bool cdc_write_process(void) {
  if (!partial_packet) {
    if (queue_is_empty(&print_queue)) {
      current_pool_index = 0;
      oldest_message_index = 0;
      return true;
    }

    queue_try_remove(&print_queue, &active_packet);
    oldest_message_index = active_packet.character_data - print_data_pool;
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
    // ideally adv == cdc_queue_free_space. better to not assume however
    uint32_t adv =
        tud_cdc_write(active_packet.character_data, cdc_queue_free_space);
    active_packet.character_data += adv;
    active_packet.len -= adv;

    partial_packet = true;
  }

  tud_cdc_write_flush();

  return true;
}

// static size_t advance_line_queue(const char *str, size_t len) {
//   int a;
//   return 0;
// }

bool printf_cdc(const char *format, ...) {
  if (!tud_cdc_connected() || queue_is_full(&print_queue))
    return false;

  va_list args;
  va_start(args, format);

  // a second copy in case the message has to be formatted again at index 0
  va_list args_copy;
  va_copy(args_copy, args);

  // wrapped: new messages are already going in before the oldest unsent one
  bool wrapped = current_pool_index < oldest_message_index;

  // first try: the gap right after current_pool_index
  size_t start = current_pool_index;
  size_t space = wrapped ? oldest_message_index - current_pool_index
                         : PRINT_DATA_STORAGE_SIZE - current_pool_index;

  int length = vsnprintf(&print_data_pool[start], space, format, args);

  // second try: go back to index 0 and use the gap before the oldest message
  // length < oldest_message_index leaves room for the '\0'
  if (length >= 0 && (size_t)length >= space && !wrapped &&
      (size_t)length < oldest_message_index) {
    start = 0;
    space = oldest_message_index;
    length = vsnprintf(&print_data_pool[start], space, format, args_copy);
  }

  va_end(args_copy);
  va_end(args);

  if (length < 0 || (size_t)length >= space) // fail
    return false;

  LineUnit unit = {.character_data = &print_data_pool[start],
                   .len = (uint32_t)length};

  if (queue_try_add(&print_queue, &unit)) {
    current_pool_index = start + (size_t)length;
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

void tud_suspend_cb(bool remote_wakeup_en) {
  (void)remote_wakeup_en;
  // Flush our local structures so old prints don't instantly burst
  // out when the device is plugged back in later
  queue_free(&print_queue);
  queue_init(&print_queue, (uint)sizeof(LineUnit), QUEUE_SIZE);
  current_pool_index = 0;
  oldest_message_index = 0;
  partial_packet = false;
}