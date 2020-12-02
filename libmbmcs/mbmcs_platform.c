/**
 *      Copyright 2015 TSN.SHINGENN
 *
 *      Basic Author: Seiichi Takeda  '2015-August-01 Active
 *              Last Alteration $Author: seiichi $
 **/

/* Cランタイム＆POSIX */
#include <sys/types.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/* MB MCSランタイム */
#include <xparameters.h>
#include <xiomodule.h>
#include <mb_interface.h>

/* libmultios */
#include "multios_sys_types.h"
#include "multios_lite_mallocater.h"
#include "multios_stl_list.h"
#include "multios_stdlib.h"

/* this */
#include "libmbmcs_libmultios_bridge.h"
#include "mbmcs_stdio.h"
#include "mbmcs_stdintrc.h"
#include "mbmcs_MONOTONIC_TIMER.h"
#include "mbmcs_platform.h"

/* debug */
#ifdef __GNUC__
__attribute__ ((unused))
#endif
#ifdef DEBUG
static int debuglevel = 5;
#else
static const int debuglevel = 0;
#endif
#include "dbms.h"

#if defined(_MULTIOS_DMSG_IS_UART)
#define EOL_CRLF "\n\r"
#else
#define EOL_CRLF "\n"
#endif

typedef struct _libmbmcs_status {
    XIOModule iomodule;
#ifdef MBMCS_ENABLE_HARDWARE_JIFFIES_COUNTER
    mbmcs_monotonic_timer_t hwmonotonic;
#endif
#ifdef MBMCS_ENABLE_FIT1_JIFFIES_COUNTER
    volatile uint64_t jiffies;
#endif /* end of MBMCS_ENABLE_FIT1_JIFFIES_COUNTER */
#ifdef MBMCS_ENABLE_EXTERNAL_INTERRUPT_WITH_MBMCS_STDINTR
    mbmcs_stdintrc_t stdintrc;
    multios_stl_list_t intr_handler_queue;
    multios_stl_list_t isobj_handler_queue;
#endif /* end of MBMCS_ENABLE_EXTERNAL_INTERRUPT_WITH_MBMCS_STDINTR */
#if defined(MBMCS_ENABLE_FIT1_JIFFIES_COUNTER) && defined(MBMCS_ENABLE_SOFTWARE_INTERVAL_TIMER)
    multios_stl_list_t interval_timer_handler_queue;
    multios_stl_list_t itobj_handler_queue;
#endif /* end of MBMCS_ENABLE_SOFTWARE_INTERVAL_TIMER */
    int num_intr_locks;
    union {
	uint8_t flags;
	struct { 
	    uint8_t stdintrc:1;
	    uint8_t intr_fit1:1;
	    uint8_t virtual_heap:1;
	    uint8_t intr_ext:1;
	    uint8_t intr_handler_queue:1;
	    uint8_t isobj_handler_queue:1;
	    uint8_t interval_timer_handler_queue:1;
	    uint8_t itobj_handler_queue:1;
	} f;
    } init;

    union {
	uint8_t flags;
	struct {
	    uint8_t uart_echoback:1;
	} f;
    } stat;

} libmbmcs_status_t;

typedef struct {
    uintptr_t intrc_addr_offset;
    uint32_t intr_bitmask;
    void (*isr_func)(void*);
    void (*isobj_func)(void*);
    void *args_p;
} intr_handler_table_t;

typedef struct {
    void (*isobj_func)(void*);
    void *args_p;
} isobj_handler_table_t;

typedef struct {
    size_t interval_time_msec;
    size_t timer_counter_msec;
    void (*timer_cb_func)(void*);
    void *args_p;
} interval_timer_handler_t;

static libmbmcs_status_t mbmcs_stat = {{ 0 }};

/* GCC最適化オプション指定のためのプロトタイプ宣言 */
//#if defined(__GNUC__)
extern int mbmcs_platform_init(void *const, const size_t)
	__attribute__ ((optimize("Os")));
extern int mbmcs_platform_monotonic_hardware_timer_init(const uintptr_t)
	__attribute__ ((optimize("Os")));
extern void mbmcs_platform_interrupt_post_process(void (* const itobj_exec_func)(void),void (* const iobj_exec_func)(void) )
	__attribute__ ((optimize("Os")));
extern int mbmcs_platform_uart_gets( char * const, const size_t, void (*const)(void),void (* const)(void) )
	__attribute__ ((optimize("Os")));
extern int mbmcs_platform_stdintr_init(const uintptr_t base_addr)
	__attribute__ ((optimize("Os")));
extern int mbmcs_platform_stdintr_add_interrupt_service_functions(const uintptr_t, const uint32_t, void(*const)(void *const), void (*)(void *const ), void * const, const bool)
	__attribute__ ((optimize("Os")));
extern void mbmcs_platform_stdintr_exec_isobj_funcs(void)
	__attribute__ ((optimize("O2")));
extern int mbmcs_platform_stdintr_enable_intr_signal_output(const uintptr_t)
	__attribute__ ((optimize("Os")));
extern int mbmcs_platform_stdintr_disable_intr_signal_output(const uintptr_t)
	__attribute__ ((optimize("Os")));
extern int mbmcs_platform_interval_timer_service_init(void)
	__attribute__ ((optimize("Os")));
extern int mbmcs_platform_interval_timer_add_service_function(const uint32_t, void(*const)(void *const), void * const, const bool)
	__attribute__ ((optimize("Os")));
extern void mbmcs_platform_interval_timer_service_exec_funcs(void)
	__attribute__ ((optimize("O2")));
extern void mbmcs_platform_disable_interrupts(void)
	__attribute__ ((optimize("Os")));
