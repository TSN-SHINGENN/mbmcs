/*! \mainpage libmbmcsリファレンスガイド
 * 
 * \section 概要
 *  libmbmcsはXilinx社FPGAで提供されるソフトIPのCPUで動作するStandaloneライブラリを支援します。<br> 
 *  libmbmcsはlibmultiosと協調して支援を行うため、libmultiosの様々なテンプレートライブラリを使用可能です。<br>
 *  MicroblazeのStandaloneはスレッドアルゴリズムを提供していないため、スレッド関連のテンプレートは切り離されます。<br>
 *  Standaloneで提供されるmalloc(メモリアロケータ)はメモリの消費量が大きいです。<br>
 *  multios_malloc_lite()を使用することをお勧めします。libmbmcsライブラリの内部で初期化します。<br>
 *
 * \section 使い方
 *
 * \section リポジトリ情報
 *
 * 秘密です。<br>
 * 
 * \section 著作権
 * &nbsp; Copyright(C) TSN･SHINGENN <br>
 * <br>
 * <br>
 **/
/**
 * @file libmbmcs.c
 * @brief libmbmcsライブラリの情報取得関数
 */

/* libmultios */
// #include <libmultios/multios_endian.h>
#include "multios_endian.h"

/* this */
#include "libmbmcs_revision.h"
#include "libmbmcs.h"

#if defined(__GNUC__)
extern const char *mbmcs_get_lib_revision(void)
	__attribute__ ((optimize("Os")));
extern const char *mbmcs_get_lib_name(void)
	__attribute__ ((optimize("Os")));
#endif


/**
 * @fn const char *multios_get_lib_revision(void)
 * @brief libmbmcsの版管理番号の取得
 * @return 版管理番号文字列ポインタ
 */
const char *mbmcs_get_lib_revision(void)
{
    return LIB_MBMCS_REVISION;
}

/**
 * @fn const char *mbmcs_get_lib_name(void)
 * @brief libmbmcsライブラリの標準名を返します
 * @return 標準名文字列ポインタ
 */
const char *mbmcs_get_lib_name(void)
{
    return LIB_MBMCS_NAME;
}

const char *const _libmbmcs_idkeyword =
    "@(#)libmultios_idkeyword : " LIB_MBMCS_NAME " revision"
    LIB_MBMCS_REVISION
    " CopyRight TSN.SHINGENN All Rights Reserved.";

/**
 * @fn const char *mbmcs_get_idkeyword(void)
 * @brief libmultios_idkeywordを返します。
 * @return idkeyword文字列ポインタ
 */
const char *mbmcs_get_idkeyword(void)
{
    return _libmbmcs_idkeyword;
}
