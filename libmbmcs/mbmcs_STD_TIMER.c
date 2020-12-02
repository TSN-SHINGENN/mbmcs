/**
 *      Copyright 2019 TSN・SHINGENN
 *
 *	Basic Author: Seiichi Takeda  '2019-October-20 Active
 *		Last Alteration $Author: takeda $
 */

/**
 * @file mbmcs_STD_TIMER.c
 * @brief Microblaze タイマーモジュール制御API
 **/

/* POSIX or CRL */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

/* this */
#include "mbmcs_STD_TIMER.h"
#include "mbmcs_stdio.h"

/* dbms */
#ifdef __GNUC__
__attribute__ ((unused))
#endif
#ifdef DEBUG
static int debuglevel = 4;
#else
static const int debuglevel = 0;
#endif
//#include <libmultios/dbms.h>
#include "dbms.h"

#if defined(_MULTIOS_DMSG_IS_UART)
#define EOL_CRLF "\n\r"
#else
#define EOL_CRLF "\n"
#endif

#define STD_TIMER_OP_FORCE_CLEAR_BIT ((uint32_t)0x1)
#define STD_TIMER_OP_FORCE_CLEAR_SHIFT (31)
#define STD_TIMER_OP_FORCE_CLEAR_MASK (STD_TIMER_OP_FORCE_CLEAR_BIT << STD_TIMER_OP_FORCE_CLEAR_SHIFT)

#define STD_TIMER_OP_PAUSE_BIT ((uint32_t)0x1)
#define STD_TIMER_OP_PAUSE_SHIFT (30)
#define STD_TIMER_OP_PAUSE_MASK (STD_TIMER_OP_PAUSE_BIT << STD_TIMER_OP_PAUSE_SHIFT)

#define STD_TIMER_OP_COUNTER_LATCH_MASK_BIT ((uint32_t)0x1)
#define STD_TIMER_OP_COUNTER_LATCH_MASK_SHIFT (29)
#define STD_TIMER_OP_COUNTER_LATCH_MASK_MASK (STD_TIMER_OP_COUNTER_LATCH_MASK_BIT << STD_TIMER_OP_COUNTER_LATCH_MASK_SHIFT)

#define STD_TIMER_OP(a) *((volatile unsigned int*)((intptr_t)(a) + 0x0))
#define STD_TIMER_LSB(a) *((volatile unsigned int*)((intptr_t)(a) + 0x8))
#define STD_TIMER_MSB(a) *((volatile unsigned int*)((intptr_t)(a) + 0xc))

/* GCC最適化オプション指定のためのプロトタイプ宣言 */
extern void mbmcs_std_timer_init(mbmcs_std_timer_t *const self_p, const uintptr_t base_addr)
	__attribute__ ((optimize("Os")));
extern uint64_t mbmcs_std_timer_get_timer_counter( mbmcs_std_timer_t *const self_p)
	__attribute__ ((optimize("O2")));
extern void mbmcs_std_timer_force_reset(mbmcs_std_timer_t *const self_p)
	__attribute__ ((optimize("Os")));
extern void mbmcs_std_timer_pause(mbmcs_std_timer_t *const self_p)
	__attribute__ ((optimize("Os")));
extern void mbmcs_std_timer_unpause(mbmcs_std_timer_t *const self_p)
	__attribute__ ((optimize("Os")));
extern int mbmcs_std_timer_set_timer_counter( mbmcs_std_timer_t *const self_p, const uint64_t msec64)
	__attribute__ ((optimize("O2")));
extern int mbmcs_std_timer_set_timer_counter_and_unpause( mbmcs_std_timer_t *const self_p, const uint64_t msec64)
	__attribute__ ((optimize("Os")));

/**
 * @fn void  mbmcs_std_timer_init(mbmcs_std_timer_t *const self_p, const uintptr_t base_addr)
 * @brief タイマーオブジェクトのベースアドレスの設定
 * @param self_p mbmcs_std_timer_t構造体オブジェクトポインタ
 * @param base_addr ベースアドレス
 **/
void  mbmcs_std_timer_init(mbmcs_std_timer_t *const self_p, const uintptr_t base_addr)
{
    self_p->base_addr = base_addr;

    return;
}

/**
 * @brief uint64_t mbmcs_std_timer_get_timer_counter( const mbmcs_std_timer_t *const self_p)
 * @brief タイマー値の取得
 * @param self_p mbmcs_std_timer_t構造体オブジェクトポインタ
 * @retval タイマー値
 **/
