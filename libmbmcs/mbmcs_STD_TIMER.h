#ifndef INC_MBMCS_STD_TIMER_H
#define INC_MBMCS_STD_TIMER_H

#pragma once

#include <stdint.h>

typedef struct {
    uintptr_t base_addr;
} mbmcs_std_timer_t;

#if defined (__cplusplus)
extern "C" {
#endif

void mbmcs_std_timer_init(mbmcs_std_timer_t *const self_p, const uintptr_t base_addr);
uint64_t mbmcs_std_timer_get_timer_counter( mbmcs_std_timer_t *const self_p);
void mbmcs_std_timer_force_reset(mbmcs_std_timer_t *const self_p);
void mbmcs_std_timer_pause(mbmcs_std_timer_t *const self_p);
void mbmcs_std_timer_unpause(mbmcs_std_timer_t *const self_p);
int mbmcs_std_timer_set_timer_counter( mbmcs_std_timer_t *const self_p, const uint64_t msec64);
int mbmcs_std_timer_set_timer_counter_and_unpause( mbmcs_std_timer_t *const self_p, const uint64_t msec64);

#if defined (__cplusplus)
}
#endif

#endif /* end of INC_MBMCS_STD_TIMER_H */


