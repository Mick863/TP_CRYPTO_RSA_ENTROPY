// fake_avr.c
#include "fake_avr.h"

uint8_t ADMUX = 0, ADCSRA = 0, PORTB = 0, DDRB = 0, DDRC = 0, DDRD = 0;
uint8_t PORTC = 0, PORTD = 0, PIND = 0, PINB = 0, PINC = 0;
uint8_t PRR = 0, ACSR = 0, ASSR = 0, OSCCAL = 0x5A;
uint8_t TCCR0A = 0, TCCR0B = 0, TIMSK0 = 0, TCNT0 = 0;
uint8_t TCCR2B = 0, TCNT2 = 0;
uint16_t ADC = 512;

// experiment.c
#ifndef F_CPU 
#define F_CPU 16000000UL
#endif

#include <inttypes.h>
#ifdef __AVR__
#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <avr/boot.h>
#include <util/delay.h>
#else
#include "fake_avr.h"
#endif