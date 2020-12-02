/**
 *      Copyright 2018 TSN.SHINGENN
 *
 *      Basic Author: Seiichi Takeda  '2018-June-13 Active
 *
 *              Last Alteration $Author: seiichi $
 */

/* Standard C Library */
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* libmultios */
#include "multios_lite_sprintf.h"

/* this */
#include "mbmcs_stdio.h"

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

#if 1
extern void outbyte (char); /* microlaze mcs */

extern void mbmcs_printf_stdout_initializer(void (*)(const int))
	__attribute__ ((optimize("Os")));
extern int mbmcs_putchar(const int)
	__attribute__ ((optimize("Os")));
extern int mbmcs_printf( const char *const, ...)
	__attribute__ ((optimize("Os")));
extern int mbmcs_snprintf( char *const, const size_t, const char *const, ...)
	__attribute__ ((optimize("Os")));

#endif




/**
 * @fn int mbmcs_putchar(const int code)
 * @brief  mbmcs���C�u�����̕W���o�͂�1�����o�͂��܂��B
 * 	C�����^�C���K�i�ɏ������܂��B
 *	outbyte() �� Microblaze MCS��UART�o�͂�1�o�C�g�o�͂��܂��B
 * @param code ASCII�L�����N�^�R�[�h
 * @return unsigned char �Ƃ��ď������܂ꂽ������ int �ɃL���X�g���ĕԂ��B
 *	�G���[�̏������킩��Ȃ��̂�EOF��Ԃ����Ƃ͂Ȃ�
 **/
int mbmcs_putchar(const int code)
{
    const char c8 = (char)(0xff & code);

    outbyte(c8);

    return (int)c8;
}

/**
 * @fn static void own_putchar_cb(const int code)
 * @brief mbmcs_printf_stdout_initializer()�Ńf�t�H���g�l�œn���R�[���o�b�N
 * @param code ASCII�L�����N�^�R�[�h
 **/
static void own_putchar_cb(const int code)
{
    mbmcs_putchar(code);

    return;
}

/**
 * @fn void mbmcs_printf_stdout_initializer(void (*const putchar_cb_func)(const int))
 * @brief �W���o�͂Ƃ��ĂP�����o�͂���֐����R�[���o�b�N�Őݒ肵�܂��B
 *	main()�̈�ԍŏ��̏����� __attribute__((constructor))�ōŏ��ɏ������Ă��������B
 * @param putchar_cb_func NULL�w��Ńf�t�H���g�l
 **/
void mbmcs_printf_stdout_initializer(void (*putchar_cb_func)(const int))
{
    if(NULL == putchar_cb_func) {
	putchar_cb_func = own_putchar_cb;
    }

    multios_lite_printf_init(putchar_cb_func);

    return;
}

/**
 * @fn int mbmcs_printf( const char *const fmt, ...)
 * @brief �W���o�͂Ɏw�肳��Ă�����fmt�ɏ]���ĕ�������o�͂��܂��B
 * @param fmt ������t�H�[�}�b�g�A�����_�����͑ΏۊO 
 * @param ... �ϒ�����
 * @retval 0 �ȏ� �o�͂���������
 * @retval -1 �G���[ errno�Q��
 **/
int mbmcs_printf( const char *const fmt, ...)
{
    int retval;
    va_list ap;

//    xil_printf( "%s : fmt=%s" EOL_CRLF,__func__, fmt);

    va_start(ap, fmt);
    retval = multios_lite_vprintf(fmt, ap);
    va_end(ap);

    return retval;
}
/**
 * @fn int mbmcs_snprintf( char *buf, size_t size, const char *const format, ...)
 * @brief buf��printf�̏����ŕ������o�͂��܂��B
 * @param fmt ������t�H�[�}�b�g�A�����_�����͑ΏۊO 
 * @param ... �ϒ�����
 * @retval 0 �ȏ� �o�͂���������
 * @retval -1 �G���[ errno�Q��
 **/
int mbmcs_snprintf( char *const buf, const size_t size, const char *const fmt, ...)
{
    int retval;
    va_list ap;

//    xil_printf( "%s : fmt=%s" EOL_CRLF,__func__, fmt);

    va_start(ap, fmt);
    retval = multios_lite_vsnprintf( buf, size, fmt, ap);
    va_end(ap);

    return retval;
}




