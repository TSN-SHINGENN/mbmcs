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
 * @brief  mbmcsライブラリの標準出力に1文字出力します。
 * 	Cランタイム規格に準拠します。
 *	outbyte() は Microblaze MCSのUART出力に1バイト出力します。
 * @param code ASCIIキャラクタコード
 * @return unsigned char として書き込まれた文字を int にキャストして返す。
 *	エラーの条件がわからないのでEOFを返すことはない
 **/
int mbmcs_putchar(const int code)
{
    const char c8 = (char)(0xff & code);

    outbyte(c8);

    return (int)c8;
}

/**
 * @fn static void own_putchar_cb(const int code)
 * @brief mbmcs_printf_stdout_initializer()でデフォルト値で渡すコールバック
 * @param code ASCIIキャラクタコード
 **/
static void own_putchar_cb(const int code)
{
    mbmcs_putchar(code);

    return;
}

/**
 * @fn void mbmcs_printf_stdout_initializer(void (*const putchar_cb_func)(const int))
 * @brief 標準出力として１文字出力する関数をコールバックで設定します。
 *	main()の一番最初の処理か __attribute__((constructor))で最初に処理してください。
 * @param putchar_cb_func NULL指定でデフォルト値
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
 * @brief 標準出力に指定されている先にfmtに従って文字列を出力します。
 * @param fmt 文字列フォーマット、小数点処理は対象外 
 * @param ... 可変長引数
 * @retval 0 以上 出力した文字数
 * @retval -1 エラー errno参照
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
 * @brief bufにprintfの書式で文字を出力します。
 * @param fmt 文字列フォーマット、小数点処理は対象外 
 * @param ... 可変長引数
 * @retval 0 以上 出力した文字数
 * @retval -1 エラー errno参照
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