extern void mbmcs_platform_enable_interrupts(void)
	__attribute__ ((optimize("Os")));
extern uint32_t mbmcs_platform_read_register32(const uintptr_t)
	__attribute__ ((optimize("O1")));
extern void mbmcs_platform_write_register32(const uintptr_t,const uint32_t)
	__attribute__ ((optimize("O1")));
//#endif /* end of optimize option for GCC */

/* 外部参照される変数 */
#ifdef MBMCS_ENABLE_FIT1_JIFFIES_COUNTER
const volatile uint64_t *const mbmcs_jiffies_p = &mbmcs_stat.jiffies;
#endif /* end of MBMCS_ENABLE_FIT1_JIFFIES_COUNTER */

XIOModule * const mbmcs_iomodule_p = &mbmcs_stat.iomodule;

#define get_mbmcs_stat_p() (&mbmcs_stat)


static void check_interrput_mismatch(const char *const)
	__attribute__ ((optimize("Os")))
	__attribute__((unused));

static void check_interrput_mismatch(const char *const function_name)
{
    libmbmcs_status_t * const s = get_mbmcs_stat_p();

    if(s->num_intr_locks) {
	DMSG(stderr, "%s : number of interrupt locks %d, mismatch" EOL_CRLF,
	    function_name, s->num_intr_locks);
   	abort(); 
    }
    return;
}

static void fit1_intr_jiffes_handler(void *p) 
	__attribute__ ((save_volatiles))
	__attribute__ ((optimize("O2")));

static void fit1_intr_jiffes_handler(void *p)
{
    libmbmcs_status_t * const s = get_mbmcs_stat_p();

    DBMS5( stderr, "%s : execute" EOL_CRLF, __func__);
#if defined(MBMCS_ENABLE_FIT1_JIFFIES_COUNTER) && !defined(MBMCS_ENABLE_HARDWARE_JIFFIES_COUNTER)
    ++(s->jiffies);
#elif defined(MBMCS_ENABLE_FIT1_JIFFIES_COUNTER) && defined(MBMCS_ENABLE_HARDWARE_JIFFIES_COUNTER)
	s->jiffies = (MULTIOS_HINT_LIKELY(s->hwmonotonic.base_addr)) ?
		mbmcs_monotonic_timer_get_timer_counter(&s->hwmonotonic) : (s->jiffies)+1;
#endif
#if defined(MBMCS_ENABLE_FIT1_JIFFIES_COUNTER)
    if(MULTIOS_HINT_UNLIKELY(s->jiffies & ((uint64_t)(1) << 63))) {
	s-> jiffies = 0;
    }
#endif
#if defined(MBMCS_ENABLE_FIT1_JIFFIES_COUNTER) && defined(MBMCS_ENABLE_SOFTWARE_INTERVAL_TIMER)
    if(MULTIOS_HINT_LIKELY( s->init.f.interval_timer_handler_queue)) {
	const size_t num_funcs = multios_stl_list_get_pool_cnt(
			&s->interval_timer_handler_queue);
	size_t n;
	static uint64_t old_jiffies = 0;
	static uint64_t intr_laps;

	intr_laps = (s->jiffies == 0) ? 0 : (s->jiffies - old_jiffies);
	if(MULTIOS_HINT_LIKELY(intr_laps)) {
	    old_jiffies = s->jiffies;
	} else {
	    DBMS5( stderr, "%s : jiffies not incliment" EOL_CRLF, __FUNCTION__);
	    XIOModule_Acknowledge(&s->iomodule, XIN_IOMODULE_FIT_1_INTERRUPT_INTR);
	    return;
 	}

	if(!num_funcs) {
	    DBMS5( stderr, "%s : service functions are nothing" EOL_CRLF, __FUNCTION__);
	    XIOModule_Acknowledge(&s->iomodule, XIN_IOMODULE_FIT_1_INTERRUPT_INTR);
	    return;
	}
	
	// xil_printf( "%s : num_funcs = %d" EOL_CRLF, __FUNCTION__, (int)num_funcs);
	for( n=0; n<num_funcs; ++n) {
	    interval_timer_handler_t ith_tab;
	    int result;

	    result = multios_stl_list_get_element_at(
			    &(s->interval_timer_handler_queue), n, &ith_tab, sizeof(ith_tab));
	    if(result) {
		char strbuf[MULTIOS_XTOA_DEC64_BUFSZ];
		IFDBG5THEN {
		    DMSG( stderr, "%s : multios_stl_list_get_element_at(%s) fail : strerr=%s" EOL_CRLF,
				    __FUNCTION__, multios_u64toadec((uint64_t)n, strbuf, MULTIOS_XTOA_DEC64_BUFSZ), strerror(result));
		}
		abort();
	    }
	    
	    // ++(ith_tab.timer_counter_msec);
	    ith_tab.timer_counter_msec += intr_laps; 
            // xil_printf( " counter = %d time_msec=%d" EOL_CRLF, (int)ith_tab.timer_counter_msec,(int)ith_tab.interval_time_msec);
	    if(ith_tab.timer_counter_msec >= ith_tab.interval_time_msec) {
		// xil_printf( "c:%d t:%d j:%lld" EOL_CRLF, (int)ith_tab.timer_counter_msec,
			// (int)ith_tab.interval_time_msec, (long long int)s->jiffies);
	        isobj_handler_table_t itobj_tab;
                itobj_tab.isobj_func = ith_tab.timer_cb_func;
                itobj_tab.args_p = ith_tab.args_p;
		result = multios_stl_list_push_back( &(s->itobj_handler_queue), &itobj_tab, sizeof(itobj_tab));
		if(result) {
		    DBMS1( stderr, "%s : multios_stl_list_push_back(isobj_handler_queue) fail : strerror= %s" EOL_CRLF, __FUNCTION__, strerror(result));
		    abort();
		}
	        (ith_tab.timer_counter_msec) = 0;
	    }
	    result = multios_stl_list_overwrite_element_at(
			    &(s->interval_timer_handler_queue), n, &ith_tab, sizeof(ith_tab));
	    if(result) {
		IFDBG5THEN {
		    char strbuf[MULTIOS_XTOA_DEC64_BUFSZ];
		    DMSG( stderr, "%s : multios_stl_list_overwrite_element_at(%s) fail : strerr = %s" EOL_CRLF,
				    __FUNCTION__, multios_u64toadec((uint64_t)n, strbuf, MULTIOS_XTOA_DEC64_BUFSZ), strerror(result));
		}
		abort();
	    }
	}
    }
#endif /* end of MBMCS_ENABLE_SOFTWARE_INTERVAL_TIMER */
    XIOModule_Acknowledge(&s->iomodule, XIN_IOMODULE_FIT_1_INTERRUPT_INTR);

    return;
}


