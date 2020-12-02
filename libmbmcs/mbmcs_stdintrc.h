#ifndef INC_MBMCS_STDINTRC_H
#define INC_MBMCS_STDINTRC_H

#pragma once

typedef struct {
      uintptr_t base_addr;
} mbmcs_stdintrc_t;

#ifdef __cplusplus
extern "C" {
#endif

int mbmcs_stdintrc_init(mbmcs_stdintrc_t * const self_p, const uintptr_t base_addr);
int mbmcs_stdintrc_enable_output( const mbmcs_stdintrc_t *const self_p, const uintptr_t addr_offs);
int mbmcs_stdintrc_disable_output( const mbmcs_stdintrc_t *const self_p, const uintptr_t addr_offs);
int mbmcs_stdintrc_get_irq_flag( const mbmcs_stdintrc_t * const self_p, const uintptr_t addr_offs, uint32_t *const flags_p);
int mbmcs_stdintrc_clear_irq_flag( const mbmcs_stdintrc_t * const self_p, const uintptr_t addr_offs, const uint32_t flags);

#ifdef __cplusplus
}
#endif

#endif /* end of INC_MBMCS_STDINTRC_H */
