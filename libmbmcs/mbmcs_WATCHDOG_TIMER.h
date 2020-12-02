#ifndef INC_MBMCS_WATCHDOG_TIMER_H
#define INC_MBMCS_WATCHDOG_TIMER_H

#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uintptr_t base_addr;
} mbmcs_watchdog_timer_t;

#ifdef __cplusplus
extern "C" {
#endif

int mbmcs_watchdog_timer_init(mbmcs_watchdog_timer_t *const self_p, uintptr_t base_addr);
void mbmcs_watchdog_timer_set_thresh(mbmcs_watchdog_timer_t const *const self_p, const uint16_t thresh);
void mbmcs_watchdog_timer_set_irq_thresh(mbmcs_watchdog_timer_t const *const self_p, uint16_t thresh);
void mbmcs_watchdog_timer_enable(mbmcs_watchdog_timer_t const *const self_p, const bool is_true);
void mbmcs_watchdog_timer_irq_enable(mbmcs_watchdog_timer_t const *const self_p, const bool is_true);

void mbmcs_watchdog_timer_interval_reset(mbmcs_watchdog_timer_t const *const self_p);

void _mbmcs_watchdog_timer_dump_ctrl_reg(mbmcs_watchdog_timer_t const *const self_p);

#ifdef __cplusplus
}
#endif

#endif /* end of INC_MBMCS_WATCHDOG_TIMER_H */
