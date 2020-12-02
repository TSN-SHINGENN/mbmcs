#ifndef INC_MBMCS_ATMEL_93C_SPI_H
#define INC_MBMCS_ATMEL_93C_SPI_H

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef union {
    uint8_t flags;
    struct {
	uint8_t vcc_is_over_4p5:1;
	uint8_t use_interrupt:1;
	uint8_t datamode_is_x16:1;
    } f;
} mbmcs_atmel_93c_api_attr_t;

typedef enum enum_mbmcs_atmel_93c_series {
    ATMEL_93C46 = 30,
    ATMEL_93C56,
    ATMEL_93C66,
    ATMEL_93C86,
    ATMEL_93C_EOD = 0,
} enum_mbmcs_atmel_93c_series_t;

typedef struct {
    enum_mbmcs_atmel_93c_series_t model;
    uint8_t x8_addr_depth;
    uint8_t x16_addr_depth;
} mbmcs_atmel_93c_bit_depth_list_t;

static const mbmcs_atmel_93c_bit_depth_list_t mbmcs_atmel_93c_bit_depth_list[]= {
/*      MODEL, x8 BitDepth, x16 BitDepth */
{ ATMEL_93C46  ,         7,            6}, /*  1k-bit (128x8 or 64x16)  */
{ ATMEL_93C56  ,         9,            8}, /*  2k-bit (256x8 or 128x16) */
{ ATMEL_93C66  ,         9,            8}, /*  4k-bit (512x8 or 256x16) */
{ ATMEL_93C86  ,        11,           10}, /* 16k-bit (2kix8 or 1kix16) */
{ ATMEL_93C_EOD,         0,            0}};

typedef struct {
    intptr_t base_addr;
    mbmcs_atmel_93c_api_attr_t attr;
    enum_mbmcs_atmel_93c_series_t model;
} mbmcs_atmel_93c_spi_t;

int mbmcs_atmel_93c_init( mbmcs_atmel_93c_spi_t * const self_p, const intptr_t base_addr, const enum_mbmcs_atmel_93c_series_t model);
int mbmcs_atmel_93c_set_vcc_attribute( mbmcs_atmel_93c_spi_t * const self_p, uint8_t vcc_is_exceed_4p5);

int mbmcs_atmel_93c_destroy( mbmcs_atmel_93c_spi_t * const self_p);

int mbmcs_atmel_93c_readx8( const mbmcs_atmel_93c_spi_t * const self_p, const uint16_t addr, uint8_t *const d8_p);
int mbmcs_atmel_93c_writex8( const mbmcs_atmel_93c_spi_t * const self_p, const uint16_t addr, const uint8_t d8);

int mbmcs_atmel_93c_erasex8( const mbmcs_atmel_93c_spi_t * const self_p, const uint16_t addr);

int mbmcs_atmel_93c_write_enable( const mbmcs_atmel_93c_spi_t * const self_p);
int mbmcs_atmel_93c_write_disable( const mbmcs_atmel_93c_spi_t * const self_p);

#ifdef __cplusplus
}
#endif

#endif /* end of INC_MBMCS_ATMEL_93C_SPI_H */
