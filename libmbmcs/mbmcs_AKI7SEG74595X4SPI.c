/**
 *      Copyright 2019 TSN�ESHINGENN
 *
 *	Basic Author: Seiichi Takeda  '2019-June-06 Active
 *		Last Alteration $Author: takeda $
 */

/**
 * @file mbmcs_AKI7SEG74595X4SPI.c
 * @brief 4��7�Z�O�\����̋쓮���C�u����
 **/

/* POSIX or CRL */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

/* dbms */
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

#define MBMCS_EXTEND_MODULE_MEMMAP_ADDR (0xC0000000)
#define MBMCS_MEMALIGN (4)

/* this */
//#include "mbmcs_bit_shift.h"
#include "mbmcs_AKI7SEG74595X4SPI.h"
#define AKI7LED_DP_REG(a) *((volatile uint32_t*)((uintptr_t)(a) + 0x0))
#define AKI7LED_ASCII_REG(a) *((volatile uint32_t*)((uintptr_t)(a) + 0x4))
#define AKI7LED_HEX_REG(a) *((volatile uint32_t*)((uintptr_t)(a) + 0x8))
#define AKI7LED_OP_STAT_REG(a) *((volatile uint32_t*)((uintptr_t)(a) + 0xC))

#define AKI7X4DIGIT_MASK (0x0000ffff)
#define AKI7X4ASCII_MASK (0xffffffff)
#define AKI7X4DOT_SHIFT  (0)
#define AKI7X4DOT_MASK   (((uint32_t)0xf) << AKI7X4DOT_SHIFT)
#define AKI7X4ON_SHIFT (31)
#define AKI7X4ON_MASK   (((uint32_t)0x1) << AKI7X4ON_SHIFT )
#define AKI7X4SELASCII_SHIFT (30)
#define AKI7X4SELASCII_MASK (((uint32_t)0x1) << AKI7X4SELASCII_SHIFT )
#define AKI7X4FORCESENDSYNCTMG_SHIFT (29)
#define AKI7X4FORCESENDSYNCTMG_MASK (((uint32_t)0x1) << AKI7X4FORCESENDSYNCTMG_SHIFT )

#define AKI7X4SEND_SHIFT (0)
#define AKI7X4SEND_MASK (((uint32_t)0x1) << AKI7X4SEND_SHIFT )
#define AKI7X4BRIGHTNESS_SHIFT (16)
#define AKI7X4BRIGHTNESS_MASK (((uint32_t)0xff) << AKI7X4BRIGHTNESS_SHIFT) 

/* GCC�œK���I�v�V�����w��̂��߂̃v���g�^�C�v�錾 */
extern int  mbmcs_aki7seg74595x4spi_init(mbmcs_aki7seg74595x4spi_t *const self_p, const uintptr_t base_addr)
	__attribute__ ((optimize("Os")));
extern void mbmcs_aki7seg74595x4spi_write_hex_digit(const mbmcs_aki7seg74595x4spi_t *const self_p, const uint16_t data16)
	__attribute__ ((optimize("O2")));
extern void mbmcs_aki7seg74595x4spi_write_dot(const mbmcs_aki7seg74595x4spi_t *const self_p, const uint8_t dots_bit)
	__attribute__ ((optimize("O2")));
extern int  mbmcs_aki7seg74595x4spi_test(const mbmcs_aki7seg74595x4spi_t *const self_p, const uint8_t is_on)
	__attribute__ ((optimize("O2")));
extern int  mbmcs_aki7seg74595x4spi_on(const mbmcs_aki7seg74595x4spi_t *const self_p, const uint8_t is_on)
	__attribute__ ((optimize("O2")));
extern void mbmcs_aki7seg74595x4spi_ascii_mode(const mbmcs_aki7seg74595x4spi_t *const self_p, const uint8_t is_on)
	__attribute__ ((optimize("O2")));
extern int  mbmcs_aki7seg74595x4spi_write_ascii_digit(const mbmcs_aki7seg74595x4spi_t *const self_p, const char * const str)
	__attribute__ ((optimize("O2")));
extern void mbmcs_aki7seg74595x4spi_brightness(const mbmcs_aki7seg74595x4spi_t *const self_p, const uint8_t val)
	__attribute__ ((optimize("O2")));
extern int  mbmcs_aki7seg74595x4spi_destroy(mbmcs_aki7seg74595x4spi_t *const self_p)
	__attribute__ ((optimize("Os")));
void mbmcs_aki7seg74595x4spi_force_send_sync_timing_is_on(const mbmcs_aki7seg74595x4spi_t *const self_p, uint8_t is_on)
	__attribute__ ((optimize("O2")));

/**
 * @fn int mbmcs_aki7seg74595x4spi_init(mbmcs_aki7seg74595x4spi_t *const self_p, const uintptr_t base_addr)	
 * @brief mbmcs_ysm_430axxa_35_7seg4led_t�C���X�^���X�̏�����
 * @param self_p mbmcs_ysm_430axxa_35_7seg4led_t�I�u�W�F�N�g�|�C���^
 * @param base_addr �������}�b�v�h�x�[�X�A�h���X
 * @retval 0 ����
 * @retval EINVAL �������s��
 **/
