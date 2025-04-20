#include "entropie.h"
#include <stdlib.h>
#include <stdint.h>


#define SRAM_SIZE 128
#define ADC_SAMPLES 64

void collect_sram_entropy(uint8_t *buffer, size_t len) {
    for (size_t i = 0; i < len; i++) {
        buffer[i] = rand() & 0xFF;
    }
}

void collect_adc_entropy(uint8_t *buffer, size_t len) {
    for (size_t i = 0; i < len; i++) {
        buffer[i] = (rand() >> 3) & 0xFF;
    }
}

void collect_entropy(uint8_t *dest, size_t len) {
    uint8_t sram_data[SRAM_SIZE];
    uint8_t adc_data[ADC_SAMPLES];

    collect_sram_entropy(sram_data, SRAM_SIZE);
    collect_adc_entropy(adc_data, ADC_SAMPLES);

    for (size_t i = 0; i < len; i++) {
        dest[i] = sram_data[i % SRAM_SIZE] ^ adc_data[i % ADC_SAMPLES];
    }
}