static void extend_intr_handler(void *p)
	__attribute__ ((save_volatiles))
	__attribute__ ((optimize("O2")));

static void extend_intr_handler(void *p)
{
    int result;
    libmbmcs_status_t * const s = get_mbmcs_stat_p();
    size_t n;
    const size_t num_funcs = multios_stl_list_get_pool_cnt(&s->intr_handler_queue);
    uint32_t reg32 __attribute__((unused));

    /* ライブラリから割り込み処理として呼ばれているので 割り込みの操作はしない */
    DBMS5( stderr, "%s : execute, num_funcs=%d" EOL_CRLF, __FUNCTION__, (int)num_funcs);
    if(!num_funcs) {
	DMSG( stderr, "%s : service functions are nothing" EOL_CRLF, __FUNCTION__);
#if 0 /* Spaltern-6 */
	XIOModule_Acknowledge(&s->iomodule, XIN_IOMODULE_EXTERNAL_INTERRUPT_INTR);
#else /* Artix-7 */
	XIOModule_Acknowledge(&s->iomodule, XIN_IOMODULE_GPI_1_INTERRUPT_INTR);
#endif
	return;
    }
//	mbmcs_stdintrc_get_irq_flag( &s->stdintrc, 0x0, &reg32);
//	DMSG( stderr, "%s : intrreg(0)=0x%08x" EOL_CRLF, __FUNCTION__, reg32);
//      _multios_lite_mallocater_dump_region_list(&_multios_lite_mallocater_heap_emulate_obj);

    for( n=0; n<num_funcs; ++n) {
	intr_handler_table_t intr_tab;
	uint32_t intr_bitflags;

	result = multios_stl_list_get_element_at( &(s->intr_handler_queue), n, &intr_tab, sizeof(intr_tab));
	if(result) {
	    char strbuf[MULTIOS_XTOA_DEC64_BUFSZ];
	    IFDBG5THEN {
		DMSG( stderr, "%s : multios_stl_list_get_element_at(%s) fail : strerr = %s" EOL_CRLF,
			__FUNCTION__, multios_u64toadec((uint64_t)n, strbuf, MULTIOS_XTOA_DEC64_BUFSZ),
			strerror(result));
		}
		abort();
	}
	result = mbmcs_stdintrc_get_irq_flag( &s->stdintrc, intr_tab.intrc_addr_offset, &intr_bitflags);
	if(result) {
	    IFDBG1THEN {
		    char strbuf[MULTIOS_XTOA_HEX32_BUFSZ];
		    multios_u32toahex( (s->stdintrc.base_addr + intr_tab.intrc_addr_offset), strbuf, MULTIOS_XTOA_HEX32_BUFSZ, NULL);
		    DMSG( stderr, "%s : mbmcs_stdintrc_get_irq_flag fail : addres:%08s strerror:%s" EOL_CRLF,
			__FUNCTION__, strbuf, strerror(result));
	    }
	    abort();
	}

	IFDBG5THEN {
	    char str1buf[MULTIOS_XTOA_HEX32_BUFSZ];
	    char str2buf[MULTIOS_XTOA_HEX32_BUFSZ];
	    multios_u32toahex( intr_bitflags, str1buf, MULTIOS_XTOA_HEX32_BUFSZ, NULL);
	    multios_u32toahex( intr_tab.intr_bitmask, str2buf, MULTIOS_XTOA_HEX32_BUFSZ, NULL);
	    DMSG( stderr. "%s : regster v=0x%08s intr_tab.intr_bitmask =0x%08s" EOL_CRLF,
		    __func__, str1buf, str2buf);
	}

	if( intr_bitflags & intr_tab.intr_bitmask ) {
	    /* IRQフラグのクリア */
	    result = mbmcs_stdintrc_clear_irq_flag( &(s->stdintrc), intr_tab.intrc_addr_offset, intr_tab.intr_bitmask);
	    if(result) {
		IFDBG5THEN {
		    DMSG( stderr, "%s : mbmcs_stdintrc_get_irq_flag fail" EOL_CRLF, __FUNCTION__);
		}
		abort();
	    }

	    if( NULL != intr_tab.isr_func ) {
		/* 割り込み中にすばやく行うことはここで実行 */
		intr_tab.isr_func( intr_tab.args_p );
	    }
	    if( NULL != intr_tab.isobj_func ) {
		IFDBG5THEN {
		    DMSG( stderr, "%s : set iobj process" EOL_CRLF, __FUNCTION__);
		}
		isobj_handler_table_t isobj_tab;
		isobj_tab.isobj_func = intr_tab.isobj_func;
		isobj_tab.args_p = intr_tab.args_p;
		result = multios_stl_list_push_back( &(s->isobj_handler_queue), &isobj_tab, sizeof(isobj_tab));
		if(result) {
		    DBMS1( stderr, "%s : multios_stl_list_push_back(isobj_handler_queue) fail : strerror=%s" EOL_CRLF,
				    __FUNCTION__, strerror(result));
		    abort();
		}
	    }
        }
    }
    if(multios_stl_list_get_pool_cnt(&(s->isobj_handler_queue))) {
	mbmcs_platform_stdintr_disable_intr_signal_output(0x0);
    }

#if 0 /* Spaltern-6 */
    XIOModule_Acknowledge(&s->iomodule, XIN_IOMODULE_EXTERNAL_INTERRUPT_INTR);
#else /* Artix-7 */
    XIOModule_Acknowledge(&s->iomodule, XIN_IOMODULE_GPI_1_INTERRUPT_INTR);
#endif
    return;
}

