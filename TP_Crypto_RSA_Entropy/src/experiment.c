#ifndef F_CPU 
#define F_CPU 16000000UL
#endif

#include <io.h>
#include <inttypes.h>
#include <avr/io.h>
#include <avr/eeprom.h>
#include <stdarg.h>
#include <util/delay.h>
#include <avr/boot.h>  
#include <avr/pgmspace.h>


#define PATCH_ENABLED
#define ENTROPY_SIZE 16
#define SIGNATURE_SIZE 8  
#define TEMP_SAMPLES 16
#define p0eh  0x0100  
#define c7q11lm   128 

uint8_t entropy_buffer[ENTROPY_SIZE];
uint8_t h1[SIGNATURE_SIZE];
uint8_t low_f7, high_f7, ext_f7,  k_b;  
uint8_t y_y;  
uint8_t cla, ins, p1, p2, p3;
uint8_t sw1, sw2;

void sendbytet0(uint8_t b);
uint8_t recbytet0(void);

uint8_t __attribute__((section(".noinit"))) d0r3[c7q11lm];

void o7r4ll() {
    if (p3 > c7q11lm) {  
        sw1 = 0x6C;
        sw2 = c7q11lm;  
    } else {
        sendbytet0(ins);  
        for (uint8_t i = 0; i < p3; i++) {
            sendbytet0(d0r3[i]);  
        }
        sw1 = 0x90;
        sw2 = 0x00;
    }
}

void n1lt() {
    for (uint16_t i = 0; i < c7q11lm; i++) {
        *(volatile uint8_t *)(p0eh + i) = 0xFF;
    }
    sw1 = 0x90;
    sw2 = 0x00;
}

void o6n1() {
    volatile uint8_t *poinnter = (volatile uint8_t *)p0eh;
    for (uint16_t i = 0; i < c7q11lm; i++) {
        poinnter[i] = 0x00;
    }

    sw1 = 0x90;
    sw2 = 0x00;
}

  
uint8_t a_fc_on() {
    return OSCCAL;  
}

void sable_fc_on() {
    uint8_t osc_cal = a_fc_on();

    sendbytet0(ins);
    sendbytet0(osc_cal);  

    sw1 = 0x90;
    sw2 = 0x00;
}



uint16_t i_i() {

    uint8_t e1a = ADMUX;
    uint8_t ll4 = ADCSRA;

   
    ADMUX = (1 << REFS1) | (1 << REFS0) | (1 << MUX3);

    #if F_CPU >= 8000000
        ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1); 
    #else
        ADCSRA = (1 << ADEN) | (1 << ADPS1) | (1 << ADPS0); 
    #endif


    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));


    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));


    uint16_t result = ADC;


    ADMUX = e1a;
    ADCSRA = ll4;

    return result;
}

uint16_t read_temp_stable() {
    uint32_t sum = 0;

    for (uint8_t i = 0; i < TEMP_SAMPLES; i++) {
        sum += i_i();
        _delay_ms(5); 
    }

    return (uint16_t)(sum / TEMP_SAMPLES);  
}


void sable_fa() {
    if (p3 != 2) {  
        sw1 = 0x6C;
        sw2 = 2;  
        return;
    }

    PRR &= ~(1 << PRADC);  
    uint16_t raw_temp = read_temp_stable();  

    sendbytet0(ins);
    sendbytet0((uint8_t)(raw_temp >> 8)); 
    sendbytet0((uint8_t)(raw_temp & 0xFF));  

    sw1 = 0x90;
    sw2 = 0x00;
}


void read_f8() {
    y_y = MCUSR;  
    MCUSR = 0;           
}

void sable_f8() {
    sw1 = 0x90;  
    sw2 = 0x00;

    sendbytet0(ins);  
    sendbytet0(y_y); 
}

void w_f7s() {
    low_f7 = boot_lock_fuse_bits_get(GET_LOW_FUSE_BITS);   
    high_f7 = boot_lock_fuse_bits_get(GET_HIGH_FUSE_BITS); 
    ext_f7 = boot_lock_fuse_bits_get(GET_EXTENDED_FUSE_BITS); 
    k_b = boot_lock_fuse_bits_get(GET_LOCK_BITS);      
}

void sable_f7s() {
    w_f7s();  

    sendbytet0(ins);  
    sendbytet0(low_f7);
    sendbytet0(high_f7);
    sendbytet0(ext_f7);
    sendbytet0(k_b);

    sw1 = 0x90; 
    sw2 = 0x00;
}


void load_f6_bytes() {
    h1[0] = boot_signature_byte_get(0x0000);
    h1[1] = boot_signature_byte_get(0x0001);
    h1[2] = boot_signature_byte_get(0x0002);
    h1[3] = boot_signature_byte_get(0x0003);
    h1[4] = boot_signature_byte_get(0x0004);
    h1[5] = boot_signature_byte_get(0x0005);
    h1[6] = boot_signature_byte_get(0x0006);
    h1[7] = boot_signature_byte_get(0x0007);
}


