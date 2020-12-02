#ifndef INC_LIBMBMCS_LIBMULTIOS_BRIDGE_H
#define INC_LIBMBMCS_LIBMULTIOS_BRIDGE_H

#pragma once

#include <stdint.h>

extern volatile uint64_t const * const mbmcs_jiffies_p;


#ifdef __cplusplus
extern "C" {
#endif

void mbmcs_msleep( const unsigned int milisecs );
unsigned int mbmcs_sleep( const unsigned int seconds);

int64_t mbmcs_sysconf_pagesize(void);
int64_t mbmcs_sysconf_nprocessor_conf(void);
int64_t mbmcs_sysconf_nprocessor_onln(void);
int64_t mbmcs_sysconf_phys_pages(void);
int64_t mbmcs_sysconf_avphys_pages(void);

#ifdef __cplusplus
}
#endif

#ifndef _MBMCS_TIMESPEC_T
#define _MBMCS_TIMESPEC_T
typedef struct {
    time_t tv_sec;	/*!< 秒数 */
    uint64_t tv_nsec;	/*!< ナノ秒数 */
} mbmcs_timespec_t;
#endif /* end of _MBMCS_TIMESPEC_T */

#ifdef __cplusplus
extern "C" {
#endif

int mbmcs_clock_gettime_clock_monotonic(mbmcs_timespec_t * const mts_p);

#ifdef __cplusplus
}
#endif

#endif /* end of INC_LIBMBMCS_LIBMULTIOS_BRIDGE_H */
