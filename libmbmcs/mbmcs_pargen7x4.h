#ifndef INC_MBMCS_PARGEN7X4_H
#define INC_MBMCS_PARGEN7X4_H

#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uintptr_t base_addr;
} mbmcs_pargen7x4_t;

#if defined(__cplusplus)
extern "C" {
#endif

int mbmcs_pargen7x4_init(mbmcs_pargen7x4_t* const self_p, const uintptr_t base_addr);
uint32_t mbmcs_pargen7x4_set_data(mbmcs_pargen7x4_t* const self_p, const uint32_t d32_7x4);
uint32_t mbmcs_pargen7x4_get_data(mbmcs_pargen7x4_t* const self_p);
uint32_t mbmcs_pargen7x4_get_merged(mbmcs_pargen7x4_t* __restrict const self_p);
uint32_t mbmcs_pargen7x4_get_masked(mbmcs_pargen7x4_t* __restrict const self_p);
int mbmcs_pargen7x4_checked_is_failure(mbmcs_pargen7x4_t* __restrict const self_p);

#if defined(__cplusplus)
}
#endif

#endif /* end of INC_MBMCS_PARGEN7X4_H */
