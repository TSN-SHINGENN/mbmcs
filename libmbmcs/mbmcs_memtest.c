/**
 *      Copyright 2015 TSN.SHINGENN
 *
 *	Basic Author: Seiichi Takeda  '2015-November-13 Active
 *		Last Alteration $Author: takeda $
 */

/* POSIX or CRL */ 
#include <stdint.h>
#include <stdio.h>
#include <errno.h>

/* libmultios */
//#include "libmultios/multios_stdlib.h"
#include "multios_stdlib.h"
#include "multios_rand.h"
#include "mbmcs_memtest.h"

/* dbms */
#ifdef DEBUG
static int debuglevel = 4;
#else
static const int debuglevel = 1;
#endif
#include "dbms.h"

#if defined(_MULTIOS_DMSG_IS_UART)
#define EOL_CRLF "\n\r"
#else
#define EOL_CRLF "\n"
#endif

extern uint32_t mbmcs_memory_test( void * const, const size_t, const mbmcs_memory_test_attr_t *const)
	__attribute__ ((optimize("Os")));

/**
 * @fn uint16_t mbmcs_memory_test( void * const buf, const size_t bufsz , const uint8_t testflag)
 * @brief 指定されたメモリ空間のメモリテストを行います
 * @param buf テストを行うメモリ空間の先頭アドレス
 * @param bufsz メモリ空間のバイトサイズ
 * @param testflag 0:デフォルトテスト項目 ビットフラグ指定(0x1:8ビット単位 0x2:16ビット単位 0x4:32ビット単位　その他：予約)
 **/
