/**
 *      Copyright 2015 TSN・SHINGENN
 *
 *	Basic Author: Seiichi Takeda  '2015-July-23 Active
 *		Last Alteration $Author: takeda $
 */

/**
 * @file mbmcs_YSM-430AXXA_35_7SEG4LED.c
 * brief 4桁7セグ表示器の駆動ライブラリ
 **/

/* POSIX or CRL */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

/* this */
#include "mbmcs_stdio.h"
#include "mbmcs_YSM-430AXXA_35_7SEGLED.h"

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

#define MBMCS_EXTTEND_MODULE_MEMMAP_ADDR (0xC0000000)
#define MBMCS_MEMALIGN (4)


#define SEGLED_OP_AND_HEX_REG(a) *((volatile uint32_t*)((uintptr_t)(a) + 0x0))
#define SEGLED_ASCII_REG(a) *((volatile uint32_t*)((uintptr_t)(a) + 0x4))

#define SEG7X4DIGIT_MASK (0x0000ffff)
#define SEG7X4DOT_SHIFT  (16)
#define SEG7X4DOT_MASK   (((uint32_t)0xf) << SEG7X4DOT_SHIFT)
#define SEG7X4COLON_SHIFT (20)
#define SEG7X4COLON_MASK (((uint32_t)0x1) << SEG7X4COLON_SHIFT)
#define SEG7X4APOS_SHIFT (21)
#define SEG7X4APOS_MASK  (((uint32_t)0x1) << SEG7X4APOS_SHIFT)
#define SEG7X4TEST_SHIFT (30)
#define SEG7X4TEST_MASK  (((uint32_t)0x1) << SEG7X4TEST_SHIFT )
#define SEG7X4OFF_SHIFT (31)
#define SEG7X4OFF_MASK   (((uint32_t)0x1) << SEG7X4OFF_SHIFT )
#define SEG7X4SELASCII_SHIFT (29)
#define SEG7X4SELASCII_MASK (((uint32_t)0x1) << SEG7X4SELASCII_SHIFT )
#define SEG7X4ASCII_MASK (0xffffffff)

/* GCC最適化オプション指定のためのプロトタイプ宣言 */
extern int mbmcs_ysm_430axxa_35_7seg4led_init(mbmcs_ysm_430axxa_35_7seg4led_t *const self_p, const uintptr_t base_addr)
	__attribute__ ((optimize("Os")));
extern void mbmcs_ysm_430axxa_35_7seg4led_write_hex_digit(const mbmcs_ysm_430axxa_35_7seg4led_t *const self_p, const uint16_t data16)
	__attribute__ ((optimize("O2")));
extern void mbmcs_ysm_430axxa_35_7seg4led_write_dot(const mbmcs_ysm_430axxa_35_7seg4led_t *const self_p, const uint8_t dots_bit)
	__attribute__ ((optimize("O2")));
extern void mbmcs_ysm_430axxa_35_7segled_write_colon(const mbmcs_ysm_430axxa_35_7seg4led_t *const self_p, const uint8_t is_on)
	__attribute__ ((optimize("O2")));
extern void mbmcs_ysm_430axxa_35_7seg4led_write_apos(const mbmcs_ysm_430axxa_35_7seg4led_t *const self_p, const uint8_t is_on)
	__attribute__ ((optimize("O2")));
extern int mbmcs_ysm_430axxa_35_7seg4led_test(const mbmcs_ysm_430axxa_35_7seg4led_t *const self_p, const uint8_t is_on)
	__attribute__ ((optimize("O2")));
extern int mbmcs_ysm_430axxa_35_7seg4led_on(const mbmcs_ysm_430axxa_35_7seg4led_t *const self_p, const uint8_t is_on)
	__attribute__ ((optimize("O2")));
extern void mbmcs_ysm_430axxa_35_7seg4led_ascii_mode(const mbmcs_ysm_430axxa_35_7seg4led_t *const self_p, const uint8_t is_on)
	__attribute__ ((optimize("O2")));
extern int mbmcs_ysm_430axxa_35_7seg4led_write_ascii_digit(const mbmcs_ysm_430axxa_35_7seg4led_t *const self_p, const char * const str)
	__attribute__ ((optimize("O2")));
extern int mbmcs_ysm_430axxa_35_7seg4led_destroy(mbmcs_ysm_430axxa_35_7seg4led_t *const self_p)
	__attribute__ ((optimize("O2")));



