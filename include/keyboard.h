#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "kstdint.h"

typedef struct {
    int has_char;
    char ch;        /* translated ASCII when available */
    uint8_t sc;     /* raw scancode */
} key_event_t;

/* Poll the controller. Returns 1 if an event was produced. */
int keyboard_poll(key_event_t* ev);

#endif