uint32_t mbmcs_memory_test( void * const buf, const size_t bufsz , const mbmcs_memory_test_attr_t *const attr_p) 
{
    const uint32_t d32_ary[4] = { 0x47bd9f6a, 0x1e806c95, 0x2fdc3a5c, 0xa5a55a5a };
    uint32_t err = 0;

    if((attr_p->flags==0) || (attr_p->f.fix8)) {
        size_t n;
	volatile uint8_t *p = (uint8_t*)buf;
	const uint8_t d8[4] = { 0xff & d32_ary[0],  0xff & d32_ary[1],  0xff & d32_ary[2],  0xff & d32_ary[3]};
	const size_t u8_cnt = bufsz;

	DMSG( stderr, "U8..");
	for( n=0; n<u8_cnt; n+=4 ) {
	//  DMSG( stderr, "n=%d 0x%p" EOL_CRLF, n, p);
	//  DMSG( stderr, "%p : %02x%02x%02x%02x : ", p, d8[0], d8[1], d8[2], d8[3]);
	    *p++ = d8[0];
	    *p++ = d8[1];
	    *p++ = d8[2];
	    *p++ = d8[3];
	    p -= 4; 
//	    DMSG( stderr, "%08x ", *(uint32_t*)p);
//	    DMSG( stderr, "%02x%02x%02x%02x : %08x : ", *p, *(p+1), *(p+2), *(p+3), *(uint32_t*)p);
 	    err |= ( *p++ != d8[0]) ? 0x1 : 0;
//	    DMSG( stderr, "%02x ", 0xff & *p);
 	    err |= ( *p++ != d8[1]) ? 0x2 : 0;
//	    DMSG( stderr, "%02x ", 0xff & *p);
 	    err |= ( *p++ != d8[2]) ? 0x4 : 0;
//	    DMSG( stderr, "%02x" EOL_CRLF, 0xff & *p);
 	    err |= ( *p++ != d8[3]) ? 0x8 : 0;
#if 0
	    if(err) {
		DMSG( stderr, "NG" EOL_CRLF);
	    } else {
		DMSG( stderr, "OK" EOL_CRLF);
	    }
#endif
	}
	if(err) goto out;
	DMSG( stderr, "OK! ");
    }

    if((NULL == attr_p) || (attr_p->f.fix16)) {
        size_t n;
	volatile uint16_t *p = (uint16_t*)buf;
	const uint16_t d16[4] = { 0xffff & d32_ary[0],  0xffff & d32_ary[1],  0xffff & d32_ary[2],  0xffff & d32_ary[3]};
	const size_t u16_cnt = bufsz / sizeof(uint16_t);

	DMSG( stderr, "U16..");
	for( n=0; n<u16_cnt; n+=4 ) {
	//  DMSG( stderr, "n=%d 0x%p" EOL_CRLF, n, p);
//	    DMSG( stderr, "%p : %04x%04x%04x%04x : ", p, d16[1], d16[2], d16[3], d16[0]);
	    *p++ = d16[1];
	    *p++ = d16[2];
	    *p++ = d16[3];
	    *p++ = d16[0];
	    p -= 4;
//	    DMSG( stderr, "%08x%08x ", *(uint32_t*)p, *(uint32_t*)(p+1));
//	    DMSG( stderr, "%04x%04x%04x%04x : %08x%08x : ", *(p+0), *(p+1), *(p+2), *(p+3), *(uint32_t*)p, *(uint32_t*)(p+2));
 	    err |= ( *p++ != d16[1] ) ? 0x10 : 0;
 	    err |= ( *p++ != d16[2] ) ? 0x20 : 0;
 	    err |= ( *p++ != d16[3] ) ? 0x40 : 0;
 	    err |= ( *p++ != d16[0] ) ? 0x80 : 0;
#if 0
	    if(err & 0xf0) {
		DMSG( stderr, "NG" EOL_CRLF);
	    } else {
		DMSG( stderr, "OK" EOL_CRLF);
	    }
#endif
	}
	if(err) goto out;
	DMSG( stderr, "OK! ");
    }

    if((NULL == attr_p) || (attr_p->f.fix32)) {
        size_t n;
	volatile uint32_t *p = (uint32_t*)buf;
	const size_t u32_cnt = bufsz / sizeof(uint32_t);

	DMSG( stderr, "U32..");
	for( n=0; n<u32_cnt; n+=4 ) {
	  // DMSG( stderr, "n=%d 0x%p" EOL_CRLF, n, p);
	  // DMSG( stderr, "%p : fix32 %08x%08x%08x%08x : ", p, d32_ary[2], d32_ary[3], d32_ary[0], d32_ary[1]);
	    *p++ = d32_ary[2];
	    *p++ = d32_ary[3];
	    *p++ = d32_ary[0];
	    *p++ = d32_ary[1];
	    p -= 4;
	  // DMSG( stderr, "%08x%08x%08x%08x : ", *(p+0), *(p+1), *(p+2), *(p+3));
 	    err |= ( *p++ != d32_ary[2]) ? 0x400 : 0;
 	    err |= ( *p++ != d32_ary[3]) ? 0x800 : 0;
 	    err |= ( *p++ != d32_ary[0]) ? 0x100 : 0;
 	    err |= ( *p++ != d32_ary[1]) ? 0x200 : 0;
#if 0
	    if(err & 0xf00) {
		DMSG( stderr, "NG" EOL_CRLF);
	    } else {
		DMSG( stderr, "OK" EOL_CRLF);
	    }
#endif
	}
	if(err) goto out;
	DMSG( stderr, "OK! ");
    }

    if(attr_p->f.fix64) {
        size_t n;
	volatile uint64_t *p = (uint64_t*)buf;
	const uint16_t d64[2] = { d32_ary[0] | (d32_ary[1] <<32),  d32_ary[2] | (d32_ary[3]<<32)};
	const size_t u64_cnt = bufsz / sizeof(uint64_t);

	DMSG( stderr, "U64..");
	for( n=0; n<u64_cnt; n+=2 ) {
	  // DMSG( stderr, "n=%d 0x%p" EOL_CRLF, n, p);
	  // DMSG( stderr, "%p : fix64 %016x%016x : ", p, d64[1], d64[0]);
	    *p++ = d64[1];
	    *p++ = d64[0];
	    p -= 2;
	  // DMSG( stderr, "%016x%016x : ", *(p+0), *(p+1));
 	    err |= ( *p++ != d64[1]) ? 0x2000 : 0;
 	    err |= ( *p++ != d64[0]) ? 0x1000 : 0;
#if 0
	    if(err & 0xf00) {
		DMSG( stderr, "NG" EOL_CRLF);
	    } else {
		DMSG( stderr, "OK" EOL_CRLF);
	    }
#endif
	}
	if(err) goto out;
	DMSG( stderr, "OK! ");
    }

    if(attr_p->f.rnd32) {
	size_t n;
	volatile uint32_t *p = (uint32_t*)buf;
	const size_t u32_cnt = bufsz / sizeof(uint32_t);

	DMSG( stderr, "r32..");
	for( n=0; n<u32_cnt; ++n) {
	    static multios_rand_xorshift_t seed = MULTIOS_RAND_XORSHIFT_INIT_VALUE;
	    
	    *p = multios_rand_xorshift_exec(&seed);
	    ++p;
	}

	p = (uint32_t*)buf;
	for( n=0; n<u32_cnt; ++n) {
	    static multios_rand_xorshift_t seed = MULTIOS_RAND_XORSHIFT_INIT_VALUE;
	    uint32_t vc, vr;

	    vr = multios_rand_xorshift_exec(&seed);
	    vc = *p; ++p;
	    err |= (vr != vc) ? 0x1000f000 : 0;
//	    DMSG( stderr, "%p : rnd32 %08x %08x %s"EOL_CRLF, p, vr, vc, (err ? "NG" : "OK" ));
	}

	if(err) goto out;
	DMSG( stderr, "OK! ");
    }


out:
    if(err) {
	DMSG( stderr, "NG! 0x%x" EOL_CRLF, err);
    } else {
	DMSG( stderr, "OK! Checking Complete." EOL_CRLF);
    }

    return err;
//    return 0;
}


