/**
 *      Copyright 2015 TSN.SHINGENN
 *
 *      Basic Author: Seiichi Takeda  '2015-August-01 Active
 *              Last Alteration $Author: seiichi $
 */

#include <sys/types.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/* MB MCSランタイム */
#include <xparameters.h>

/* multios */
#include "multios_lite_mallocater.h"

/* this */
#include "libmbmcs_libmultios_bridge.h"
#include "mbmcs_stdio.h"
#include "mbmcs_platform.h"
#include "mbmcs_unistd.h"

/* Debug */
#ifdef __GNUC__
__attribute__ ((unused))
#endif
#ifdef DEBUG
static int debuglevel = 4;
#else
static int debuglevel = 0;
#endif
#include "dbms.h"

#if defined(_MULTIOS_DMSG_IS_UART)
#define EOL_CRLF "\n\r"
#else
#define EOL_CRLF "\n"
#endif

/**
 * @fn int64_t mbmcs_sysconf_pagesize(void)
 * @brief multios_sysconfが返す pagesizeを返します。
 *	BlockRAMを使っている際にpagesizeという概念が無いが。念のため
 * @return ページサイズと同等と思われるブロックメモリの最小単位
 **/
int64_t mbmcs_sysconf_pagesize(void)
{
    return XPAR_CPU_MICROBLAZE_I_DATA_SIZE;
}

/**
 * @fn int64_t mbmcs_sysconf_nprocessor_conf(void)
 * @brief multios_sysconfが返すカーネルが認識している CPUの数を返します。
 * @return 1（固定）
 **/
int64_t mbmcs_sysconf_nprocessor_conf(void)
{
    return 1; /* FPGA内でデュアルプロセッサにしても個別に動作するため */
}

/**
 * @fn int64_t mbmcs_sysconf_nprocessor_onln(void)
 * @brief multios_sysconfが返すカーネルが動作している CPUの数を返します。
 * @return 1（固定）
 **/
int64_t mbmcs_sysconf_nprocessor_onln(void)
{
    return 1; /* FPGA内でデュアルプロセッサにしても個別に動作するため */
}

/**
 * @fn int64_t mbmcs_sysconf_phys_pages(void)
 * @brief メモリの物理ページ数を返します。最大64KByte/8Byte
 * @return ページ数
 **/
int64_t mbmcs_sysconf_phys_pages(void)
{
    return 2 * 0xffff / XPAR_CPU_MICROBLAZE_I_DATA_SIZE;
}

/**
 * @fn int64_t mbmcs_sysconf_avphys_pages(void)
 * @brief 仮想ヒープライブラリが管理しているFREE領域の総サイズを返します。
 *	フラグメントが発生している場合は、返された最大領域を取得できません。
 **/
int64_t mbmcs_sysconf_avphys_pages(void)
{
    return multios_lite_mallocater_avphys_with_obj(&_multios_lite_mallocater_heap_emulate_obj) / XPAR_CPU_MICROBLAZE_I_DATA_SIZE;

}
