#ifndef INC_MBMCS_MONOTONIC_TIMER_H
#define INC_MBMCS_MONOTONIC_TIMER_H

#pragma once

#include <stdint.h>

typedef struct {
    uintptr_t base_addr;
} mbmcs_monotonic_timer_t;

#ifdef __cplusplus
extern "C" {
#endif

void mbmcs_monotonic_timer_init(mbmcs_monotonic_timer_t *const self_p, const uintptr_t base_addr);
uint64_t mbmcs_monotonic_timer_get_timer_counter( const mbmcs_monotonic_timer_t *const self_p);

#ifdef __cplusplus
}
#endif

#endif /* end of _INC_MBMCS_MONOTONIC_TIMER_H */


