#ifndef COMMON_INCLUDE_GUARD
#define COMMON_INCLUDE_GUARD

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#define ARRAY_SIZE(X) ((sizeof(X)) / (sizeof((X)[0])))

#ifndef nullptr
#define nullptr (void *)0
#endif

static inline void fatalf(const char *fmt, ...) {
    va_list args;

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    exit(EXIT_FAILURE);
}

#endif