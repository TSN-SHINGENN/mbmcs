/**
 *      Copyright 2019 TSN.SHINGENN
 *
 *	Basic Author: Seiichi Takeda  '2019-October-04 Active
 *		Last Alteration $Author: takeda $
 */

/**
 * @file mbmcs_pargen7x4.c
 * @brief 7-bit x 4(data) �̃f�[�^���p���e�B�t������32-bit�ŏ���
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
 * @brief mbmcs_pargen7x4���W���[���̃������}�b�v�h�A�h���X��o�^���܂�
 * @param self_p mbmcs_pargen7x4_t�C���X�^���X�|�C���^
 * @param base_addr mbmcs_stdintr���W���[���̃x�[�X�A�h���X
 * @retval 0 ����
 * @retval EINVAL �������s��
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
 * @brief mbmcs_pargen7x4���W���[���Ńp���e�B����������f�[�^��ݒ�
 *	�p���e�B�̕t���� mbmcs_pargen7x4_get_merged()�Ŏ擾���܂��B
 *	�p���e�B�폜�l��mbmcs_pargen7x4_get_masked()�Ŏ擾���܂��B
 *	�p���e�B�̐����`�F�b�N��mbmcs_pargen7x4_checked_is_succeed()�Ō��ʂ��擾���܂��B
 * @param self_p mbmcs_pargen7x4_t�C���X�^���X�|�C���^
 * @param d32_7x4 7bitx4��32-bit�f�[�^
 * @retval ���W�X�^�ēǍ�32-bit�f�[�^�l(��ɃR���y�A�p)
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
 * @brief mbmcs_pargen7x4_set_data()�Őݒ肵���f�[�^���擾
 * @param self_p mbmcs_pargen7x4_t�C���X�^���X�|�C���^
 * @return mbmcs_pargen7x4_set_data()�Őݒ肵��32-bit�f�[�^�l
 **/
uint32_t mbmcs_pargen7x4_get_data(mbmcs_pargen7x4_t* const self_p)
{
    const volatile uint32_t *const reg32_p = (uint32_t*)self_p->base_addr;

    return *reg32_p;
}


/**
 * @fn uint32_t mbmcs_pargen7x4_get_merged(mbmcs_pargen7x4_t* __restrict const self_p)
 * @brief mbmcs_pargen7x4_set_data()�Őݒ肵���f�[�^���擾
 * @param self_p mbmcs_pargen7x4_t�C���X�^���X�|�C���^
 * @return mbmcs_pargen7x4_set_data()�Őݒ肵��32-bit�f�[�^�l
 **/
uint32_t mbmcs_pargen7x4_get_merged(mbmcs_pargen7x4_t* __restrict const self_p)
{
    const volatile uint32_t *const reg32_p = (uint32_t*)self_p->base_addr;

    return *(reg32_p + 1);
}


/**
 * @fn uint32_t mbmcs_pargen7x4_get_masked(mbmcs_pargen7x4_t* __restrict const self_p)
 * @brief mbmcs_pargen7x4_set_data()�Őݒ肵���f�[�^����p���e�B���}�X�N�����f�[�^���擾
 * @param self_p mbmcs_pargen7x4_t�C���X�^���X�|�C���^
 * @return mbmcs_pargen7x4_set_data()�Őݒ肵��32-bit�f�[�^�l����p���e�B�r�b�g���폜�����l
 **/
uint32_t mbmcs_pargen7x4_get_masked(mbmcs_pargen7x4_t* __restrict const self_p)
{
    const volatile uint32_t *const reg32_p = (uint32_t*)self_p->base_addr;

    return *(reg32_p + 2);
}

/**
 * @fn int mbmcs_pargen7x4_checked_is_failure(mbmcs_pargen7x4_t* __restrict const self_p)
 * @brief mbmcs_pargen7x4_set_data()�Őݒ肵���f�[�^���p���e�B�`�F�b�N�Ɏ��s������^�l�ɂȂ�܂��B
 * @param self_p mbmcs_pargen7x4_t�C���X�^���X�|�C���^
 * @retval 0 ����
 * @retval ~0 ���s
 **/
int mbmcs_pargen7x4_checked_is_failure(mbmcs_pargen7x4_t* __restrict const self_p)
{
    const volatile uint32_t *const reg32_p = (uint32_t*)self_p->base_addr;

    return (*(reg32_p) != *(reg32_p+1)) ? ~0 : 0;
}
