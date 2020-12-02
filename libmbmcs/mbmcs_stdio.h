#ifndef INC_MBMCS_STDIO_H
#define INC_MBMCS_STDIO_H

#pragma once

#include <stddef.h>


#if defined(__cplusplus)
extern "C" {
#endif

void mbmcs_printf_stdout_initializer(void (*putchar_cb_func)(const int));

int mbmcs_putchar(const int code);
int mbmcs_printf( const char *const fmt, ...);
int mbmcs_snprintf( char *const str, const size_t size, const char *const fmt, ...);

#if defined(__cplusplus)
}
#endif

#endif /* end of INC_MBMCS_STDIO_H */
