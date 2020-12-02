/**
 *      Copyright 2019 TSN.SHINGENN
 *
 *	Basic Author: Seiichi Takeda  '2019-October-04 Active
 *		Last Alteration $Author: takeda $
 */

/**
 * @file mbmcs_pargen7x4.c
 * @brief 7-bit x 4(data) のデータをパリティ付加して32-bitで処理
 **/

/* POSIX or CRL */ 
#include <stdint.h>
#include <stdio.h>
#include <errno.h>

/* this */
#include "mbmcs_def.h"
#include "mbmcs_stdio.h"
#include "mbmcs_pargen7x4.h"

/* dbms */
#ifdef __GNUC__
__attribute__ ((unused))
#endif
#ifdef DEBUG
static int debuglevel = 4;
#else
static const int debuglevel = 0;
#endif
#include "dbms.h"

#if defined(_MULTIOS_DMSG_IS_UART)
#define EOL_CRLF "\n\r"
#else
#define EOL_CRLF "\n"
#endif


#ifndef MBMCS_EXTTEND_MODULE_MEMMAP_ADDR
#define MBMCS_EXTTEND_MODULE_MEMMAP_ADDR 0xC0000000
#endif
#ifndef MBMCS_MEMALIGN
#define MBMCS_MEMALIGN 4
#endif

extern int mbmcs_pargen7x4_init(mbmcs_pargen7x4_t* const self_p, const uintptr_t base_addr)
	__attribute__ ((optimize("Os")));
extern uint32_t mbmcs_pargen7x4_set_data(mbmcs_pargen7x4_t* const self_p, const uint32_t d32_7x4)
	__attribute__ ((optimize("Os")));
extern uint32_t mbmcs_pargen7x4_get_data(mbmcs_pargen7x4_t* const self_p)
	__attribute__ ((optimize("Os")));
extern uint32_t mbmcs_pargen7x4_get_merged(mbmcs_pargen7x4_t* __restrict const self_p)
	__attribute__ ((optimize("Os")));
extern uint32_t mbmcs_pargen7x4_get_masked(mbmcs_pargen7x4_t* __restrict const self_p)
	__attribute__ ((optimize("Os")));
extern int mbmcs_pargen7x4_checked_is_failure(mbmcs_pargen7x4_t* __restrict const self_p)
	__attribute__ ((optimize("Os")));

/**
 * @fn int mbmcs_pargen7x4_init(mbmcs_pargen7x4_t* const self_p, const uintptr_t base_addr)
 * @brief mbmcs_pargen7x4モジュールのメモリマップドアドレスを登録します
 * @param self_p mbmcs_pargen7x4_tインスタンスポインタ
 * @param base_addr mbmcs_stdintrモジュールのベースアドレス
 * @retval 0 成功
 * @retval EINVAL 引数が不正
 **/
int mbmcs_pargen7x4_init(mbmcs_pargen7x4_t* const self_p, const uintptr_t base_addr)
{
    if((base_addr < MBMCS_EXTTEND_MODULE_MEMMAP_ADDR) || ( base_addr & (MBMCS_MEMALIGN-1))) {
	return EINVAL;
    }

    self_p->base_addr = base_addr;

    return 0;
}

/**
 * @fn int mbmcs_pargen7x4_set_data(mbmcs_pargen7x4_t* const self_p, const uint32_t d32_7x4)
 * @brief mbmcs_pargen7x4モジュールでパリティ処理をするデータを設定
 *	パリティの付加は mbmcs_pargen7x4_get_merged()で取得します。
 *	パリティ削除値はmbmcs_pargen7x4_get_masked()で取得します。
 *	パリティの整合チェックはmbmcs_pargen7x4_checked_is_succeed()で結果を取得します。
 * @param self_p mbmcs_pargen7x4_tインスタンスポインタ
 * @param d32_7x4 7bitx4の32-bitデータ
 * @retval レジスタ再読込32-bitデータ値(主にコンペア用)
 **/
uint32_t mbmcs_pargen7x4_set_data(mbmcs_pargen7x4_t* const self_p, const uint32_t d32_7x4)
{
    volatile uint32_t *const  reg32_p = (uint32_t*)self_p->base_addr;
    // volatile uint32_t d32;

    *reg32_p = d32_7x4;
    // DMSG( stderr, "base=%08x *reg32_p=%08x d32_7x4=%08x" EOL_CRLF, (int)reg32_p, *reg32_p, d32_7x4);
    // d32 = *(reg32_p+1);
    // DMSG( stderr, "*reg32_p+1=%08x" EOL_CRLF, d32);
    // d32 = *(reg32_p+2);
    // DMSG( stderr, "*reg32_p+2=%08x" EOL_CRLF, d32);

    return *reg32_p;
}

/**
 * @fn uint32_t mbmcs_pargen7x4_get_data(mbmcs_pargen7x4_t* const self_p)
 * @brief mbmcs_pargen7x4_set_data()で設定したデータを取得
 * @param self_p mbmcs_pargen7x4_tインスタンスポインタ
 * @return mbmcs_pargen7x4_set_data()で設定した32-bitデータ値
 **/
uint32_t mbmcs_pargen7x4_get_data(mbmcs_pargen7x4_t* const self_p)
{
    const volatile uint32_t *const reg32_p = (uint32_t*)self_p->base_addr;

    return *reg32_p;
}


/**
 * @fn uint32_t mbmcs_pargen7x4_get_merged(mbmcs_pargen7x4_t* __restrict const self_p)
 * @brief mbmcs_pargen7x4_set_data()で設定したデータを取得
 * @param self_p mbmcs_pargen7x4_tインスタンスポインタ
 * @return mbmcs_pargen7x4_set_data()で設定した32-bitデータ値
 **/
uint32_t mbmcs_pargen7x4_get_merged(mbmcs_pargen7x4_t* __restrict const self_p)
{
    const volatile uint32_t *const reg32_p = (uint32_t*)self_p->base_addr;

    return *(reg32_p + 1);
}


/**
 * @fn uint32_t mbmcs_pargen7x4_get_masked(mbmcs_pargen7x4_t* __restrict const self_p)
 * @brief mbmcs_pargen7x4_set_data()で設定したデータからパリティをマスクしたデータを取得
 * @param self_p mbmcs_pargen7x4_tインスタンスポインタ
 * @return mbmcs_pargen7x4_set_data()で設定した32-bitデータ値からパリティビットを削除した値
 **/
uint32_t mbmcs_pargen7x4_get_masked(mbmcs_pargen7x4_t* __restrict const self_p)
{
    const volatile uint32_t *const reg32_p = (uint32_t*)self_p->base_addr;

    return *(reg32_p + 2);
}

/**
 * @fn int mbmcs_pargen7x4_checked_is_failure(mbmcs_pargen7x4_t* __restrict const self_p)
 * @brief mbmcs_pargen7x4_set_data()で設定したデータがパリティチェックに失敗したら真値になります。
 * @param self_p mbmcs_pargen7x4_tインスタンスポインタ
 * @retval 0 成功
 * @retval ~0 失敗
 **/
int mbmcs_pargen7x4_checked_is_failure(mbmcs_pargen7x4_t* __restrict const self_p)
{
    const volatile uint32_t *const reg32_p = (uint32_t*)self_p->base_addr;

    return (*(reg32_p) != *(reg32_p+1)) ? ~0 : 0;
}
