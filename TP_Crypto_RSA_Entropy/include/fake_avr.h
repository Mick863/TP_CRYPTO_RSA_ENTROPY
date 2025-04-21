#ifndef FAKE_AVR_H
#define FAKE_AVR_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define F_CPU 16000000UL
#define EEMEM
#define PROGMEM

// Simulate EEPROM
static inline uint8_t eeprom_read_byte(const uint8_t* addr) { return 0x42; }
static inline void eeprom_write_byte(uint8_t* addr, uint8_t val) { (void)addr; (void)val; }
static inline void eeprom_read_block(void* dest, const void* src, size_t n) { memset(dest, 0x42, n); }
static inline void eeprom_write_block(const void* src, void* dest, size_t n) { (void)src; (void)dest; (void)n; }

// Simulate delays
static inline void _delay_ms(double ms) { printf("[FAKE] _delay_ms(%.2f ms)\n", ms); }
static inline void _delay_us(double us) { printf("[FAKE] _delay_us(%.2f us)\n", us); }

// Simulate sei/cli
static inline void sei(void) { printf("[FAKE] sei() called\n"); }
static inline void cli(void) { printf("[FAKE] cli() called\n"); }

// Define ISR macro
#define ISR(vector) void vector(void)

// Simulate AVR registers as global variables
extern uint8_t ADMUX, ADCSRA, PORTB, DDRB, DDRC, DDRD;
extern uint8_t PORTC, PORTD, PIND, PINB, PINC;
extern uint8_t PRR, ACSR, ASSR, MCUSR, OSCCAL;
extern uint8_t TCCR0A, TCCR0B, TIMSK0, TCNT0, TCCR2B, TCNT2;
extern uint16_t ADC;


// Simulate register bits
#define REFS0 0
#define REFS1 1
#define ADEN  2
#define ADSC  3
#define ADPS0 4
#define ADPS1 5
#define ADPS2 6
#define MUX0  7
#define MUX1  8
#define MUX2  9
#define MUX3 10
#define PD3   3
#define PD4   4
#define PD5   5
#define PD6   6
#define PD7   7
#define PRADC 0
#define PC6   6
#define EXCLK 6
#define AS2   5

// Simulate boot functions
static inline uint8_t boot_lock_fuse_bits_get(int x) { return (uint8_t)(x & 0xFF); }
static inline uint8_t boot_signature_byte_get(uint16_t addr) { return (uint8_t)(addr & 0xFF); }

#endif // FAKE_AVR_H