void mbmcs_platform_interrupt_post_process(void (* const itobj_exec_func)(void),void (* const iobj_exec_func)(void) )
{
    /*　割り込み処理コールバック */
    if( NULL != iobj_exec_func) {
	// xil_printf( "%s : isobj_exec_func" EOL_CRLF, __FUNCTION__);
	iobj_exec_func();
    }
    /* タイマー割り込み処理コールバック */
    if( NULL != itobj_exec_func ) {
	// xil_printf( "%s : itobj_exec_func" EOL_CRLF, __FUNCTION__);
	itobj_exec_func();
    }

    return;
}


int mbmcs_platform_uart_gets( char * const recvbuf, const size_t recvbufsz, void (* const itobj_exec_func)(void),void (* const iobj_exec_func)(void) )
{
    uint8_t *p = (uint8_t*)recvbuf;
    int len = 0;
    size_t remainsz = recvbufsz;
    libmbmcs_status_t * const s = get_mbmcs_stat_p();

    if( remainsz < 2 ) {
	return -1;
    }

    memset( p, 0x0, remainsz);
    --remainsz;

    while(remainsz) {
        int result;
	char c;
	while(( result = XIOModule_Recv( &(s->iomodule), p, 1)) == 0 ) {
	    /*　割り込み処理コールバック */
	    mbmcs_platform_interrupt_post_process( itobj_exec_func, iobj_exec_func);
    	}

	if( *p == '\t') *p= ' ';
	c = *p;
	switch(*p) {
	case '\r': // xil_printf("LF\r\n");
	case '\n': // xil_printf("CR\r\n");
	    ++len;
	case '\0': // xil_printf("null\r\n");
	    if(s->stat.f.uart_echoback) { mbmcs_printf("\r\n"); };
	    return len;
	case '\b': // xil_printf("BS\r\n");
	case 0x7f: // xil_printf("DEL\r\n");
	    if( len > 0 ) {
		--p;
		--len;
		if(s->stat.f.uart_echoback) { mbmcs_printf("\b \b"); };
	    }
	    break;
	default:
	    if(s->stat.f.uart_echoback) { mbmcs_printf("%c", c); };
	    ++p;
	    ++len;
	    --remainsz;
	}
	// xil_printf("%d %d\r\n", '\n', '\0');
	// xil_printf("size:%d len:%d code=%d \r\n", size, len, (int)c);
    }	    

    return (len==0) ? -1 : len;
}
/**
 * @fn int mbmcs_platform_uart_set_echoback_to_on(const bool is_true)
 * @brief コンソールのUARTにエコーバックを返信するかどうかを設定します
 * @param is_true true:エコーバックする false:エコーバックしない
 **/
void mbmcs_platform_uart_set_echoback_to_on(const int is_true)
{
    libmbmcs_status_t * const s = get_mbmcs_stat_p();
    const unsigned int f = (is_true!=0) ? ~0 : 0;

    s->stat.f.uart_echoback = f;

    // xil_printf("i%d f:%d %d" EOL_CRLF, is_true, (int)f, (int)(s->stat.f.uart_echoback));

    return;
}

/**
 * @fn int mbmcs_platform_uart_get_echoback_is_on(void)
 * @brief UARTエコーバックの状態を取得します
 * @retval 0以外 エコーバックON
 * @retval 0 エコーバックOFF
 **/
int mbmcs_platform_uart_get_echoback_is_on(void)
{
    const libmbmcs_status_t * const s = get_mbmcs_stat_p();
    const unsigned int u = (s->stat.f.uart_echoback!=0) ? ~0 : 0;
    const int i = (u!=0) ? ~0 : 0;

    // xil_printf("o%d u%d i%d" EOL_CRLF, (int)(s->stat.f.uart_echoback), (int)u, i);

    return i;
}


int mbmcs_platform_monotonic_hardware_timer_init(const uintptr_t base_addr)
{
#ifdef MBMCS_ENABLE_HARDWARE_JIFFIES_COUNTER
    libmbmcs_status_t * const s = get_mbmcs_stat_p();
    mbmcs_monotonic_timer_init(&s->hwmonotonic, base_addr);
    return 0;
#else
    return EPERM;
#endif
}

/**
 * @fn int mbmcs_platform_init(void)
 * @brief libmbmcsライブラリの初期化を行います。
 *	プログラムの増大を防ぐために機能の有効化は libmbmcs_platform.hの定義を参照します。
 *	MBMCS_ENABLE_FIT1_JIFFIES_COUNTER
 *	    FIT1タイマを使用して　jiffiesカウンタをインクリメントします。
 *　　　MBMCS_ENABLE_EXTERNAL_INTERRUPT
 *	    外部割り込みを使用可能にします。
 * FIT１タイマを使ってjiffiesを起動し、スリープ系関数を使用可能にします。
 */
