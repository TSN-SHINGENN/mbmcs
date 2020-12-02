/**
 *      Copyright 2019 TSN.SHINGENN
 *
 *      Basic Author: Seiichi Takeda  '2019-December-20 Active
 *
 *              Last Alteration $Author: seiichi $
 */

/* Standard C Library */
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <errno.h>

/* libmultios */
#include "multios_lite_sprintf.h"

/* this */
#include "mbmcs_printk.h"

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

extern void mbmcs_printk_initializer(void (*putc_cb_func)(const int))
	__attribute__ ((optimize("Os")));

int mbmcs_printk( const char *const fmt, ...)
	__attribute__ ((optimize("Os")));

#endif

static void (*_printk_putc_cb)(int) = NULL;

/**
 * @fn void mbmcs_printk_initializer(int (*putchar_cb_func)(const int))
 * @brief mbmcs_printk()�̏o�͐���R�[���o�b�N�Őݒ肵�܂��B
 *	�h���C�o�f�o�b�O�����Ȃ̂ŁA���荞�ݏ����������CPU����������̂ŕ�����
 * @param putchar_cb_func putchar�R�[���o�b�N�֐�
 **/
void mbmcs_printk_initializer(void (*putc_cb_func)(const int))
{
    _printk_putc_cb = putc_cb_func;

    return;
}

/**
 * @brief mbmcs_printk_initializer()�Ŏw�肳�ꂽ�o�͂�fmt�����ŏo�͂��܂��B
 * @param fmt ������t�H�[�}�b�g�A�����_�����͑ΏۊO 
 * @param ... �ϒ�����
 **/
int mbmcs_printk( const char *const fmt, ...)
{
    int retval;
    va_list ap;

    if(NULL == _printk_putc_cb) {
	return -1;
    }

    va_start(ap, fmt);
    retval = _multios_vsprintf_lite( _printk_putc_cb, fmt, ap);
    va_end(ap);

    return retval;
}


