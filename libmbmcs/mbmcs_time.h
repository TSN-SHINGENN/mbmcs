#ifndef INC_MBMCS_TIME_H
#define INC_MBMCS_TIME_H

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

void mbmcs_msleep( const unsigned int msec );
unsigned int mbmcs_sleep( const unsigned int seconds);

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

int mbmcs_clock_gettime_clock_monotonic(mbmcs_timespec_t *const mts_p);

#ifdef __cplusplus
}
#endif

#endif /* end of INC_MBMCS_TIME_H */
