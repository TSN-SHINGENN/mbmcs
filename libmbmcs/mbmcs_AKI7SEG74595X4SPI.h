#ifndef INC_MBMCS_AKI7SEG74595X4SPI_H
#define INC_MBMCS_AKI7SEG74595X4SPI_H

#include <stdint.h>

typedef struct {
      uintptr_t base_addr;
} mbmcs_aki7seg74595x4spi_t;

#if defined(__cplusplus)
extern "C" {
#endif

int  mbmcs_aki7seg74595x4spi_init(mbmcs_aki7seg74595x4spi_t *const self_p, const uintptr_t base_addr);
void mbmcs_aki7seg74595x4spi_write_hex_digit(const mbmcs_aki7seg74595x4spi_t *const self_p, const uint16_t data16);
void mbmcs_aki7seg74595x4spi_write_dot(const mbmcs_aki7seg74595x4spi_t *const self_p, const uint8_t dots_bit);
int  mbmcs_aki7seg74595x4spi_test(const mbmcs_aki7seg74595x4spi_t *const self_p, const uint8_t is_on);
int  mbmcs_aki7seg74595x4spi_on(const mbmcs_aki7seg74595x4spi_t *const self_p, const uint8_t is_on);
void mbmcs_aki7seg74595x4spi_ascii_mode(const mbmcs_aki7seg74595x4spi_t *const self_p, const uint8_t is_on);
int  mbmcs_aki7seg74595x4spi_write_ascii_digit(const mbmcs_aki7seg74595x4spi_t *const self_p, const char * const str);
void mbmcs_aki7seg74595x4spi_send_to_module(const mbmcs_aki7seg74595x4spi_t *const self_p);
void mbmcs_aki7seg74595x4spi_force_send_sync_timing_is_on(const mbmcs_aki7seg74595x4spi_t *const self_p, uint8_t is_on);
void mbmcs_aki7seg74595x4spi_brightness(const mbmcs_aki7seg74595x4spi_t *const self_p, const uint8_t val);
int  mbmcs_aki7seg74595x4spi_destroy(mbmcs_aki7seg74595x4spi_t *const self_p);

#if defined(__cplusplus)
}
#endif

#endif /* end of INC_MBMCS_AKI7SEG74595X4SPI_H */
