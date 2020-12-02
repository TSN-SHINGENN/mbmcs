#ifndef INC_LIBMBMCS_PLATFORM_H
#define INC_LIBMBMCS_PLATFORM_H

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* microblaze mcs */
#include <xiomodule.h>

/* FIT タイマ割り込みと同値に設定してください。HZ:1/1000=1msec*/
/* HZは通常1msにで使用してください。 */
#define HZ 1000

#define MBMCS_ENABLE_FIT1_JIFFIES_COUNTER 
#define MBMCS_ENABLE_HARDWARE_JIFFIES_COUNTER
#define MBMCS_ENABLE_SOFTWARE_INTERVAL_TIMER
#define MBMCS_ENABLE_EXTERNAL_INTERRUPT_WITH_MBMCS_STDINTR

extern XIOModule * const mbmcs_iomodule_p;

#ifdef __cplusplus
extern "C" {
#endif

int mbmcs_platform_init(void *const vertual_heap_buf, const size_t bufsz);
int mbmcs_platform_monotonic_hardware_timer_init(const uintptr_t base_addr);

void mbmcs_platform_interrupt_post_process(void (* const itobj_exec_func)(void),void (* const iobj_exec_func)(void) );
int mbmcs_platform_uart_gets( char * const recvbuf, const size_t recvbufsz, void (*const fit1_exec_func)(void),void (* const isobj_exec_func)(void) );
void mbmcs_platform_uart_set_echoback_to_on(const int is_true);
int mbmcs_platform_uart_get_echoback_is_on(void);

int mbmcs_platform_stdintr_init(const uintptr_t base_addr);
int mbmcs_platform_stdintr_add_interrupt_service_functions(const uintptr_t offset, const uint32_t intr_bitmask, void( * const isr_func)(void *const), void (*isobj_func)(void *const ), void * const args_p, const bool exec_intr_proc);

void mbmcs_platform_stdintr_exec_isobj_funcs(void);

int mbmcs_platform_stdintr_enable_intr_signal_output(const uintptr_t stdintrc_addr_offset);
int mbmcs_platform_stdintr_disable_intr_signal_output(const uintptr_t stdintrc_addr_offset);

int mbmcs_platform_interval_timer_service_init(void);

int mbmcs_platform_interval_timer_add_service_function(const uint32_t interval_msec, void(* const cb_func)(void *const), void * const args_p, const bool exec_intr_proc);

void mbmcs_platform_interval_timer_service_exec_funcs(void);

void mbmcs_platform_disable_interrupts(void);
void mbmcs_platform_enable_interrupts(void);

uint32_t mbmcs_platform_read_register32(const uintptr_t addr);
void mbmcs_platform_write_register32( const uintptr_t addr, const uint32_t data);

#ifdef __cplusplus
}
#endif

#ifdef MBMCS_ENABLE_FIT1_JIFFIES_COUNTER 

extern const volatile  uint64_t* const  mbmcs_jiffies_p;

#ifdef __cplusplus
extern "C" {
#endif

void mbmcs_msleep( const unsigned int seconds );
unsigned int  mbmcs_sleep( const unsigned int milisecs );

#ifdef __cplusplus
}
#endif

#endif /* end of MBMCS_ENABLE_FIT1_JIFFIES_COUNTER */

#endif /* end of INC_LIBMBMCS_PLATFORM_H */
