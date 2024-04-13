#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#define MAX_INT_DIGITS 11
#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    while (*fmt) {
        if (*fmt == '%') {
            ++fmt;  // Skip '%'

            // Parse flags, width, precision, etc. (omitted for simplicity)

            char conv_spec = *fmt++;
            switch (conv_spec) {
                case 'd':
                case 'i': {
                    int val = va_arg(args, int);
                    // Implement integer formatting and output via putch()
                    char buffer[MAX_INT_DIGITS];
                    int index = 0;
                    while (val>=10){
                        buffer[index] = '0' + val%10;
                        val = val/10;
                        index++;
                    }
                    buffer[index] = '0' + val;
                    while (index>=0){
                        putch(buffer[index]);
                        index--;
                    }
                    break;
                }
                // case 'u': {
                //     unsigned int val = va_arg(args, unsigned int);
                //     // Implement unsigned integer formatting and output via putch()

                //     break;
                // }
                case 'x':
                case 'X': {
                    unsigned int val = va_arg(args, unsigned int);
                    // Convert the unsigned integer to its uppercase hexadecimal representation
                    char hex_buffer[11]; // Assuming 32-bit unsigned int requires at most 8 hex digits + null terminator
                    int len = snprintf(hex_buffer, sizeof(hex_buffer), "%X", val);

                    // Output the hexadecimal string via putch()
                    for (int i = 0; i < len; ++i)
                    {
                        putch(hex_buffer[i]);
                    }
                    break;
                }
                // case 'f': {
                //     double val = va_arg(args, double);
                //     // Implement floating-point formatting and output via putch()
                //     break;
                // }
                // case 'c': {
                //     int ch = va_arg(args, int);
                //     putch(ch);
                //     break;
                // }
                case 's': {
                    const char *str = va_arg(args, const char *);
                    while (*str) {
                        putch(*str++);
                    }
                    break;
                }
                case '%': {
                    putch('%');
                    break;
                }
                default: {
                    // Unknown conversion specifier, handle it as needed
                    break;
                }
            }
        } else {
            putch(*fmt++);
        }
    }

    va_end(args);
    return 0;
}


int vsprintf(char *out, const char *fmt, va_list ap) {
  panic("Not implemented");
}

int sprintf(char *out, const char *fmt, ...) {
  panic("Not implemented");
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