int  mbmcs_aki7seg74595x4spi_init(mbmcs_aki7seg74595x4spi_t *const self_p, const uintptr_t base_addr)	
{
    if((base_addr < MBMCS_EXTEND_MODULE_MEMMAP_ADDR) ||  ( base_addr % MBMCS_MEMALIGN )) {
	return EINVAL;
    }
    self_p->base_addr = base_addr;
 
     return 0;
}

/**
 * @fn void mbmcs_aki7seg74595x4spi_write_hex_digit(const mbmcs_aki7seg74595x4spi_t *const self_p, const uint16_t data16)
 * @brief 16bit(8bitx4)�̃o�C�i���f�[�^�[�����W�X�^�ɏ����܂��B�o�C�i�����[�h�̂Ƃ��ɕ\������܂��B
 * @param self_p mbmcs_aki7seg74595x4spi_t�I�u�W�F�N�g�|�C���^
 * @param data16 �o�C�i���f�[�^
 */
void mbmcs_aki7seg74595x4spi_write_hex_digit(const mbmcs_aki7seg74595x4spi_t *const self_p, const uint16_t data16)
{
    uint32_t reg32 = AKI7LED_HEX_REG(self_p->base_addr);
    
    reg32 &= ~AKI7X4DIGIT_MASK;
    reg32 |= AKI7X4DIGIT_MASK & data16;

    AKI7LED_HEX_REG(self_p->base_addr) = reg32;

    return ;
}

/**
 * @fn void mbmcs_aki7seg74595x4spi_write_dot(const mbmcs_aki7seg74595x4spi_t *const self_p, const uint8_t dots_bit)
 * @brief dot�̕\���t���O�����W�X�^�ɏ����܂��B(1:ON 0:OFF)
 * @param self_p mbmcs_aki7seg74595x4spi_t�I�u�W�F�N�g�|�C���^
 * @param dotsbit 4����dot�t���O
 */
void mbmcs_aki7seg74595x4spi_write_dot(const mbmcs_aki7seg74595x4spi_t *const self_p, const uint8_t dots_bit)
{
    uint32_t reg32 = AKI7LED_DP_REG(self_p->base_addr);
    
    reg32 &= ~AKI7X4DOT_MASK;
    reg32 |= (AKI7X4DOT_MASK & ((uint32_t)(dots_bit) << AKI7X4DOT_SHIFT));
    DBMS3( stderr, "write dot 0x%x reg32:0x%08x" EOL_CRLF, dots_bit, reg32);


    AKI7LED_DP_REG(self_p->base_addr) = reg32;

    return;
}

#if 0
/**
 * @fn int mbmcs_ysm_430axxa_35_7seg4led_test( mbmcs_ysm_430axxa_35_7seg4led_t *const self_p, const uint8_t is_on)
 * @brief 7�Z�OLED���f�[�^�Ɋ֌W�Ȃ��S�_�����܂��B(LED_ON�ł���K�v����j
 * @param self_p mbmcs_ysm_430axxa_35_7seg4led_t�I�u�W�F�N�g�|�C���^
 * @brief is_on 0:OFF 0�ȊO:ON
 **/
int mbmcs_ysm_430axxa_35_7seg4led_test(const mbmcs_ysm_430axxa_35_7seg4led_t *const self_p, const uint8_t is_on)
{
    uint32_t reg32 = SEGLED_OP_AND_HEX_REG(self_p->base_addr);
    
    reg32 &= ~AKI7X4TEST_MASK;
    reg32 |= (is_on) ? AKI7X4TEST_MASK : 0x0;

    SEGLED_OP_AND_HEX_REG(self_p->base_addr) = reg32;

    return 0;
}
#endif

/**
 * @fn int mbmcs_aki7seg74595x4spi_on(const mbmcs_aki7seg74595x4spi_t *const self_p, const uint8_t is_on)
 * @brief LED�\����ON/OFF���܂��B
 * @param self_p mbmcs_ysm_430axxa_35_7seg4led_t�I�u�W�F�N�g�|�C���^
 * @brief is_on 0:OFF 0�ȊO:ON
 **/
int  mbmcs_aki7seg74595x4spi_on(const mbmcs_aki7seg74595x4spi_t *const self_p, const uint8_t is_on)
{
    uint32_t reg32 = AKI7LED_OP_STAT_REG(self_p->base_addr);
    
    reg32 &= ~AKI7X4ON_MASK;
    reg32 |= (is_on) ? 0x0 : AKI7X4ON_MASK;

    AKI7LED_OP_STAT_REG(self_p->base_addr) = reg32;

    return 0;
}

