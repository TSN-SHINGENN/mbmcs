#ifndef INC_MBMCS_ALPSEC12_ROTARYENCODER_H
#define INC_MBMCS_ALPSEC12_ROTARYENCODER_H

#pragma once

typedef struct {
      uintptr_t base_addr;
} mbmcs_alpsec12_rotaryencoder_t;

#ifdef __cplusplus
extern "C" {
#endif

int mbmcs_alpsec12_rotaryencoder_init(mbmcs_alpsec12_rotaryencoder_t *const self_p, const uintptr_t base_addr);
void mbmcs_alpsec12_rotaryencoder_get_count(const mbmcs_alpsec12_rotaryencoder_t *const self_p, uint8_t *const left_p, uint8_t *const right_p);
void mbmcs_alpsec12_rotaryencoder_set_module_enable(const mbmcs_alpsec12_rotaryencoder_t *const self_p, const uint8_t is_on);
void mbmcs_alpsec12_rotaryencoder_set_module_irq_enable(const mbmcs_alpsec12_rotaryencoder_t *const self_p, const uint8_t is_on);
void mbmcs_alpsec12_rotaryencoder_clear_counter(const mbmcs_alpsec12_rotaryencoder_t *const self_p);

#ifdef __cplusplus
}
#endif

#endif /* end of INC_MBMCS_ALPSEC12_ROTARYENCODER_H */