/**
 * @fn int mbmcs_ysm_430axxa_35_7seg4led_init(mbmcs_ysm_430axxa_35_7seg4led_t *const self_p, const uintptr_t base_addr)
 * @brief mbmcs_ysm_430axxa_35_7seg4led_tインスタンスの初期化
 * @param self_p mbmcs_ysm_430axxa_35_7seg4led_tオブジェクトポインタ
 * @param base_addr メモリマップドベースアドレス
 * @retval 0 成功
 * @retval EINVAL 引数が不正
 **/
int mbmcs_ysm_430axxa_35_7seg4led_init(mbmcs_ysm_430axxa_35_7seg4led_t *const self_p, const uintptr_t base_addr)
{
    if((base_addr < MBMCS_EXTTEND_MODULE_MEMMAP_ADDR) ||  ( base_addr % MBMCS_MEMALIGN )) {
	return EINVAL;
    }
    self_p->base_addr = base_addr;
 
     return 0;
}

/**
 * @fn void mbmcs_ysm_430axxa_35_7seg4led_write_hex_digit(mbmcs_ysm_430axxa_35_7seg4led_t *const self_p, const uint16_t data16)
 * @brief 16bit(8bitx4)のバイナリデーターをレジスタに書きます。バイナリモードのときに表示されます。
 * @param self_p mbmcs_ysm_430axxa_35_7seg4led_tオブジェクトポインタ
 * @param data16 バイナリデータ
 */
void mbmcs_ysm_430axxa_35_7seg4led_write_hex_digit(const mbmcs_ysm_430axxa_35_7seg4led_t *const self_p, const uint16_t data16)
{
    uint32_t reg32 = SEGLED_OP_AND_HEX_REG(self_p->base_addr);
    
    reg32 &= ~SEG7X4DIGIT_MASK;
    reg32 |= SEG7X4DIGIT_MASK & data16;

    SEGLED_OP_AND_HEX_REG(self_p->base_addr) = reg32;

    return ;
}

/**
 * @fn void mbmcs_ysm_430axxa_35_7seg4led_write_dot( mbmcs_ysm_430axxa_35_7seg4led_t *const self_p, const uint8_t dots_bit)
 * @brief dotの表示フラグをレジスタに書きます。(1:ON 0:OFF)
 * @param self_p mbmcs_ysm_430axxa_35_7seg4led_tオブジェクトポインタ
 * @param dotsbit 4桁のdotフラグ
 */
void mbmcs_ysm_430axxa_35_7seg4led_write_dot(const mbmcs_ysm_430axxa_35_7seg4led_t *const self_p, const uint8_t dots_bit)
{
    uint32_t reg32 = SEGLED_OP_AND_HEX_REG(self_p->base_addr);
    
    reg32 &= ~SEG7X4DOT_MASK;
    reg32 |= (SEG7X4DOT_MASK & ((uint32_t)(dots_bit) << SEG7X4DOT_SHIFT));

    SEGLED_OP_AND_HEX_REG(self_p->base_addr) = reg32;

    return;
}

/**
 * @fn void mbmcs_ysm_430axxa_35_7segled_write_colon( mbmcs_ysm_430axxa_35_7seg4led_t *const self_p, const uint8_t is_on)
 * @brief Center colonの表示フラグをレジスタに書きます
 * @param self_p mbmcs_ysm_430axxa_35_7seg4led_tオブジェクトポインタ
 * @param is_on 0:OFF 0以外:ON
 **/
void mbmcs_ysm_430axxa_35_7segled_write_colon(const mbmcs_ysm_430axxa_35_7seg4led_t *const self_p, const uint8_t is_on)
{
    uint32_t reg32 = SEGLED_OP_AND_HEX_REG(self_p->base_addr);
    
    reg32 &= ~SEG7X4COLON_MASK;
    reg32 |= (is_on) ? SEG7X4COLON_MASK : 0x0;

    SEGLED_OP_AND_HEX_REG(self_p->base_addr) = reg32;

    return;
}

/**
 * @fn void mbmcs_ysm_430axxa_35_7seg4led_write_apos( mbmcs_ysm_430axxa_35_7seg4led_t *const self_p, const uint8_t is_on)
 * @brief APOSの表示フラグをレジスタに書きます。
 * @param self_p mbmcs_ysm_430axxa_35_7seg4led_tオブジェクトポインタ
 * @brief is_on 0:OFF 0以外:ON
 **/
void mbmcs_ysm_430axxa_35_7seg4led_write_apos(const mbmcs_ysm_430axxa_35_7seg4led_t *const self_p, const uint8_t is_on)
{
    uint32_t reg32 = SEGLED_OP_AND_HEX_REG(self_p->base_addr);
    
    reg32 &= ~SEG7X4APOS_MASK;
    reg32 |= (is_on) ? SEG7X4APOS_MASK : 0x0;

    SEGLED_OP_AND_HEX_REG(self_p->base_addr) = reg32;

    return;
}

