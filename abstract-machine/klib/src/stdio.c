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

int vsprintf(char *out, const char *fmt, va_list ap)
{
    panic("Not implemented");
}

int sprintf(char *out, const char *fmt, ...) {
  panic("Not implemented");
}

static int _vsnprintf_helper(char *out, size_t n, const char *fmt, va_list args);

int snprintf(char *out, size_t n, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int result = _vsnprintf_helper(out, n, fmt, args);
    va_end(args);
    return result;
}

static int _vsnprintf_helper(char *out, size_t n, const char *fmt, va_list args)
{
    int written = 0;

    while (*fmt && n > 1)
    {
        if (*fmt == '%')
        {
            fmt++; // Skip '%'

            if (*fmt == '%')
            { // Handle escaped '%'
                *out++ = '%';
                n--;
                written++;
                fmt++;
                continue;
            }

            // char *endptr;
            long int num = 0;
            int width = 0, pad = 0;

            if (*fmt == '-')
            { // Handle left padding
                fmt++;
                pad = 1;
            }

            while (*fmt >= '0' && *fmt <= '9')
            { // Parse width
                width = width * 10 + (*fmt - '0');
                fmt++;
            }

            if (*fmt == 'l')
            { // Handle 'l' prefix for long integers
                fmt++;
                num = va_arg(args, long int);
            }
            else
            {
                num = va_arg(args, int);
            }

            // Convert number to string
            char num_buf[32]; // Assuming 32-bit integers need at most 11 characters (including negative sign and null terminator)
            int num_len = snprintf(num_buf, sizeof(num_buf), "%ld", num);

            // Perform padding and copy number string to output
            if (pad)
            {
                int pad_len = width - num_len;
                for (int i = 0; i < pad_len; ++i)
                {
                    *out++ = ' ';
                    n--;
                }
            }

            memcpy(out, num_buf, num_len);
            out += num_len;
            n -= num_len;
            written += num_len;

            fmt++; // Move to next character after number format
        }
        else
        {
            *out++ = *fmt++;
            n--;
            written++;
        }
    }

    if (n > 0)
    {
        *out = '\0'; // Add null terminator if there is space
    }
    else if (n == 0)
    {
        out[-1] = '\0'; // Truncate and add null terminator if no space left
    }

    return written;
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
