//
// Created by liulingao on 24-4-12.
//

#ifndef OS_WORKBENCH_MYLIB_H
#define OS_WORKBENCH_MYLIB_H

#endif //OS_WORKBENCH_MYLIB_H
#include <am.h>
#include <stdarg.h>

void my_putchar(char c) {
    putch(c);
}

void my_puts(const char *str) {
    while (*str) {
        my_putchar(*str++);
    }
}

void my_print_integer(int num) {
    char buf[12];  // Assuming int fits into 12 characters (including sign and null terminator)
    int i = 0;
    if (num < 0) {
        my_putchar('-');
        num = -num;
    }
    do {
        buf[i++] = '0' + num % 10;
        num /= 10;
    } while (num > 0);
    for (--i; i >= 0; --i) {
        my_putchar(buf[i]);
    }
}

void my_printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    while (*fmt) {
        if (*fmt == '%') {
            fmt++;
            switch (*fmt) {
                case 'd':
                    my_print_integer(va_arg(args, int));
                    break;
                case 's':
                    my_puts(va_arg(args, const char *));
                    break;
                // You can add support for more formats here, such as %c or %x
                default:
                    my_putchar(*fmt);  // Print the '%' followed by an unrecognized format specifier
                    break;
            }
        } else {
            my_putchar(*fmt);
        }
        fmt++;
    }

    va_end(args);
}


void print(const char *s, ...) {
    va_list ap;
    va_start(ap, s);
    while (s) {
        for (; *s; s ++) putch(*s);
        s = va_arg(ap, const char *);
    }
    va_end(ap);
}