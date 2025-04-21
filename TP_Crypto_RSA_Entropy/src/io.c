// fichier "io.c"
// lecture et écriture d'octets en t=0
// uint8_t recbytet0();      // reçoit un octet t=0
// void sendbytet0(uint8_t); // émet un octet t=0

#include <inttypes.h>
#include "compact_avr.h"

#define IOPIN 4

// envoi d’un bit sur le lien série
static void sendbit(uint8_t b) {
    uint8_t outB = 0;
    if ((b & 1) != 0) outB = (1 << IOPIN);
    while (TCNT2 <= 45) {} // attendre fin envoi précédent
    TCNT2 = 0;
    PORTB = outB;
}

// envoi d’un octet t=0
void sendbytet0(uint8_t b) {
    uint8_t i, p, tccr2b_save;

    tccr2b_save = TCCR2B;
    TCCR2B = 2;
    TCNT2 = 40;

reenvoyer:
    DDRB |= (1 << IOPIN);
    PORTB |= (1 << IOPIN);
    sendbit(0); // bit start
    p = 0;

    for (i = 0; i < 8; i++) {
        sendbit(b);
        p ^= b;
        b >>= 1;
    }

    sendbit(p); // bit parité
    sendbit(1); // bit stop

    while (TCNT2 <= 45) {}

    DDRB &= ~(1 << IOPIN);
    PORTB &= ~(1 << IOPIN);

    while (TCNT2 <= 91) {}

    if ((PINB & (1 << IOPIN)) == 0) {
        while ((PINB & (1 << IOPIN)) == 0) {}
        TCNT2 = 22;
        goto reenvoyer;
    }

    TCCR2B = tccr2b_save;
}

// lecture d’un bit
static uint8_t getbit() {
    uint8_t b;
    while (TCNT2 <= 45) {}
    TCNT2 = 0;

    while (TCNT2 < 15) {}
    b = (PINB & (1 << IOPIN));
    while (TCNT2 <= 23) {}
    b += (PINB & (1 << IOPIN));
    while (TCNT2 <= 30) {}
    b += (PINB & (1 << IOPIN));

    return (b >> 5) & 1;
}

// lecture d’un octet t=0
uint8_t recbytet0() {
    uint8_t i, r, p, b;

    TCCR2B = 2;
relire:
    DDRB &= ~(1 << IOPIN);
    PORTB |= (1 << IOPIN);

    r = 0;

    while (PINB & (1 << IOPIN)) {}
    while (PINB & (1 << IOPIN)) {}

    TCNT2 = 0;
    p = 0;

    for (i = 0; i < 8; i++) {
        b = getbit();
        r += (b << i);
        p += b;
    }

    p = (getbit() ^ p) & 1;

    while (TCNT2 <= 91) {}

    if (p) {
        DDRB |= (1 << IOPIN);
        PORTB &= ~(1 << IOPIN);
        while (TCNT2 <= 151) {}
        goto relire;
    } else {
        while (TCNT2 <= 137) {}
    }

    TCCR2B = 0;
    return r;
}
