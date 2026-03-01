#ifndef TERMINAL_H
#define TERMINAL_H

#include "kstdint.h"

enum vga_color {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN = 14,
    VGA_COLOR_WHITE = 15,
};

typedef struct {
    uint16_t buffer[80 * 25];
    uint8_t color;
    size_t row;
    size_t col;
} terminal_t;

void term_init(void);
void term_set_active(int idx);
int  term_active(void);

void term_set_color(uint8_t fg, uint8_t bg);
void term_clear(void);
void term_putc(char c);
void term_write(const char* s);
void term_writeln(const char* s);

void term_move_cursor(size_t row, size_t col);

#endif
