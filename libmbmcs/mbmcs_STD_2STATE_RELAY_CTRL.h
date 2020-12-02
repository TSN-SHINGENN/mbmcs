#ifndef INC_MBMCS_STD_2STATE_RELAY_CTRL_H
#define INC_MBMCS_STD_2STATE_RELAY_CTRL_H

#pragma once

#include <stdint.h>

typedef struct {
    uintptr_t base_addr;
} mbmcs_std_2state_relay_ctrl_t;

typedef volatile union {
    uint8_t flags;
    struct {
        uint8_t is_delay_threshed:1;
        uint8_t is_active_threshed:1;
    } f;
} mbmcs_std_2state_relay_ctrl_interrupt_status_t;

#ifdef __cplusplus
extern "C" {
#endif

int mbmcs_std_2state_relay_ctrl_init(mbmcs_std_2state_relay_ctrl_t *const self_p, const uintptr_t base_addr);
void mbmcs_std_2state_relay_ctrl_irq_enable(const mbmcs_std_2state_relay_ctrl_t *const self_p, const uint8_t is_on);

int mbmcs_std_2state_relay_ctrl_delay_timer_is_active(const mbmcs_std_2state_relay_ctrl_t *const self_p);

void mbmcs_std_2state_relay_ctrl_exec_pause(const mbmcs_std_2state_relay_ctrl_t *const self_p);
void mbmcs_std_2state_relay_ctrl_exec_clear(const mbmcs_std_2state_relay_ctrl_t *const self_p);
void mbmcs_std_2state_relay_ctrl_exec_continue(const mbmcs_std_2state_relay_ctrl_t *const self_p);
void mbmcs_std_2state_relay_ctrl_exec_start(const mbmcs_std_2state_relay_ctrl_t *const self_p);

int mbmcs_std_2state_relay_ctrl_is_busy(const mbmcs_std_2state_relay_ctrl_t *const self_p);

int mbmcs_std_2state_relay_ctrl_ative_timer_is_active(const mbmcs_std_2state_relay_ctrl_t *const self_p);
int mbmcs_std_2state_relay_ctrl_delay_timer_is_active(const mbmcs_std_2state_relay_ctrl_t *const self_p);

void mbmcs_std_2state_relay_ctrl_set_pause_at_active_thresh(const mbmcs_std_2state_relay_ctrl_t *const self_p, const uint8_t is_on);
int mbmcs_std_2state_relay_ctrl_get_flag_pause_at_active_thresh(const mbmcs_std_2state_relay_ctrl_t *const self_p);

uint32_t mbmcs_std_2state_relay_ctrl_get_delay_timer_counter(const mbmcs_std_2state_relay_ctrl_t *const self_p);
uint32_t mbmcs_std_2state_relay_ctrl_get_active_timer_counter(const mbmcs_std_2state_relay_ctrl_t *const self_p);

int mbmcs_std_2state_relay_ctrl_set_delay_timer_thresh_msec(const mbmcs_std_2state_relay_ctrl_t *const self_p,const uint32_t msec);

int mbmcs_std_2state_relay_ctrl_set_active_timer_thresh_msec(const mbmcs_std_2state_relay_ctrl_t *const self_p, const uint32_t msec);

uint32_t mbmcs_std_2state_get_delay_timer_count_msec(const mbmcs_std_2state_relay_ctrl_t *const self_p);
uint32_t mbmcs_std_2state_relay_ctrl_relay_ctrl_get_active_timer_count_msec(const mbmcs_std_2state_relay_ctrl_t *const self_p);

void mbmcs_std_2state_relay_get_interrupt_flags(const mbmcs_std_2state_relay_ctrl_t *const self_p, mbmcs_std_2state_relay_ctrl_interrupt_status_t * const intr_stat_p);

#ifdef __cplusplus
}
#endif

#endif /* end of INC_MBMCS_STD_2STATE_RELAY_CTRL_H */
