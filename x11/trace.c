#include "compiler.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>

void trace_fmt(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    printf(fmt, ap);
    va_end(ap);
    printf("\n");
}

void hexdump(const uint8_t *buffer, size_t sz) {
    for (size_t i = 0; i < sz; i += 16) {
        printf("%08zX   ", i);

        for (size_t j = 0; j < 16; j++) {
            if (i + j < sz) {
                printf("%02hhx ", buffer[i + j]);
            } else {
                printf("   ");
            }
        }

        printf("  ");

        for (size_t j = 0; j < 16; j++) {
            if (i + j < sz) {
                if (isprint(buffer[i + j])) {
                    printf("%c", (char)buffer[i + j]);
                } else {
                    printf(".");
                }
            } else {
                printf(" ");
            }
        }

        printf("\n");
    }
}