/**
 * @fn int mbmcs_ysm_430axxa_35_7seg4led_test( mbmcs_ysm_430axxa_35_7seg4led_t *const self_p, const uint8_t is_on)
 * @brief 7セグLEDをデータに関係なく全点灯します。(LED_ONである必要あり）
 * @param self_p mbmcs_ysm_430axxa_35_7seg4led_tオブジェクトポインタ
 * @brief is_on 0:OFF 0以外:ON
 **/
int mbmcs_ysm_430axxa_35_7seg4led_test(const mbmcs_ysm_430axxa_35_7seg4led_t *const self_p, const uint8_t is_on)
{
    uint32_t reg32 = SEGLED_OP_AND_HEX_REG(self_p->base_addr);
    
    reg32 &= ~SEG7X4TEST_MASK;
    reg32 |= (is_on) ? SEG7X4TEST_MASK : 0x0;

    SEGLED_OP_AND_HEX_REG(self_p->base_addr) = reg32;

    return 0;
}

/**
 * @fn int mbmcs_ysm_430axxa_35_7seg4led_on( mbmcs_ysm_430axxa_35_7seg4led_t *const self_p, const uint8_t is_on)
 * @brief LED表示をON/OFFします。
 * @param self_p mbmcs_ysm_430axxa_35_7seg4led_tオブジェクトポインタ
 * @brief is_on 0:OFF 0以外:ON
 **/
int mbmcs_ysm_430axxa_35_7seg4led_on(const mbmcs_ysm_430axxa_35_7seg4led_t *const self_p, const uint8_t is_on)
{
    uint32_t reg32 = SEGLED_OP_AND_HEX_REG(self_p->base_addr);
    
    reg32 &= ~SEG7X4OFF_MASK;
    reg32 |= (is_on) ? 0x0 : SEG7X4OFF_MASK;

    SEGLED_OP_AND_HEX_REG(self_p->base_addr) = reg32;

    return 0;
}

/**
 * @fn void mbmcs_ysm_430axxa_35_7seg4led_ascii_mode(mbmcs_ysm_430axxa_35_7seg4led_t *const self_p, const uint8_t is_on)
 * @brief BINモードとASCIIモードを切り替えます
 * @param self_p mbmcs_ysm_430axxa_35_7seg4led_tオブジェクトポインタ
 * @brief is_on 0:BINモード 0以外:ASCIIモード
 **/
void mbmcs_ysm_430axxa_35_7seg4led_ascii_mode(const mbmcs_ysm_430axxa_35_7seg4led_t *const self_p, const uint8_t is_on)
{
    uint32_t reg32 = SEGLED_OP_AND_HEX_REG(self_p->base_addr);
    
    reg32 &= ~SEG7X4SELASCII_MASK;
    reg32 |= (is_on) ? SEG7X4SELASCII_MASK : 0x0;
    DBMS3( stderr, "ASCII_MODE=0x%08x" EOL_CRLF, reg32);

    SEGLED_OP_AND_HEX_REG(self_p->base_addr) = reg32;

    return;
}

/**
 * @fn int mbmcs_ysm_430axxa_35_7seg4led_write_ascii_digit(mbmcs_ysm_430axxa_35_7seg4led_t *const self_p, const char *str)
 * @brief ASCIIモードのデータを4文字までの文字列で与えます。
 * @param self_p mbmcs_ysm_430axxa_35_7seg4led_tオブジェクトポインタ
 * @param str 4文字までの文字列
 * @retval 0 成功
 * @retval EINVAL 4文字を超えている
 **/
int mbmcs_ysm_430axxa_35_7seg4led_write_ascii_digit(const mbmcs_ysm_430axxa_35_7seg4led_t *const self_p, const char * const str)
{
    uint32_t reg32 = 0;
    const size_t length = strlen(str);
    size_t n;

    if( !(length <= 4) ) {
	return EINVAL;
    }

    for( n=0; n<length; ++n) {
        reg32 <<= 8;
	reg32 |= 0xff & str[n];
    }

    SEGLED_ASCII_REG(self_p->base_addr) = reg32;

    return 0;
}

/**
 * @fn int mbmcs_ysm_430axxa_35_7seg4led_destroy(mbmcs_ysm_430axxa_35_7seg4led_t *const self_p)
 * @brief インスタンスを破棄します。現在することがありません。
 **/
int mbmcs_ysm_430axxa_35_7seg4led_destroy(mbmcs_ysm_430axxa_35_7seg4led_t *const self_p)
{
    (void)self_p;

    return 0;
}