int mbmcs_platform_init(void *const virtual_heap_buf, const size_t bufsz)
{
    int result;
    libmbmcs_status_t * const s = get_mbmcs_stat_p();
    XIOModule * const iom_p = &(s->iomodule);

    memset( s, 0x0, sizeof(libmbmcs_status_t));

#ifdef MBMCS_ENABLE_FIT1_JIFFIES_COUNTER
    s->jiffies = 0;
#ifdef MBMCS_ENABLE_HARDWARE_JIFFIES_COUNTER
    s->hwmonotonic.base_addr=0;
#endif
#endif

    s->num_intr_locks = 0;

    // Initialize module to obtain base address
    result = XIOModule_Initialize(iom_p, XPAR_IOMODULE_0_DEVICE_ID);

    // 割り込みを使うおまじない
    microblaze_register_handler(XIOModule_DeviceInterruptHandler, XPAR_IOMODULE_0_DEVICE_ID);

#ifdef MBMCS_ENABLE_FIT1_JIFFIES_COUNTER
    // FIT1を関連付け
    XIOModule_Connect(iom_p, XIN_IOMODULE_FIT_1_INTERRUPT_INTR, fit1_intr_jiffes_handler, NULL);
    // FIT1をENABLE
    XIOModule_Enable(iom_p,XIN_IOMODULE_FIT_1_INTERRUPT_INTR);
    s->init.f.intr_fit1 = 1;
#endif /* end of MBMCS_ENABLE_FIT1_JIFFIES_COUNTER */

    if( virtual_heap_buf != NULL ) {
	result = multios_lite_mallocater_init_obj(&_multios_lite_mallocater_heap_emulate_obj, virtual_heap_buf, bufsz);
	if(result) {
	    DBMS1(stderr, " mbmcs_platform_init : multios_lite_mallocater_init_obj fail:%s\r\n", strerror(result));
	    abort();
	}
	s->init.f.virtual_heap = 1;
    }

    s->stat.flags = 0;
    s->stat.f.uart_echoback = 0;

    // _multios_lite_mallocater_dump_region_list(&_multios_lite_mallocater_heap_emulate_obj);

    microblaze_enable_interrupts();
    XIOModule_Start(&s->iomodule);

    return 0;
}

int mbmcs_platform_stdintr_init(const uintptr_t base_addr)
{
#ifdef MBMCS_ENABLE_EXTERNAL_INTERRUPT_WITH_MBMCS_STDINTR
    int result;
    libmbmcs_status_t * const s = get_mbmcs_stat_p();

    // DBMS5( stderr, "%s : execute" EOL_CRLF, __FUNCTION__);

    if(!s->init.f.virtual_heap) {
	DBMS1( stderr, "%s : virtual heap not initalize" EOL_CRLF, __FUNCTION__);
	return ENOMEM;
    }

    result = mbmcs_stdintrc_init(&s->stdintrc, base_addr);
    if(result) {
	return result;
    }
    s->init.f.stdintrc=1;

    mbmcs_platform_disable_interrupts();

    result = mbmcs_stdintrc_enable_output( &s->stdintrc, 0x0);
    if( result ) {
        char strbuf[MULTIOS_XTOA_HEX64_BUFSZ];
	multios_u64toahex( (uint64_t)base_addr, strbuf, MULTIOS_XTOA_HEX64_BUFSZ, NULL);
	DBMS1( stderr, "%s : mbmcs_stdintrc_enable_output fail, invalid base_addr=0x%08s" EOL_CRLF,
			__FUNCTION__, strbuf);
	return result;
    }

#if 0
     // 外部割り込みを関連付け
    XIOModule_Connect(&s->iomodule, XIN_IOMODULE_EXTERNAL_INTERRUPT_INTR, extend_intr_handler, NULL);
    XIOModule_Enable(&s->iomodule, XIN_IOMODULE_EXTERNAL_INTERRUPT_INTR);
    s->init.f.intr_ext=1;
#else 
    // VIVADOでのMCS外部割込みをSDKのBSPがエラーにしてしまうのでGPIの立ち上がり割り込みを使う
    XIOModule_Connect(&s->iomodule, XIN_IOMODULE_GPI_1_INTERRUPT_INTR, extend_intr_handler, NULL);
    XIOModule_Enable(&s->iomodule, XIN_IOMODULE_GPI_1_INTERRUPT_INTR);
    s->init.f.intr_ext=1;
#endif

    result = multios_stl_list_init( &s->intr_handler_queue, sizeof(intr_handler_table_t));
    if(result) {
	DBMS1(stderr, "%s : multios_stl_list_init(intr_handler_queue) fail:%s\r\n",
			__FUNCTION__, strerror(result));
	abort();
    }
    s->init.f.intr_handler_queue = 1;
    result = multios_stl_list_init(&s->isobj_handler_queue, sizeof(isobj_handler_table_t));
    if(result) {
	DBMS1(stderr, "%s : multios_stl_list_init(isobj_handler_queue) fail:%s\r\n",
			__FUNCTION__, strerror(result));
	    abort();
    }
    s->init.f.isobj_handler_queue = 1;

    mbmcs_platform_enable_interrupts();
    XIOModule_Start(&s->iomodule);

    return 0;
#else /* end of MBMCS_ENABLE_EXTERNAL_INTERRUPT_WITH_MBMCS_STDINTR */
    DBMS1( stderr, "%s : not define MBMCS_ENABLE_EXTERNAL_INTERRUPT_WITH_MBMCS_STDINTR" EOL_CRLF, __func__);
    return EACCES;
#endif 
}
/**
 * @fn int mbmcs_platform_stdintr_add_interrupt_service_functions(const uintptr_t offset, const uint32_t intr_bitmask, void( * const isr_func)(void *const), void (*isobj_func)(void *const ), void * const args_p)
 * @brief 割り込みプロセスの関数を追加します。割り込み信号が入った場合は順番に処理されます。
 * @param offset メモリにマップされた割り込みレジスタのオフセット基準は　mbmcs_platform_stdintr_init で指定した base_addr
 * @param intr_bitmask 割り込みレジスタのフラグ位置
 * @param isr_handler_func 割り込みが入った段階で処理するコールバック関数ポインタ
 * @param isobj_handler_func 割り込みフラグ処理後に後処理されるコールバック関数ポインタ(NULLで処理は無し）
 * @param args_p isr_handler/isobj_handler コール時に引き渡されるポインタ
 * @retval 0 成功 within_intr_handler_func
 * @retval EINVAL 引数が不正
 **/