void sable_f6() {
    load_f6_bytes();
    
    if (p3 > SIGNATURE_SIZE) { sw1 = 0x6C; sw2 = SIGNATURE_SIZE; } 
    else {
        sendbytet0(ins);
        for (uint8_t i = 0; i < p3; i++) {
            sendbytet0(h1[i]);
        }
        sw1 = 0x90; sw2 = 0x00;
    }
}


uint8_t _c00b() {
    return (uint8_t)(PINB & 0b11011111);  
}

uint8_t _c00c() {
    return (uint8_t)(PINC);
}

uint8_t _c00d() {
    return (uint8_t)(PIND);
}

   
uint16_t t_f5_mc() {
    PRR &= ~(1 << PRADC);  
    ADMUX = (1 << REFS0) | (1 << MUX1) | (1 << MUX0);  
    ADMUX = (1 << REFS0) | (1 << MUX1) | (1 << MUX2);  
    ADCSRA |= (1 << ADEN);  
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));

    return ADC; 
}


void f2() {
    if (p3 != 1) { sw1 = 0x6C; sw2 = 1; } 
    else { sendbytet0(ins); sendbytet0(_c00b()); sw1 = 0x90; sw2 = 0x00; }
}

  
void f3() {
    if (p3 != 1) { sw1 = 0x6C; sw2 = 1; } 
    else { sendbytet0(ins); sendbytet0(_c00c()); sw1 = 0x90; sw2 = 0x00; }
}

uint8_t _c00d_random_byte() {
    uint8_t rand_byte = 0;
    for (uint8_t i = 0; i < 8; i++) {
        rand_byte <<= 1;
        rand_byte |= (PIND & (1 << PD3)) ? 1 : 0;  
        _delay_us(10);
    }
    return rand_byte;
}

void f4_fe() {
    uint8_t pind_value = _c00d();  
    uint8_t pind_random = _c00d_random_byte();  

    if (p3 != 2) {
        sw1 = 0x6C;

     sw2 = 2;
    } else {
        sendbytet0(ins);
        sendbytet0(pind_value); 
        sendbytet0(pind_random); 
        sw1 = 0x90;
        sw2 = 0x00;
    }
}

void f4() {
    if (p3 != 1) { sw1 = 0x6C; sw2 = 1; } 
    else { sendbytet0(ins); sendbytet0(_c00d()); sw1 = 0x90; sw2 = 0x00; }
}

void sable_f5_mc() {
    uint16_t adc_mc_value = t_f5_mc(); 

    if (p3 != 2) {  
        sw1 = 0x6C;
        sw2 = 2;
    } else {
        sendbytet0(ins);
        sendbytet0((uint8_t)(adc_mc_value >> 8));  
        sendbytet0((uint8_t)(adc_mc_value & 0xFF));  
 
        sw1 = 0x90;
        sw2 = 0x00;
    }
}
uint8_t _c00d_d00D3() {
    uint8_t rand_byte = 0;
    for (uint8_t i = 0; i < 8; i++) {
        rand_byte <<= 1;
        rand_byte |= (PIND & (1 << PD3)) ? 1 : 0;  
        _delay_us(10);
    }
    return rand_byte;
}

uint8_t _c00d_d00D4() {
    uint8_t rand_byte = 0;
    for (uint8_t i = 0; i < 8; i++) {
        rand_byte <<= 1;
        rand_byte |= (PIND & (1 << PD4)) ? 1 : 0;  
        _delay_us(10);
    }
    return rand_byte;
}

uint8_t _c00d_d00D5() {
    uint8_t rand_byte = 0;
    for (uint8_t i = 0; i < 8; i++) {
        rand_byte <<= 1;
        rand_byte |= (PIND & (1 << PD5)) ? 1 : 0;
        _delay_us(10);
    }
    return rand_byte;
}

uint8_t _c00d_d00D6() {
    uint8_t rand_byte = 0;
    for (uint8_t i = 0; i < 8; i++) {
        rand_byte <<= 1;
        rand_byte |= (PIND & (1 << PD6)) ? 1 : 0;  
        _delay_us(10);
    }
    return rand_byte;
}

uint8_t _c00d_d00D7() {
    uint8_t rand_byte = 0;
    for (uint8_t i = 0; i < 8; i++) {
        rand_byte <<= 1;
        rand_byte |= (PIND & (1 << PD7)) ? 1 : 0; 
        _delay_us(10);
    }
    return rand_byte;
}

void f4_d0PD3() {
    uint8_t pind_value = _c00d();
    uint8_t pind_random = _c00d_d00D3();

    if (p3 != 2) {
        sw1 = 0x6C;
        sw2 = 2;
    } else {
        sendbytet0(ins);
        sendbytet0(pind_value);
        sendbytet0(pind_random);
        sw1 = 0x90;
        sw2 = 0x00;

    }
}

