#ifndef INC_MBMCS_PRINTK_H
#define INC_MBMCS_PRINTK_H

#pragma once

#include <stddef.h>

#if defined(__cplusplus)
extern "C" {
#endif

void mbmcs_printk_initializer(void (*putchar_cb_func)(const int));
int mbmcs_printk( const char *const fmt, ...);

#if defined(__cplusplus)
}
#endif

#endif /* end of INC_MBMCS_PRINTK_H */
