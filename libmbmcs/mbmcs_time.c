/**
 *      Copyright 2015 TSN.SHINGENN
 *
 *      Basic Author: Seiichi Takeda  '2015-August-01 Active
 *              Last Alteration $Author: seiichi $
 */

/**
 * @file multios_unistd.c
 * @brief POSIX unistdライブラリのラッパーライブラリコード
 *     libmultiosでの不足分を補います
 **/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* libmultios */
#include "libmbmcs_libmultios_bridge.h"

/* libmbmcs */
#include "mbmcs_platform.h"
#include "mbmcs_unistd.h"
#include "mbmcs_time.h"

/* Debug */
#ifdef __GNUC__
__attribute__ ((unused))
#endif
#ifdef DEBUG
static int debuglevel = 1;
#else
static const int debuglevel = 0;
#endif
#include "dbms.h"

#if defined(_MULTIOS_DMSG_IS_UART)
#define EOL_CRLF "\n\r"
#else
#define EOL_CRLF "\n"
#endif

/* GCC最適化オプション指定のためのプロトタイプ宣言 */
#if defined(__GNUC__)

extern void mbmcs_msleep( const unsigned int)
	__attribute__ ((optimize("Os")));
extern unsigned int mbmcs_sleep( const unsigned int)
	__attribute__ ((optimize("Os")));
extern int mbmcs_clock_gettime_clock_monotonic(mbmcs_timespec_t *const)
	__attribute__ ((optimize("Os")));

#endif /* end of optimize option for GCC */

/**
 * @fn void mbmcs_msleep( unsigned int milisec )
 * @brief 指定ミリ秒間休止する。
 * @param milisec ミリ秒
 */
void mbmcs_msleep( const unsigned int milisec )
{
#ifndef MBMCS_ENABLE_FIT1_JIFFIES_COUNTER
    DMSG(stderr,"mbmcs_msleep : FIT1_JIFFIES_COUNTER function is not enabled" EOL_CRLF);
    abort();
#else
    const uint64_t mbmcs_jiffies = *mbmcs_jiffies_p;
    uint64_t endtime = mbmcs_jiffies + milisec;

    while ( endtime > *mbmcs_jiffies_p );

    return;
#endif
}

/**
 * @fn unsigned int mbmcs_sleep( const unsigned int seconds)
 * @brief 指定秒間休止する。
 *	シグナルによって割り込まれることがあります。
 * @param seconds 秒
 * @return とりあえず0固定
 */
unsigned int mbmcs_sleep( const unsigned int seconds)
{
#ifndef MBMCS_ENABLE_FIT1_JIFFIES_COUNTER
    DMSG(stderr,"mbmcs_sleep : FIT1_JIFFIES_COUNTER function is not enabled" EOL_CRLF);
    abort();
#else

    const uint64_t mbmcs_jiffies = *mbmcs_jiffies_p;
    uint64_t endtime = mbmcs_jiffies + ((uint64_t)seconds * 1000);

    while ( endtime > *mbmcs_jiffies_p );

    return 0;
#endif
}

/**
 * @fn int mbmcs_clock_gettime_clock_monotonic(mbmcs_timespec_t * const mts_p)
 * @brief POSIX clock_gettime() の CLOCK_MONOTONICパラメータのラッパー
 * @param mts_p mbmcs_timespec_t構造体ポインタ
 * @retval 0 成功
 **/
int mbmcs_clock_gettime_clock_monotonic(mbmcs_timespec_t * const mts_p)
{
#ifndef MBMCS_ENABLE_FIT1_JIFFIES_COUNTER
    DBMS1(stderr,"mbmcs_clock_gettime_clock_monotonic : FIT1_JIFFIES_COUNTER function is not enabled" EOL_CRLF);
    return EINVAL;
#else
    const uint64_t mbmcs_jiffies = *mbmcs_jiffies_p;
    mbmcs_timespec_t ts;

    if( NULL == mts_p ) {
	return  EFAULT;
    }

    ts.tv_sec = mbmcs_jiffies / HZ;
    ts.tv_nsec = (mbmcs_jiffies % HZ) * 1000 * 1000;

    *mts_p = ts;

    return 0;
#endif
}



