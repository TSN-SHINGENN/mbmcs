#ifndef INC_MBMCS_MEMTEST_H
#define INC_MBMCS_MEMTEST_H

#include <stdint.h>

typedef union _mbmcs_memory_test_attr {
    unsigned int flags;
    struct {
	unsigned int fix8:1;
	unsigned int fix16:1;
	unsigned int fix32:1;
	unsigned int fix64:1;
	unsigned int rnd32:1;
    } f;
} mbmcs_memory_test_attr_t;

#ifdef __cplusplus
extern "C" {
#endif

uint32_t mbmcs_memory_test( void * const buf, const size_t bufsz, const mbmcs_memory_test_attr_t *const attr_p );

#ifdef __cplusplus
}
#endif

#endif /* end of INC_MBMCS_MEMTEST_H */
