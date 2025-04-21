#include <io.h>
#include <inttypes.h>

#ifdef __AVR__
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#else
#include "fake_avr.h"
#endif

#include "rsa-crt.h"
#include "globals.h"
#include "experiment_helpers.h"

void sendbytet0(uint8_t b);
uint8_t recbytet0(void);

void atr(uint8_t n, char* hist) {
    sendbytet0(0x3b);
    sendbytet0(n);
    while (n--) {
        sendbytet0(*hist++);
    }
}

void version(int t, char* sv) {
    if (p3 != t) {
        sw1 = 0x6c;
        sw2 = t;
        return;
    }
    sendbytet0(ins);
    for (int i = 0; i < p3; i++) {
        sendbytet0(sv[i]);
    }
    sw1 = 0x90;
}


#define TAILLE_PQ 33
#define TAILLE_N 65
#define TAILLE_E 4

uint8_t ee_e[TAILLE_E] EEMEM = {0};
uint8_t ee_se EEMEM = 0;
uint8_t ee_p[TAILLE_PQ] EEMEM = {0};
uint8_t ee_sp EEMEM = 0;
uint8_t ee_q[TAILLE_PQ] EEMEM = {0};
uint8_t ee_sq EEMEM = 0;
uint8_t ee_n[TAILLE_N] EEMEM;
uint8_t ee_sn EEMEM = 0;
uint8_t ee_dp[TAILLE_PQ] EEMEM;
uint8_t ee_sdp EEMEM = 0;
uint8_t ee_dq[TAILLE_PQ] EEMEM;
uint8_t ee_sdq EEMEM = 0;
uint8_t ee_u[TAILLE_PQ] EEMEM;
uint8_t ee_su EEMEM = 0;

void intro_cle(uint8_t* dest, uint8_t* ps, int smax) {
    uint8_t x[TAILLE_N];
    if ((p3 > smax) || (p3 == 0)) {
        sw1 = 0x6c;
        sw2 = smax;
        return;
    }
    sendbytet0(ins);
    for (int i = p3 - 1; i >= 0; i--) {
        x[i] = recbytet0();
    }
    eeprom_write_block(x, dest, p3);
    eeprom_write_byte(ps, p3);
    sw1 = 0x90;
}

void precalculs() {
    int se = eeprom_read_byte(&ee_se);
    int sp = eeprom_read_byte(&ee_sp);
    int sq = eeprom_read_byte(&ee_sq);
    if ((se == 0) || (sp == 0) || (sq == 0)) return;

    uint8_t e[TAILLE_E], n[TAILLE_N], p[TAILLE_PQ], q[TAILLE_PQ], x[TAILLE_PQ];

    eeprom_read_block(e, ee_e, se);
    eeprom_read_block(p, ee_p, sp);
    eeprom_read_block(q, ee_q, sq);

    if (Compare(sp, p, sq, q) >= 0) {
        eeprom_write_byte(&ee_sp, sq);
        eeprom_write_block(q, &ee_p, sq);
        eeprom_write_byte(&ee_sq, sp);
        eeprom_write_block(p, &ee_q, sp);
        int sx = LCopy(x, sp, p);
        sp = LCopy(p, sq, q);
        sq = LCopy(q, sx, x);
        sw2++;
    }

    int sn = LLMul(n, sp, p, sq, q);
    eeprom_write_byte(&ee_sn, sn);
    eeprom_write_block(n, ee_n, sn);

    int sx = ll_inv_mod(x, sp, p, sq, q);
    eeprom_write_byte(&ee_su, sx);
    eeprom_write_block(x, ee_u, sx);

    p[0]--;
    sx = ll_inv_mod(x, se, e, sp, p);
    p[0]++;
    eeprom_write_byte(&ee_sdp, sx);
    eeprom_write_block(x, ee_dp, sx);

    q[0]--;
    sx = ll_inv_mod(x, se, e, sq, q);
    q[0]++;
    eeprom_write_byte(&ee_sdq, sx);
    eeprom_write_block(x, ee_dq, sx);
}

void lire_cle(uint8_t* k, uint8_t* sk) {
    int sn = eeprom_read_byte(sk);
    if ((sn == 0) && (p3 != 1)) {
        sw1 = 0x6c;
        sw2 = 1;
        return;
    }
    if ((sn != 0) && (p3 != sn)) {
        sw1 = 0x6c;
        sw2 = sn;
        return;
    }
    sendbytet0(ins);
    for (int i = p3 - 1; i >= 0; i--) {
        sendbytet0(eeprom_read_byte(k + i));
    }
    sw1 = 0x90;
}

int srep;
uint8_t response[TAILLE_N];

void chiffre() {
    uint8_t x[TAILLE_N], n[TAILLE_N], e[TAILLE_E];

    int sn = eeprom_read_byte(&ee_sn);
    if (sn == 0) { sw1 = 0x62; sw2 = 0; return; }

    if (p3 > TAILLE_N) { sw1 = 0x6c; sw2 = TAILLE_N; return; }

    sendbytet0(ins);
    for (int i = p3 - 1; i >= 0; i--) x[i] = recbytet0();

    eeprom_read_block(n, ee_n, sn);
    int se = eeprom_read_byte(&ee_se);
    eeprom_read_block(e, ee_e, se);

    srep = LLExpMod(response, p3, x, se, e, sn, n);
    sw1 = 0x90;
}

void get_response() {
    if (srep == 0) { srep = 1; response[0] = 0; }

    if (p3 != srep) { sw1 = 0x6c; sw2 = srep; return; }

    sendbytet0(ins);
    for (int i = 0; i < srep; i++) sendbytet0(response[srep - i - 1]);
    sw1 = 0x90;
}

void dechiffre();  // déjà dans rsa-crt.c

/*
int main(void) {
    ACSR = 0x80;
    PORTB = 0xff;
    DDRB = 0xff;
    DDRC = 0xff;
    DDRD = 0;
    PORTC = 0xff;
    PORTD = 0xff;
    ASSR = (1 << EXCLK) + (1 << AS2);
    PRR = 0x87;

    atr(5, "rsa !");

    sw2 = 0;
    for (;;) {
        cla = recbytet0();
        ins = recbytet0();
        p1 = recbytet0();
        p2 = recbytet0();
        p3 = recbytet0();
        sw2 = 0;

        if (cla == 0x82) {
            switch (ins) {
                case 0: version(4, "1.00"); break;
                case 1: intro_cle(ee_e, &ee_se, TAILLE_E); break;
                case 2: intro_cle(ee_p, &ee_sp, TAILLE_PQ); break;
                case 3: intro_cle(ee_q, &ee_sq, TAILLE_PQ); precalculs(); break;
                case 4: lire_cle(ee_e, &ee_se); break;
                case 5: lire_cle(ee_p, &ee_sp); break;
                case 6: lire_cle(ee_q, &ee_sq); break;
                case 7: lire_cle(ee_n, &ee_sn); break;
                case 16: chiffre(); break;
                case 17: dechiffre(); break;
                case 0xc0: get_response(); break;
                default: sw1 = 0x6d;
            }
        } else {
            sw1 = 0x6e;
        }

        sendbytet0(sw1);
        sendbytet0(sw2);
    }

    return 0;
}

*/