int mbmcs_platform_stdintr_add_interrupt_service_functions(const uintptr_t offset, const uint32_t intr_bitmask, void( * const isr_func)(void *const), void (*isobj_func)(void *const ), void * const args_p, const bool exec_intr_proc)
{
    libmbmcs_status_t * const s = get_mbmcs_stat_p();
    intr_handler_table_t intr_tab;
    int result;

    DBMS5( stderr, "%s : execute" EOL_CRLF, __FUNCTION__);

    if( intr_bitmask == 0 ) {
	DBMS1( stderr, "%s : invalid bitmask" EOL_CRLF, __FUNCTION__);
	return EINVAL;
    }

    intr_tab.intrc_addr_offset = offset;
    intr_tab.intr_bitmask = intr_bitmask;
    intr_tab.isr_func = isr_func;
    intr_tab.isobj_func = isobj_func;
    intr_tab.args_p = args_p;

    IFDBG3THEN {
        char strbuf[MULTIOS_XTOA_HEX32_BUFSZ];
        multios_u32toahex( (s->stdintrc.base_addr + intr_tab.intrc_addr_offset), strbuf, MULTIOS_XTOA_HEX32_BUFSZ, NULL);
        DBMS3( stderr, "intrregs_addr=0x%08s" EOL_CRLF, strbuf);
        multios_u32toahex( intr_tab.intr_bitmask, strbuf, MULTIOS_XTOA_HEX32_BUFSZ, NULL);
        DBMS3( stderr, "intr_tab.intr_bitmask=0x%08s" EOL_CRLF, strbuf);
        multios_u32toahex( (uintptr_t)intr_tab.isr_func, strbuf, MULTIOS_XTOA_HEX32_BUFSZ, NULL);
        DBMS3( stderr, "intr_tab.isr_func=0x%08s" EOL_CRLF, strbuf);
        multios_u32toahex( (uintptr_t)intr_tab.isobj_func, strbuf, MULTIOS_XTOA_HEX32_BUFSZ, NULL);
        DBMS3( stderr, "intr_tab.iobj_func=0x%08s" EOL_CRLF, strbuf);
        multios_u32toahex( (uintptr_t)intr_tab.args_p, strbuf, MULTIOS_XTOA_HEX32_BUFSZ, NULL);
        DBMS3( stderr, "intr_tab.args_p=0x%08s" EOL_CRLF, strbuf);
    }

    if( exec_intr_proc == true) {
	mbmcs_platform_disable_interrupts();
    }

    result = multios_stl_list_push_back( &(s->intr_handler_queue), &intr_tab, sizeof(intr_tab));
    if( result ) {
	DBMS1( stderr, "%s : multios_stl_list_push_back(intr_tab) fail, stderror=%s" EOL_CRLF,
			__FUNCTION__, strerror(result));
	return result;
    }

    if( exec_intr_proc == true) {
	mbmcs_platform_enable_interrupts();
    }

    return 0;
}

/**
 * @fn void mbmcs_stdintr_exec_isobj_funcs(void)
 * @brief 割り込み時に積まれた関数の処理を行います。
 *	リストオブジェクトの最後に、stdintrモジュールの割込みをイネーブルにするための処理が入っており
 *	実行されます。
 **/
void mbmcs_platform_stdintr_exec_isobj_funcs(void)
{
    libmbmcs_status_t * const s = get_mbmcs_stat_p();

    mbmcs_platform_disable_interrupts();
    // XIOModule_Disable(&s->iomodule, XIN_IOMODULE_EXTERNAL_INTERRUPT_INTR);
    // if(s->init.f.intr_fit1) {
    // XIOModule_Disable(&s->iomodule,XIN_IOMODULE_FIT_1_INTERRUPT_INTR);
    // }

    if(1) {
	size_t n;
	int result;
	const size_t num_funcs = multios_stl_list_get_pool_cnt(&s->isobj_handler_queue);

	if(!num_funcs) {
	    DBMS5( stderr, "%s : called but num_handler is %d" EOL_CRLF, __FUNCTION__,num_funcs);
	    // mbmcs_stdintrc_enable_output( &s->stdintrc, 0x0);
	    //    mbmcs_platform_stdintr_enable_intr_signal_output(0x0);
	    //    if(s->init.f.intr_fit1) {
	    //	 	XIOModule_Enable(&s->iomodule,XIN_IOMODULE_FIT_1_INTERRUPT_INTR);
	    //    }
	    //    XIOModule_Enable(&s->iomodule, XIN_IOMODULE_EXTERNAL_INTERRUPT_INTR);
	    mbmcs_platform_enable_interrupts();
	    return;
	}
	// xil_printf("num_funcs=%d" EOL_CRLF, num_funcs);
	for(n=0; n<num_funcs; ++n) {
	    isobj_handler_table_t tab;

	    result =  multios_stl_list_get_element_at( &s->isobj_handler_queue, 0, &tab, sizeof(tab));
	    if(result) {
		DBMS1( stderr,"%s :  multios_stl_list_get_element_at(%d) fail:%s" EOL_CRLF,
			__FUNCTION__, n, strerror(result));
		    abort();
	    }
	    result = multios_stl_list_pop_front( &s->isobj_handler_queue);
	    if(result) {
		DBMS1( stderr,"%s :  multios_stl_list_pop_front(%d) fail:%s" EOL_CRLF,
				__FUNCTION__, n, strerror(result));
		abort();
	    }

	    if( NULL != tab.isobj_func ) {
		tab.isobj_func(tab.args_p);
		    // check_interrput_mismatch("mbmcs_platform_stdintr_exec_isobj_funcs callback proc");
	    }
	}
//	if(s->init.f.intr_fit1) {
//	    XIOModule_Enable(&s->iomodule,XIN_IOMODULE_FIT_1_INTERRUPT_INTR);
//	}
	mbmcs_platform_stdintr_enable_intr_signal_output(0x0);
    }
//    XIOModule_Enable(&s->iomodule, XIN_IOMODULE_EXTERNAL_INTERRUPT_INTR);
    mbmcs_platform_enable_interrupts();

    return;
}

