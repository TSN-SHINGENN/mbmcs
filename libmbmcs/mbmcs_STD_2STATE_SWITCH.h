#ifndef INC_MBMCS_STD_2STATE_SWITCH_H
#define INC_MBMCS_STD_2STATE_SWITCH_H

#pragma once

#include <stdint.h>

typedef struct {
    uintptr_t base_addr;
} mbmcs_std_2state_switch_t;

typedef union {
    uint8_t flags;
    struct {
	uint8_t negative_edge:1;
	uint8_t positive_edge:1;
	uint8_t current_signal:1;
    } f;
} mbmcs_std_2state_switch_status_t;

#ifdef __cplusplus
extern "C" {
#endif

int mbmcs_std_2state_switch_init(mbmcs_std_2state_switch_t * const self_p, const uintptr_t base_addr);
void mbmcs_std_2state_switch_sens_negative_enable(const mbmcs_std_2state_switch_t * const self_p, const uint8_t is_on);
void mbmcs_std_2state_switch_sens_positive_enable(const mbmcs_std_2state_switch_t * const self_p, const uint8_t is_on);
void mbmcs_std_2state_switch_irq_enable(const mbmcs_std_2state_switch_t * const self_p, const uint8_t is_on);
int mbmcs_std_2state_switch_get_flags(const mbmcs_std_2state_switch_t * const self_p, mbmcs_std_2state_switch_status_t * const stat_p);
void mbmcs_std_2state_switch_clear_detective_egde_flags(const mbmcs_std_2state_switch_t * const self_p);
uint32_t _mbmcs_std_2state_switch_get_registers(const mbmcs_std_2state_switch_t * const self_p);

void mbmcs_std_2state_switch_edge_counter_enable(const mbmcs_std_2state_switch_t * const self_p, const uint8_t is_true);
void mbmcs_std_2state_switch_edge_counter_clear(const mbmcs_std_2state_switch_t * const self_p);
uint16_t mbmcs_std_2state_switch_get_edge_counter_value(const mbmcs_std_2state_switch_t * const self_p);
uint64_t mbmcs_std_2state_switch_get_monotonic_timer(const mbmcs_std_2state_switch_t * const self_p);

#ifdef __cplusplus
}
#endif

#endif /* end of INC_MBMCS_STD_2STATE_SWITCH_H */
