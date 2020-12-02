#ifndef INC_MBMCS_LIMITED_MOTION_2STATE_SWITCH_H
#define INC_MBMCS_LIMITED_MOTION_2STATE_SWITCH_H

#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct _mbmcs_limited_motion_2state_switch {
    uintptr_t base_addr;
} mbmcs_limited_motion_2state_switch_t;

typedef union {
    uint8_t flags;
    struct {
        uint8_t timedout:1;
	uint8_t intime:1;
	uint8_t cur_level:1;
    } f;
} mbmcs_limited_motion_2state_switch_status_t;


int mbmcs_limited_motion_2state_switch_init( mbmcs_limited_motion_2state_switch_t * const self_p, const uintptr_t base_addr);

void mbmcs_limited_motion_2state_switch_irq_enable(const mbmcs_limited_motion_2state_switch_t *self_p, bool is_on);

void mbmcs_limited_motion_2state_switch_get_interrupt_status(const mbmcs_limited_motion_2state_switch_t *const self_p, mbmcs_limited_motion_2state_switch_status_t *const stat_p);

void mbmcs_limited_motion_2state_switch_clear_interrupt_status(const mbmcs_limited_motion_2state_switch_t *const self_p);

void mbmcs_limited_motion_2state_switch_set_limited_timer(const mbmcs_limited_motion_2state_switch_t *const self_p, const uint16_t milisec);

#endif /* end of INC_MBMCS_LIMITED_MOTION_2STATE_SWITCH_H */
