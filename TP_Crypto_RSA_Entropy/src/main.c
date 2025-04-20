#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "gener_alea.h"
#include "primalite.h"

uint64_t candidate_to_uint64(const uint8_t *buf) {
    uint64_t n = 0;
    for (int i = 0; i < 8; i++) {
        n = (n << 8) | buf[i];
    }
    return n;
}

void handle_apdu(uint8_t ins) {
    switch (ins) {
        case 0xF5: {
            printf("[APDU 0xF5] Génération de candidat RSA\n");
            uint8_t candidate[16]; // 128 bits
            generate_candidate(candidate, sizeof(candidate));

            printf("Candidat : ");
            for (int i = 0; i < 16; i++) {
                printf("%02X", candidate[i]);
            }
            printf("\n");

            uint64_t number = candidate_to_uint64(candidate);
            printf("Test de primalité de %llu...\n", number);

            if (miller_rabin(number, 5)) {
                printf("Premier probable (Miller-Rabin)\n");
            } else {
                printf("Non premier\n");
            }

            break;
        }

        case 0xB7:
            printf("[APDU 0xB7] NIST/FIPS requis\n");
            break;

        default:
            printf("APDU inconnue: 0x%02X\n", ins);
    }
}

int main() {
    printf("=== Firmware Simulation RSA ===\n");

    uint8_t apdus[] = {0xF5, 0xB7, 0xFF};
    for (int i = 0; apdus[i] != 0xFF; i++) {
        handle_apdu(apdus[i]);
    }

    return 0;
}
