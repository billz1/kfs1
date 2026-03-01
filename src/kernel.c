#include "terminal.h"
#include "kprintf.h"
#include "keyboard.h"

static void show_help(void) {
    kprintf("KFS1 bonus demo\n");
    kprintf(" - prints 42 (mandatory)\n");
    kprintf(" - scrolling + hardware cursor\n");
    kprintf(" - colors\n");
    kprintf(" - kprintf (%%s %%d %%x ...)\n");
    kprintf(" - keyboard (polling)\n");
    kprintf(" - virtual screens (F1/F2/F3)\n\n");
    kprintf("Try: type, Enter, Backspace, Tab.\n");
    kprintf("Switch screens with F1/F2/F3.\n\n");
}

void kmain(void) {
    term_init();

    /* Mandatory requirement */
    term_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    kprintf("42\n");
    term_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);

    show_help();
    kprintf("> ");

    key_event_t ev;
    while (1) {
        if (!keyboard_poll(&ev)) continue;

        /* F1/F2/F3 scancodes (set 1): 0x3B, 0x3C, 0x3D */
        if (ev.sc == 0x3B) { term_set_active(0); continue; }
        if (ev.sc == 0x3C) { term_set_active(1); continue; }
        if (ev.sc == 0x3D) { term_set_active(2); continue; }

        if (ev.has_char) {
            if (ev.ch == '\n') {
                term_putc('\n');
                term_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
                kprintf("echo: ");
                term_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
                kprintf("(type more...)\n> ");
            } else {
                term_putc(ev.ch);
            }
        }
    }
}