/**
 * @fn void mbmcs_aki7seg74595x4spi_ascii_mode(const mbmcs_aki7seg74595x4spi_t *const self_p, const uint8_t is_on)
 * @brief BIN���[�h��ASCII���[�h��؂�ւ��܂�
 * @param self_p mbmcs_aki7seg74595x4spi_ascii_mode�I�u�W�F�N�g�|�C���^
 * @brief is_on 0:BIN���[�h 0�ȊO:ASCII���[�h
 **/
void mbmcs_aki7seg74595x4spi_ascii_mode(const mbmcs_aki7seg74595x4spi_t *const self_p, const uint8_t is_on)
{
    uint32_t reg32 = AKI7LED_OP_STAT_REG(self_p->base_addr);
    
    reg32 &= ~AKI7X4SELASCII_MASK;
    reg32 |= (is_on) ? AKI7X4SELASCII_MASK : 0x0;
    DBMS3( stderr, "ASCII_MODE=0x%08x" EOL_CRLF, reg32);

    AKI7LED_OP_STAT_REG(self_p->base_addr) = reg32;

    return;
}

/**
 * @fn int mbmcs_aki7seg74595x4spi_write_ascii_digit(const mbmcs_aki7seg74595x4spi_t *const self_p, const char * const str)
 * @brief ASCII���[�h�̃f�[�^��4�����܂ł̕�����ŗ^���܂��B
 * @param self_p mbmcs_aki7seg74595x4spi_t�I�u�W�F�N�g�|�C���^
 * @param str 4�����܂ł̕�����
 * @retval 0 ����
 * @retval EINVAL 4�����𒴂��Ă���
 **/
int  mbmcs_aki7seg74595x4spi_write_ascii_digit(const mbmcs_aki7seg74595x4spi_t *const self_p, const char * const str)
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

    AKI7LED_ASCII_REG(self_p->base_addr) = reg32;

    return 0;
}

/**
 * @fn int  mbmcs_aki7seg74595x4spi_destroy(mbmcs_aki7seg74595x4spi_t *const self_p)
 * @brief �V�������\�������������܂��B
 * @param self_p mbmcs_aki7seg74595x4spi_t�I�u�W�F�N�g�|�C���^
 **/
void mbmcs_aki7seg74595x4spi_send_to_module(const mbmcs_aki7seg74595x4spi_t *const self_p)
{
    uint32_t reg32=0;

    reg32 = AKI7LED_OP_STAT_REG(self_p->base_addr);

    reg32 |= AKI7X4SEND_MASK;

    AKI7LED_OP_STAT_REG(self_p->base_addr) = reg32;

    return;
}

/**
 * @fn int mbmcs_aki7seg74595x4spi_brightness(const mbmcs_aki7seg74595x4spi_t *const self_p, const uint8_t val)
 * @brief �P�x�l��ω������܂�
 * @param self_p mbmcs_aki7seg74595x4spi_t�I�u�W�F�N�g�|�C���^
 * @param val �P�x�l 0 - 255
 **/
void  mbmcs_aki7seg74595x4spi_brightness(const mbmcs_aki7seg74595x4spi_t *const self_p, const uint8_t val)
{
    uint32_t reg32=0;

    reg32 = AKI7LED_OP_STAT_REG(self_p->base_addr);

    reg32 &= ~AKI7X4BRIGHTNESS_MASK;
    reg32 |= AKI7X4BRIGHTNESS_MASK &  (val << AKI7X4BRIGHTNESS_SHIFT);

    AKI7LED_OP_STAT_REG(self_p->base_addr) = reg32;

    return;
}

/**
 * @fn void mbmcs_aki7seg74595x4spi_force_send_sync_timing_is_on(const mbmcs_aki7seg74595x4spi_t *const self_p, uint8_t is_on)
 * @brief BIN���[�h��ASCII���[�h��؂�ւ��܂�
 * @param self_p mbmcs_aki7seg74595x4spi_ascii_mode�I�u�W�F�N�g�|�C���^
 * @brief is_on 0:BIN���[�h 0�ȊO:ASCII���[�h
 **/
void mbmcs_aki7seg74595x4spi_force_send_sync_timing_is_on(const mbmcs_aki7seg74595x4spi_t *const self_p, uint8_t is_on)
{
    uint32_t reg32 = AKI7LED_OP_STAT_REG(self_p->base_addr);
    
    reg32 &= ~AKI7X4FORCESENDSYNCTMG_MASK;
    reg32 |= (is_on) ? AKI7X4FORCESENDSYNCTMG_MASK : 0x0;
    DBMS3( stderr, "FORCESENDSYNCTMG_MODE=0x%08x" EOL_CRLF, reg32);

    AKI7LED_OP_STAT_REG(self_p->base_addr) = reg32;

    return;
}


/**
 * @fn int  mbmcs_aki7seg74595x4spi_destroy(mbmcs_aki7seg74595x4spi_t *const self_p)
 * @brief �C���X�^���X��j�����܂��B���݂��邱�Ƃ�����܂���B
 **/
int  mbmcs_aki7seg74595x4spi_destroy(mbmcs_aki7seg74595x4spi_t *const self_p)
{
    (void)self_p;

    return 0;
}
