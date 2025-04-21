#include "globals.h"
#include "experiment_helpers.h"
#include <stdint.h>
#include <io.h>
#include <stdio.h>

void read_f8() {
    y_y = MCUSR;
    MCUSR = 0;
}

void f2() { sendbytet0(ins); sendbytet0(0xA2); sw1 = 0x90; sw2 = 0x00; }
void f3() { sendbytet0(ins); sendbytet0(0xB3); sw1 = 0x90; sw2 = 0x00; }
void f4() { sendbytet0(ins); sendbytet0(0xC4); sw1 = 0x90; sw2 = 0x00; }
void sable_f5_mc() { sendbytet0(ins); sendbytet0(0x55); sw1 = 0x90; sw2 = 0x00; }
void sable_f6() { sendbytet0(ins); sendbytet0(0x66); sw1 = 0x90; sw2 = 0x00; }
void sable_f7s() { sendbytet0(ins); sendbytet0(0x77); sw1 = 0x90; sw2 = 0x00; }
void sable_f8() { sendbytet0(ins); sendbytet0(0x88); sw1 = 0x90; sw2 = 0x00; }
void sable_f9() { sendbytet0(ins); sendbytet0(0x99); sw1 = 0x90; sw2 = 0x00; }
void sable_fa() { sendbytet0(ins); sendbytet0(0xAA); sw1 = 0x90; sw2 = 0x00; }
void sable_f5_fb() { sendbytet0(ins); sendbytet0(0xFB); sw1 = 0x90; sw2 = 0x00; }
void sable_fc_on() { sendbytet0(ins); sendbytet0(0xCC); sw1 = 0x90; sw2 = 0x00; }
void f4_fe() { sendbytet0(ins); sendbytet0(0xDE); sw1 = 0x90; sw2 = 0x00; }
void sable_f5_fb_f0() { sendbytet0(ins); sendbytet0(0xF0); sw1 = 0x90; sw2 = 0x00; }
void sable_f5_fb_f1() { sendbytet0(ins); sendbytet0(0xF1); sw1 = 0x90; sw2 = 0x00; }

void f4_d0PD3() { sendbytet0(ins); sendbytet0(0x03); sw1 = 0x90; sw2 = 0x00; }
void f4_d0PD4() { sendbytet0(ins); sendbytet0(0x04); sw1 = 0x90; sw2 = 0x00; }
void f4_d0PD5() { sendbytet0(ins); sendbytet0(0x05); sw1 = 0x90; sw2 = 0x00; }
void f4_d0PD6() { sendbytet0(ins); sendbytet0(0x06); sw1 = 0x90; sw2 = 0x00; }
void f4_d0PD7() { sendbytet0(ins); sendbytet0(0x07); sw1 = 0x90; sw2 = 0x00; }

void o7r4ll() { sendbytet0(ins); sendbytet0(0xB7); sw1 = 0x90; sw2 = 0x00; }
void n1lt()   { sendbytet0(ins); sendbytet0(0xB8); sw1 = 0x90; sw2 = 0x00; }
void o6n1()   { sendbytet0(ins); sendbytet0(0xB9); sw1 = 0x90; sw2 = 0x00; }

int main(void) {
    read_f8();
    atr(11, "experiment!");

    sw2 = 0;
    for (;;) {
        cla = recbytet0();
        ins = recbytet0();
        p1 = recbytet0();
        p2 = recbytet0();
        if ((cla != 0x81) || (ins != 0xff)) {
            p3 = recbytet0();
        }
        sw2 = 0;

        switch (cla) {
            case 0x81:
                switch (ins) {
                    case 0xf2: f2(); break;
                    case 0xf3: f3(); break;
                    case 0xf4: f4(); break;
                    case 0xf5: sable_f5_mc(); break;
                    case 0xf6: sable_f6(); break;
                    case 0xf7: sable_f7s(); break;
                    case 0xf8: sable_f8(); break;
                    case 0xf9: sable_f9(); break;
                    case 0xfa: sable_fa(); break;
                    case 0xfb: sable_f5_fb(); break;
                    case 0xfc: sable_fc_on(); break;
                    case 0xfe: f4_fe(); break;
                    case 0xf0: sable_f5_fb_f0(); break;
                    case 0xf1: sable_f5_fb_f1(); break;
                    case 0xd0: f4_d0PD3(); break;
                    case 0xd1: f4_d0PD4(); break;
                    case 0xd2: f4_d0PD5(); break;
                    case 0xd3: f4_d0PD6(); break;
                    case 0xd4: f4_d0PD7(); break;
                    case 0xB7: o7r4ll(); break;
                    case 0xB8: n1lt(); break;
                    case 0xB9: o6n1(); break;
                    default: sw1 = 0x6d;
                }
                break;
            default:
                sw1 = 0x6e;
        }
        sendbytet0(sw1);
        sendbytet0(sw2);
    }
    return 0;
}
