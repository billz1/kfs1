#include "keyboard.h"
#include "io.h"

#define KBD_STATUS 0x64
#define KBD_DATA   0x60

/* Very small US keymap for scancode set 1 (make codes only) */
static const char keymap[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b', /* 0x0E */
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,   /* 0x1C */
    'a','s','d','f','g','h','j','k','l',';','\'','`', 0,  '\\',
    'z','x','c','v','b','n','m',',','.','/', 0,   '*', 0,   ' ',
};

static int shift = 0;

static char shift_transform(char c) {
    /* letters */
    if (c >= 'a' && c <= 'z') return (char)(c - 'a' + 'A');
    /* numbers/symbols (minimal) */
    switch (c) {
        case '1': return '!';
        case '2': return '@';
        case '3': return '#';
        case '4': return '$';
        case '5': return '%';
        case '6': return '^';
        case '7': return '&';
        case '8': return '*';
        case '9': return '(';
        case '0': return ')';
        case '-': return '_';
        case '=': return '+';
        case '[': return '{';
        case ']': return '}';
        case '\\': return '|';
        case ';': return ':';
        case '\'': return '"';
        case ',': return '<';
        case '.': return '>';
        case '/': return '?';
        case '`': return '~';
        default: return c;
    }
}

int keyboard_poll(key_event_t* ev) {
    if (!ev) return 0;
    ev->has_char = 0;
    ev->ch = 0;
    ev->sc = 0;

    uint8_t status = inb(KBD_STATUS);
    if ((status & 0x01) == 0) return 0; /* no data */

    uint8_t sc = inb(KBD_DATA);
    ev->sc = sc;

    /* break code? (key release) */
    if (sc & 0x80) {
        uint8_t make = (uint8_t)(sc & 0x7F);
        /* shift release */
        if (make == 0x2A || make == 0x36) shift = 0;
        return 1;
    }

    /* shift press */
    if (sc == 0x2A || sc == 0x36) {
        shift = 1;
        return 1;
    }

    char c = 0;
    if (sc < 128) c = keymap[sc];
    if (c) {
        if (shift) c = shift_transform(c);
        ev->has_char = 1;
        ev->ch = c;
    }

    return 1;
}
