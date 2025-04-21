#ifndef COMPAT_AVR_H
#define COMPAT_AVR_H

#ifdef __AVR__
#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <avr/boot.h>
#include <util/delay.h>
#else
#include "fake_avr.h"
#endif

#endif // COMPAT_AVR_H
