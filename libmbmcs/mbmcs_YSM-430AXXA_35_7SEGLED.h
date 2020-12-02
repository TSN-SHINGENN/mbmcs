#ifndef INC_MBMCS_YSM_430AXXA_35_7SEGLED_H
#define INC_MBMCS_YSM_430AXXA_35_7SEGLED_H

#pragma once

#include <stdint.h>

typedef struct {
      uintptr_t base_addr;
} mbmcs_ysm_430axxa_35_7seg4led_t;

#ifdef __cplusplus
extern "C" {
#endif

int mbmcs_ysm_430axxa_35_7seg4led_init(mbmcs_ysm_430axxa_35_7seg4led_t *const self_p, const uintptr_t base_addr);
void mbmcs_ysm_430axxa_35_7seg4led_write_hex_digit(const mbmcs_ysm_430axxa_35_7seg4led_t *const self_p, const uint16_t data16);
void mbmcs_ysm_430axxa_35_7seg4led_write_dot(const mbmcs_ysm_430axxa_35_7seg4led_t *const self_p, const uint8_t dots_bit);
void mbmcs_ysm_430axxa_35_7segled_write_colon(const mbmcs_ysm_430axxa_35_7seg4led_t *const self_p, const uint8_t is_on);
void mbmcs_ysm_430axxa_35_7seg4led_write_apos(const mbmcs_ysm_430axxa_35_7seg4led_t *const self_p, const uint8_t is_on);
int mbmcs_ysm_430axxa_35_7seg4led_test(const mbmcs_ysm_430axxa_35_7seg4led_t *const self_p, const uint8_t is_on);
int mbmcs_ysm_430axxa_35_7seg4led_on(const mbmcs_ysm_430axxa_35_7seg4led_t *const self_p, const uint8_t is_on);
void mbmcs_ysm_430axxa_35_7seg4led_ascii_mode(const mbmcs_ysm_430axxa_35_7seg4led_t *const self_p, const uint8_t is_on);
int mbmcs_ysm_430axxa_35_7seg4led_write_ascii_digit(const mbmcs_ysm_430axxa_35_7seg4led_t *const self_p, const char * const str);
int mbmcs_ysm_430axxa_35_7seg4led_destroy(mbmcs_ysm_430axxa_35_7seg4led_t *const self_p);

#ifdef __cplusplus
}
#endif

#endif /* end of INC_MBMCS_YSM_430AXXA_35_7SEGLED_H */