uint64_t mbmcs_std_timer_get_timer_counter( mbmcs_std_timer_t *const self_p)
{
    volatile uint64_t retval64;
    volatile uint32_t reg32;
    reg32 = STD_TIMER_OP(self_p->base_addr);
    reg32 |= STD_TIMER_OP_COUNTER_LATCH_MASK_MASK;
    STD_TIMER_OP(self_p->base_addr) = reg32;

    retval64 = STD_TIMER_MSB(self_p->base_addr);
    
    retval64 <<= 32;
    retval64 |= STD_TIMER_LSB(self_p->base_addr);

    reg32 = STD_TIMER_OP(self_p->base_addr);
    reg32 &= ~STD_TIMER_OP_COUNTER_LATCH_MASK_MASK;
    STD_TIMER_OP(self_p->base_addr) = reg32;

    return retval64;
}

/**
 * @fn void mbmcs_std_timer_force_reset(mbmcs_std_timer_t *const self_p)
 * @brief タイマー値を０へ強制リセットします。
 * @param self_p mbmcs_std_timer_t構造体オブジェクトポインタ
 **/
void mbmcs_std_timer_force_reset(mbmcs_std_timer_t *const self_p)
{
    uint32_t d32;

    d32 = STD_TIMER_OP(self_p->base_addr);
    d32 |= STD_TIMER_OP_FORCE_CLEAR_MASK;
    STD_TIMER_OP(self_p->base_addr) = d32;

    return;
}

/**
 * @fn void mbmcs_std_timer_pause(mbmcs_std_timer_t *const self_p)
 * @brief タイマーを一時停止します。タイマー値を書き換える際にも一時停止が必要です。
 * @param self_p mbmcs_std_timer_t構造体オブジェクトポインタ
 **/
void mbmcs_std_timer_pause(mbmcs_std_timer_t *const self_p)
{
    uint32_t d32;

    d32 = STD_TIMER_OP(self_p->base_addr);
    d32 |= STD_TIMER_OP_PAUSE_MASK;
    STD_TIMER_OP(self_p->base_addr) = d32;

    return;
}

/**
 * @fn void mbmcs_std_timer_unpause(mbmcs_std_timer_t *const self_p)
 * @brief タイマーを一時停止解除します。
 * @param self_p mbmcs_std_timer_t構造体オブジェクトポインタ
 **/
void mbmcs_std_timer_unpause(mbmcs_std_timer_t *const self_p)
{
    uint32_t d32;

    d32 = STD_TIMER_OP(self_p->base_addr);
    d32 &= ~STD_TIMER_OP_PAUSE_MASK;
    STD_TIMER_OP(self_p->base_addr) = d32;

    return;
}

/**
 * @fn int mbmcs_std_timer_set_timer_counter( mbmcs_std_timer_t *const self_p, const uint64_t msec64)
 * @brief タイマー値を書き換えます。一時停止していることが条件です。
 *	一時停止状態にない場合は、タイマー値は変更されません。
 * @param self_p mbmcs_std_timer_t構造体オブジェクトポインタ
 * @param msec64 タイマー設定値
 * @retval 0 成功
 * @retval EPERM タイマー値変更に失敗
 **/
int mbmcs_std_timer_set_timer_counter( mbmcs_std_timer_t *const self_p, const uint64_t msec64)
{
    uint64_t d64 = msec64;

    STD_TIMER_LSB(self_p->base_addr) = (uint32_t)(0xffffffff & d64);
    d64 >>= 32; 
    STD_TIMER_MSB(self_p->base_addr) = (uint32_t)d64;

    /* verify */
    d64 = STD_TIMER_MSB(self_p->base_addr);
    d64 <<= 32;
    d64 |= (uint64_t)STD_TIMER_LSB(self_p->base_addr);

    return (msec64 == d64) ? 0 : EPERM;

}

/**
 * @fn int mbmcs_std_timer_set_timer_counter_and_unpause( const mbmcs_std_timer_t *const self_p, const uint64_t msec64)
 * @brief タイマー設定後に一時停止解除します。エラー検出時は状態は変更しません。
 * @retval 0 成功
 * @retval EPERM タイマー値変更に失敗。一時停止されていなかった可能性。
 **/
int mbmcs_std_timer_set_timer_counter_and_unpause( mbmcs_std_timer_t *const self_p, const uint64_t msec64)
{
    int result;

    result = mbmcs_std_timer_set_timer_counter( self_p, msec64);
    if(result) {
	return result;
    }

    mbmcs_std_timer_unpause(self_p);

    return 0;
}



