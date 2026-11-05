#include "oled_canvas.h"

#include <string.h>

#include "font.h"
#include "gfx.h"
#include "rp2040-oled.h"

// font_6x8 holds printable ascii from ' ' to '~' plus one more, 5 column
// bytes each. characters are 6 wide with the blank column, and digits and
// capitals use rows 0 to 6
#define FONT_FIRST_CHAR 32
#define FONT_GLYPH_COUNT 96
#define FONT_GLYPH_COLUMNS 5
#define FONT_ADVANCE 6
#define FONT_VISIBLE_ROWS 7

static rp2040_oled_t *canvas_oled = NULL;
static uint8_t canvas[CANVAS_WIDTH * CANVAS_PAGES];

void canvas_init(rp2040_oled_t *oled) {
  canvas_oled = oled;
  canvas_clear();
}

void canvas_clear(void) { memset(canvas, 0x00, sizeof(canvas)); }

void canvas_set_pixel(int x, int y, bool on) {
  if (x < 0 || x >= CANVAS_WIDTH || y < 0 || y >= CANVAS_HEIGHT)
    return;

  uint8_t *byte = &canvas[(y / 8) * CANVAS_WIDTH + x];
  uint8_t mask = (uint8_t)(1u << (y % 8));
  if (on)
    *byte |= mask;
  else
    *byte &= (uint8_t)~mask;
}

bool canvas_get_pixel(int x, int y) {
  if (x < 0 || x >= CANVAS_WIDTH || y < 0 || y >= CANVAS_HEIGHT)
    return false;

  return (canvas[(y / 8) * CANVAS_WIDTH + x] >> (y % 8)) & 1u;
}

void canvas_fill_rect(int x, int y, int w, int h, bool on) {
  for (int yy = y; yy < y + h; yy++)
    for (int xx = x; xx < x + w; xx++)
      canvas_set_pixel(xx, yy, on);
}

void canvas_invert_rect(int x, int y, int w, int h) {
  for (int yy = y; yy < y + h; yy++)
    for (int xx = x; xx < x + w; xx++)
      canvas_set_pixel(xx, yy, !canvas_get_pixel(xx, yy));
}

void canvas_outline_rect(int x, int y, int w, int h, bool on) {
  if (w <= 0 || h <= 0)
    return;

  canvas_fill_rect(x, y, w, 1, on);         // top
  canvas_fill_rect(x, y + h - 1, w, 1, on); // bottom
  canvas_fill_rect(x, y, 1, h, on);         // left
  canvas_fill_rect(x + w - 1, y, 1, h, on); // right
}

// how much each font pixel is stretched for a text size
static void text_scale(canvas_text_size_t size, int *sx, int *sy) {
  *sx = (size == TEXT_2X) ? 2 : 1;
  *sy = (size == TEXT_1X) ? 1 : 2;
}

static void draw_char(int x, int y, char c, int sx, int sy, bool on) {
  int index = (unsigned char)c - FONT_FIRST_CHAR;
  if (index < 0 || index >= FONT_GLYPH_COUNT)
    index = '?' - FONT_FIRST_CHAR; // characters the font doesn't have

  const uint8_t *glyph = &font_6x8[index * FONT_GLYPH_COLUMNS];
  for (int col = 0; col < FONT_GLYPH_COLUMNS; col++)
    for (int row = 0; row < 8; row++)
      if ((glyph[col] >> row) & 1u)
        canvas_fill_rect(x + col * sx, y + row * sy, sx, sy, on);
}

void canvas_draw_text(int x, int y, const char *text, canvas_text_size_t size,
                      bool on) {
  int sx, sy;
  text_scale(size, &sx, &sy);

  for (int i = 0; text[i] != '\0'; i++)
    draw_char(x + i * FONT_ADVANCE * sx, y, text[i], sx, sy, on);
}

int canvas_text_width(const char *text, canvas_text_size_t size) {
  int sx, sy;
  text_scale(size, &sx, &sy);

  int len = (int)strlen(text);
  // leave out the blank column after the last character
  return len == 0 ? 0 : (len * FONT_ADVANCE - 1) * sx;
}

int canvas_text_height(canvas_text_size_t size) {
  int sx, sy;
  text_scale(size, &sx, &sy);
  return FONT_VISIBLE_ROWS * sy;
}

void canvas_draw_bitmap(int x, int y, const uint8_t *bitmap, int w, int h,
                        bool on) {
  for (int yy = 0; yy < h; yy++)
    for (int xx = 0; xx < w; xx++)
      if ((bitmap[(yy / 8) * w + xx] >> (yy % 8)) & 1u)
        canvas_set_pixel(x + xx, y + yy, on);
}

void canvas_present_rect(int x, int y, int w, int h) {
  if (canvas_oled == NULL)
    return;

  // keep the rectangle on the screen
  if (x < 0) {
    w += x;
    x = 0;
  }
  if (y < 0) {
    h += y;
    y = 0;
  }
  if (x + w > CANVAS_WIDTH)
    w = CANVAS_WIDTH - x;
  if (y + h > CANVAS_HEIGHT)
    h = CANVAS_HEIGHT - y;
  if (w <= 0 || h <= 0)
    return;

  // copy each touched page into the library's framebuffer, overwriting the
  // bytes there, then send only the columns that changed
  int first_page = y / 8;
  int last_page = (y + h - 1) / 8;
  for (int page = first_page; page <= last_page; page++)
    rp2040_oled_draw_sprite(canvas_oled, &canvas[page * CANVAS_WIDTH + x],
                            (int16_t)x, (int16_t)(page * 8), (uint8_t)w, 8,
                            OLED_COLOR_FULL_BYTE, false);

  rp2040_oled_flush(canvas_oled);
}

void canvas_present(void) {
  canvas_present_rect(0, 0, CANVAS_WIDTH, CANVAS_HEIGHT);
}

void canvas_set_contrast(uint8_t contrast) {
  if (canvas_oled != NULL)
    rp2040_oled_set_contrast(canvas_oled, contrast);
}
