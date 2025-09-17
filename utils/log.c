#include "log.h"
#include <stdio.h>
#include <stdarg.h>
#include "utils.h"


void log_if_level(int level, const char *format, ...) {
    if (log_level > level) {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
    }
}   