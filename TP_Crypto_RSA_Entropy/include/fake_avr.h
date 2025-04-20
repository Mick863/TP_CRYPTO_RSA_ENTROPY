#ifndef AVR_FAKE_H
#define AVR_FAKE_H

#include <stdint.h>
#include <stdio.h>

#define EEMEM
#define F_CPU 16000000UL

// Simule EEPROM
static inline unsigned char eeprom_read_byte(const unsigned char* addr) {
    return 0x42;
}

static inline void eeprom_write_byte(unsigned char* addr, unsigned char val) {
    (void)addr;
    (void)val;
}

// Simule pgmspace.h
#define PROGMEM
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#define pgm_read_word(addr) (*(const uint16_t *)(addr))
#define pgm_read_dword(addr) (*(const uint32_t *)(addr))
#define memcpy_P(dest, src, len) memcpy((dest), (src), (len))

#endif
