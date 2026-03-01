#include "terminal.h"
#include "io.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEM ((volatile uint16_t*)0xB8000)

static inline uint8_t vga_entry_color(uint8_t fg, uint8_t bg) {
    return (uint8_t)(fg | (bg << 4));
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
    return (uint16_t)uc | ((uint16_t)color << 8);
}

static terminal_t g_terms[3];
static int g_active = 0;

static void hw_cursor_update(size_t row, size_t col) {
    uint16_t pos = (uint16_t)(row * VGA_WIDTH + col);
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

static void flush_active(void) {
    terminal_t* t = &g_terms[g_active];
    for (size_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        VGA_MEM[i] = t->buffer[i];
    }
    hw_cursor_update(t->row, t->col);
}

static void scroll_if_needed(terminal_t* t) {
    if (t->row < VGA_HEIGHT) return;

    /* scroll up by one line */
    for (size_t r = 1; r < VGA_HEIGHT; r++) {
        for (size_t c = 0; c < VGA_WIDTH; c++) {
            t->buffer[(r - 1) * VGA_WIDTH + c] = t->buffer[r * VGA_WIDTH + c];
        }
    }
    /* clear last line */
    for (size_t c = 0; c < VGA_WIDTH; c++) {
        t->buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + c] = vga_entry(' ', t->color);
    }
    t->row = VGA_HEIGHT - 1;
}

static void put_at(terminal_t* t, char c, size_t row, size_t col) {
    t->buffer[row * VGA_WIDTH + col] = vga_entry((unsigned char)c, t->color);
}

void term_init(void) {
    for (int i = 0; i < 3; i++) {
        terminal_t* t = &g_terms[i];
        t->color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        t->row = 0;
        t->col = 0;
        for (size_t y = 0; y < VGA_HEIGHT; y++) {
            for (size_t x = 0; x < VGA_WIDTH; x++) {
                t->buffer[y * VGA_WIDTH + x] = vga_entry(' ', t->color);
            }
        }
    }
    g_active = 0;
    flush_active();
}

int term_active(void) {
    return g_active;
}

void term_set_active(int idx) {
    if (idx < 0 || idx >= 3) return;
    g_active = idx;
    flush_active();
}

void term_set_color(uint8_t fg, uint8_t bg) {
    terminal_t* t = &g_terms[g_active];
    t->color = vga_entry_color(fg, bg);
}

void term_clear(void) {
    terminal_t* t = &g_terms[g_active];
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            t->buffer[y * VGA_WIDTH + x] = vga_entry(' ', t->color);
        }
    }
    t->row = 0;
    t->col = 0;
    flush_active();
}

void term_move_cursor(size_t row, size_t col) {
    terminal_t* t = &g_terms[g_active];
    if (row >= VGA_HEIGHT) row = VGA_HEIGHT - 1;
    if (col >= VGA_WIDTH) col = VGA_WIDTH - 1;
    t->row = row;
    t->col = col;
    flush_active();
}

void term_putc(char c) {
    terminal_t* t = &g_terms[g_active];

    if (c == '\n') {
        t->col = 0;
        t->row++;
        scroll_if_needed(t);
        flush_active();
        return;
    }
    if (c == '\r') {
        t->col = 0;
        flush_active();
        return;
    }
    if (c == '\b') {
        if (t->col > 0) {
            t->col--;
        } else if (t->row > 0) {
            t->row--;
            t->col = VGA_WIDTH - 1;
        }
        put_at(t, ' ', t->row, t->col);
        flush_active();
        return;
    }
    if (c == '\t') {
        size_t next = (t->col + 4) & ~(size_t)3;
        while (t->col < next) {
            term_putc(' ');
        }
        return;
    }

    put_at(t, c, t->row, t->col);
    t->col++;
    if (t->col >= VGA_WIDTH) {
        t->col = 0;
        t->row++;
        scroll_if_needed(t);
    }
    flush_active();
}

void term_write(const char* s) {
    for (size_t i = 0; s && s[i]; i++) {
        term_putc(s[i]);
    }
}

void term_writeln(const char* s) {
    term_write(s);
    term_putc('\n');
}