void mbmcs_platform_interval_timer_service_exec_funcs(void)
{
    libmbmcs_status_t * const s = get_mbmcs_stat_p();

    // DMSG( stderr, "%s : execute" EOL_CRLF, __FUNCTION__);
    // タイマーでキューイングされている関数の終了までタイマーの割り込み禁止 */

    mbmcs_platform_disable_interrupts();
//    XIOModule_Disable(&s->iomodule,XIN_IOMODULE_FIT_1_INTERRUPT_INTR);
//    if(s->init.f.stdintrc) {
//	XIOModule_Disable(&s->iomodule, XIN_IOMODULE_EXTERNAL_INTERRUPT_INTR);
//    }
    if(1) {
	size_t n;
	int result;
	const size_t num_funcs = multios_stl_list_get_pool_cnt(&s->itobj_handler_queue);
	if(!num_funcs ) {
	    DBMS5( stderr, "%s : called but num_handler is %d" EOL_CRLF, __FUNCTION__,num_funcs);
//	    if(s->init.f.stdintrc) {
//	        XIOModule_Enable(&s->iomodule, XIN_IOMODULE_EXTERNAL_INTERRUPT_INTR);
//	    }
//	    XIOModule_Enable(&s->iomodule,XIN_IOMODULE_FIT_1_INTERRUPT_INTR);
	    mbmcs_platform_enable_interrupts();
	    return;
	}

	for(n=0; n<num_funcs; ++n) {
	    isobj_handler_table_t tab;

	    result =  multios_stl_list_get_element_at( &s->itobj_handler_queue, 0, &tab, sizeof(tab));
	    if(result) {
		DBMS1( stderr,"%s :  multios_stl_list_get_element_at(%d) fail:%s" EOL_CRLF,
				__FUNCTION__, n, strerror(result));
		abort();
	    }
	    result = multios_stl_list_pop_front( &s->itobj_handler_queue);
	    if(result) {
		DBMS1( stderr,"%s :  multios_stl_list_pop_front(%d) fail:%s" EOL_CRLF,
				__FUNCTION__, n, strerror(result));
		abort();
	    }

	    if( NULL != tab.isobj_func ) {
		tab.isobj_func(tab.args_p);
		    // check_interrput_mismatch("mbmcs_platform_interval_timer_service_exec_funcs callback proc");
	    }
	}
    }
//    if(s->init.f.stdintrc) {
//        XIOModule_Enable(&s->iomodule, XIN_IOMODULE_EXTERNAL_INTERRUPT_INTR);
//    }
//    XIOModule_Enable(&s->iomodule,XIN_IOMODULE_FIT_1_INTERRUPT_INTR);
    mbmcs_platform_enable_interrupts();

    return;
}

/**
 * @fn int mbmcs_platform_stdintr_enable_intr_signal_output(const uintptr_t stdintrc_addr_offset)
 * @brief 複数のアドレスにマップされた割り込みモジュールを手動で上位層へ割り込みリクエスト出力を許可します
 * @param stdintrc_addr_offset マップしたベースアドレスからのオフセット
 * @retval 0 成功
 * @retval EINVAL 引数が不正
 **/
int mbmcs_platform_stdintr_enable_intr_signal_output(const uintptr_t stdintrc_addr_offset)
{
    libmbmcs_status_t * const s = get_mbmcs_stat_p();

    return mbmcs_stdintrc_enable_output( &s->stdintrc, stdintrc_addr_offset );
}

/**
 * @fn int mbmcs_platform_stdintr_disable_intr_signal_output(const uintptr_t stdintrc_addr_offset)
 * @brief 複数のアドレスにマップされた割り込みモジュールを手動で上位層へ割り込みリクエスト出力を禁止します
 * @param stdintrc_addr_offset マップしたベースアドレスからのオフセット
 * @retval 0 成功
 * @retval EINVAL 引数が不正
 **/
int mbmcs_platform_stdintr_disable_intr_signal_output(const uintptr_t stdintrc_addr_offset)

{
    libmbmcs_status_t * const s = get_mbmcs_stat_p();

    return mbmcs_stdintrc_disable_output( &s->stdintrc, stdintrc_addr_offset );
}

