#include "kprintf.h"
#include "terminal.h"
#include "kstdint.h"

#include <stdarg.h>

static void print_uint(uint32_t v, uint32_t base, int upper) {
    char buf[32];
    const char* digits = upper ? "0123456789ABCDEF" : "0123456789abcdef";
    int i = 0;

    if (v == 0) {
        term_putc('0');
        return;
    }

    while (v > 0 && i < (int)sizeof(buf)) {
        buf[i++] = digits[v % base];
        v /= base;
    }
    while (i-- > 0) term_putc(buf[i]);
}

static void print_int(int32_t v) {
    if (v < 0) {
        term_putc('-');
        print_uint((uint32_t)(-v), 10, 0);
    } else {
        print_uint((uint32_t)v, 10, 0);
    }
}

void kprintf(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    for (size_t i = 0; fmt && fmt[i]; i++) {
        if (fmt[i] != '%') {
            term_putc(fmt[i]);
            continue;
        }

        i++;
        char c = fmt[i];
        if (!c) break;

        switch (c) {
            case '%': term_putc('%'); break;
            case 'c': term_putc((char)va_arg(ap, int)); break;
            case 's': {
                const char* s = va_arg(ap, const char*);
                term_write(s ? s : "(null)");
                break;
            }
            case 'd':
            case 'i': print_int(va_arg(ap, int)); break;
            case 'u': print_uint(va_arg(ap, uint32_t), 10, 0); break;
            case 'x': print_uint(va_arg(ap, uint32_t), 16, 0); break;
            case 'X': print_uint(va_arg(ap, uint32_t), 16, 1); break;
            case 'p': {
                term_write("0x");
                print_uint((uint32_t)va_arg(ap, uint32_t), 16, 0);
                break;
            }
            default:
                term_putc('%');
                term_putc(c);
                break;
        }
    }

    va_end(ap);
}