void f4_d0PD4() {
    uint8_t pind_value = _c00d();
    uint8_t pind_random = _c00d_d00D4();

    if (p3 != 2) {
        sw1 = 0x6C;
        sw2 = 2;
    } else {
        sendbytet0(ins);
        sendbytet0(pind_value);
        sendbytet0(pind_random);
        sw1 = 0x90;
        sw2 = 0x00;
    }
}

void f4_d0PD5() {
    uint8_t pind_value = _c00d();
    uint8_t pind_random = _c00d_d00D5();

    if (p3 != 2) {
        sw1 = 0x6C;
        sw2 = 2;
    } else {
        sendbytet0(ins);
        sendbytet0(pind_value);
        sendbytet0(pind_random);
        sw1 = 0x90;
        sw2 = 0x00;
    }
}

void f4_d0PD6() {
    uint8_t pind_value = _c00d();
    uint8_t pind_random = _c00d_d00D6();

    if (p3 != 2) {
        sw1 = 0x6C;
        sw2 = 2;
    } else {
        sendbytet0(ins);
        sendbytet0(pind_value);
        sendbytet0(pind_random);
        sw1 = 0x90;
        sw2 = 0x00;
    }
}

void f4_d0PD7() {
    uint8_t pind_value = _c00d();
    uint8_t pind_random = _c00d_d00D7();

    if (p3 != 2) {
        sw1 = 0x6C;
        sw2 = 2;
    } else {
        sendbytet0(ins);
        sendbytet0(pind_value);
        sendbytet0(pind_random);
        sw1 = 0x90;
        sw2 = 0x00;
    }
}

void sable_f9() {
    if (p3 != 16) {  
        sw1 = 0x6C;  
        sw2 = 16;  
        return;
    }

    w_f7s(); 
    load_f6_bytes();  

    sendbytet0(ins);  


    sendbytet0(low_f7);
    sendbytet0(high_f7);
    sendbytet0(ext_f7);
    sendbytet0(k_b);

   
    for (uint8_t i = 0; i < SIGNATURE_SIZE; i++) {
        sendbytet0(h1[i]);
    }


    sendbytet0(_c00b());
    sendbytet0(_c00c());
    sendbytet0(_c00d());

    sendbytet0(y_y);  

    sw1 = 0x90;
    sw2 = 0x00;
}



void sable_f5_fb() {
    PRR &= ~(1 << PRADC); 
    sendbytet0(ins);

    for (uint8_t i = 0; i < 8; i++) {
        DDRC &= ~(1 << i);  

        if (i & 1) PORTC |= (1 << i);
        else PORTC &= ~(1 << i);

        ADMUX = (1 << REFS0) | i;  


        ADCSRA |= (1 << ADEN) | (1 << ADSC);
        while (ADCSRA & (1 << ADSC));

 
        ADCSRA |= (1 << ADSC);
        while (ADCSRA & (1 << ADSC));

        sendbytet0((uint8_t)(ADC >> 8));  
        sendbytet0((uint8_t)(ADC & 0xFF));  
    }

    sw1 = 0x90;
    sw2 = 0x00;
}

void sable_f5_fb_f0() {
    PRR &= ~(1 << PRADC);  
    ADMUX = (1 << REFS0);  

   
    DDRC &= ~(1 << PC6);  
    PORTC &= ~(1 << PC6);  

    sendbytet0(ins);

    for (uint8_t i = 0; i < 8; i++) {  
        ADMUX = (1 << REFS0) | i;  
        ADCSRA |= (1 << ADSC);
        while (ADCSRA & (1 << ADSC));
        sendbytet0((uint8_t)(ADC >> 8)); 
        sendbytet0((uint8_t)(ADC & 0xFF));  
    }

    sw1 = 0x90;
    sw2 = 0x00;
}

void sable_f5_fb_f1() {
    PRR &= ~(1 << PRADC); 

    sendbytet0(ins);

    for (uint8_t i = 0; i < 8; i++) { 
      
        if (i < 6) {  
            ADMUX = (1 << REFS0) | i;
        } else { 
            ADMUX = (1 << REFS1) | (1 << REFS0) | i;
        }

        ADCSRA |= (1 << ADSC);
        while (ADCSRA & (1 << ADSC));

        sendbytet0((uint8_t)(ADC >> 8));  
        sendbytet0((uint8_t)(ADC & 0xFF));  
    }

    sw1 = 0x90;
    sw2 = 0x00;
}

void atr(uint8_t n, char* hist) {
    sendbytet0(0x3b);
    sendbytet0(n);
    while (n--) {
        sendbytet0(*hist++);
    }
}

int main(void) {
    ACSR = 0x80;
    PORTB = 0xff;
    DDRB = 0xff;
    DDRC = 0xff;
    DDRD = 0;
    PORTC = 0x00;
    PORTD = 0;//0xff;
    ASSR = (1 << EXCLK) + (1 << AS2);
    //PRR = 0x87;
    PRR = 0x87 & ~(1 << PRADC);  
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