int mbmcs_platform_interval_timer_service_init(void)
{
#if defined(MBMCS_ENABLE_FIT1_JIFFIES_COUNTER) && defined(MBMCS_ENABLE_SOFTWARE_INTERVAL_TIMER)
    int result;
    libmbmcs_status_t * const s = get_mbmcs_stat_p();

    DBMS3( stderr, "%s : execute" EOL_CRLF, __FUNCTION__);

    if(s->init.f.interval_timer_handler_queue) {
	DBMS1( stderr, "%s : already initalized" EOL_CRLF, __FUNCTION__);
	return EALREADY;
    }
    if(!s->init.f.virtual_heap) {
	DBMS1( stderr, "%s : virtual heap not initalize" EOL_CRLF, __FUNCTION__);
	return ENOMEM;
    }
    result = multios_stl_list_init( &s->interval_timer_handler_queue, sizeof(interval_timer_handler_t));
    if(result) {
	DBMS1(stderr, "%s : multios_stl_list_init(interval_timer_handler_queue) fail:%s" EOL_CRLF,
			__FUNCTION__, strerror(result));
	abort();
    }
    s->init.f.interval_timer_handler_queue = 0x1;

    result = multios_stl_list_init( &s->itobj_handler_queue, sizeof(isobj_handler_table_t));
    if(result) {
	DBMS1(stderr, " %s : multios_stl_list_init(itobj_handler_queue) fail:%s" EOL_CRLF,
			__FUNCTION__, strerror(result));
	abort();
    }
    s->init.f.itobj_handler_queue = 0x1;

    return 0;
#else /* end of MBMCS_ENABLE_SOFTWARE_INTERVAL_TIMER */
    DBMS3( stderr, "mbmcs_platfotm_software_interval_timer_init : fail" EOL_CRLF);
    return EACCES;
#endif
}

int mbmcs_platform_interval_timer_add_service_function(const uint32_t interval_msec, void(* const cb_func)(void *const), void * const args_p, const bool exec_intr_proc)
{
#if defined(MBMCS_ENABLE_FIT1_JIFFIES_COUNTER) && defined(MBMCS_ENABLE_SOFTWARE_INTERVAL_TIMER)
   libmbmcs_status_t * const s = get_mbmcs_stat_p();
   interval_timer_handler_t ith_tab;
   int result;

    DBMS5( stderr, "%s : execute" EOL_CRLF, __FUNCTION__);

    IFDBG3THEN {
	char d64_strbuf[MULTIOS_XTOA_DEC64_BUFSZ];
	DMSG( stderr, "interval_time_msec=%s" EOL_CRLF,
	    multios_u64toadec((uint64_t)interval_msec, d64_strbuf, MULTIOS_XTOA_DEC64_BUFSZ));
    }

    if( (interval_msec == 0) || (cb_func == NULL) ) {
	return EINVAL;
    }

    ith_tab.interval_time_msec = interval_msec;
    ith_tab.timer_counter_msec = 0;
    ith_tab.timer_cb_func = cb_func;
    ith_tab.args_p = args_p;

    IFDBG3THEN {
	char strbuf[MULTIOS_XTOA_HEX32_BUFSZ];
	char d64_strbuf[MULTIOS_XTOA_DEC64_BUFSZ];

	DMSG( stderr, "ith_tab.interval_time_msec=%s" EOL_CRLF,
			multios_u64toadec((uint64_t)ith_tab.interval_time_msec, d64_strbuf, MULTIOS_XTOA_DEC64_BUFSZ));
	DMSG( stderr, "ith_tab.timer_counter_msec=%s" EOL_CRLF, 
			multios_u64toadec((uint64_t)ith_tab.timer_counter_msec, d64_strbuf, MULTIOS_XTOA_DEC64_BUFSZ));

	multios_u32toahex( (uintptr_t)ith_tab.timer_cb_func, strbuf, MULTIOS_XTOA_HEX32_BUFSZ, NULL);
	DMSG( stderr, "ith_tab.timer_cb_func=0x%08s" EOL_CRLF, strbuf);
	multios_u32toahex( (uintptr_t)ith_tab.args_p, strbuf, MULTIOS_XTOA_HEX32_BUFSZ, NULL);
	DMSG( stderr, "ith_tab.args_p=0x%08s" EOL_CRLF, strbuf);
    }

    if(exec_intr_proc == true) {
	mbmcs_platform_disable_interrupts();
    }

    result = multios_stl_list_push_back( &(s->interval_timer_handler_queue), &ith_tab, sizeof(ith_tab));
    if( result ) {
	DBMS1( stderr, "%s : multios_stl_list_push_back(intr_tab) fail, stderror=%s\n",
			__FUNCTION__, strerror(result));
	return result;
    }
  
    if(exec_intr_proc == true) {
	mbmcs_platform_enable_interrupts();
    }

    return 0;
#else
    DBMS5( stderr, "%s : fail" EOL_CRLF, __FUNCTION__);
    return ENOTSUP;
#endif
}

/**
 * @fn void mbmcs_platform_disable_interrupts(void)
 * @brief Microblaze CPUへの割り込みを保留します。
 */ 
void mbmcs_platform_disable_interrupts(void)
{
    libmbmcs_status_t * const s = get_mbmcs_stat_p();
    ++s->num_intr_locks;

    microblaze_disable_interrupts();
    return;
}

/**
 * @fn void microblaze_enable_interrupts(void)
 * @brief Microblaze CPUへの割り込みを許可します。
 */ 
void mbmcs_platform_enable_interrupts(void)
{
    libmbmcs_status_t * const s = get_mbmcs_stat_p();
    --s->num_intr_locks;

    microblaze_enable_interrupts();

    return;
}

#define ADDR32TOREG(a) (*(volatile uint32_t*)((intptr_t)(a)))

uint32_t mbmcs_platform_read_register32(const uintptr_t addr)
{
    return ADDR32TOREG(addr);
}

void mbmcs_platform_write_register32( const uintptr_t addr, const uint32_t data)
{
    ADDR32TOREG(addr) = data;
}
