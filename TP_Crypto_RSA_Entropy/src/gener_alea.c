#include <stdint.h>
#include <stddef.h>      
#include <string.h>

#include "gener_alea.h"
#include "entropie.h"
#include "blake2s.h"

#define ENTROPY_LEN 128

void generate_candidate(uint8_t *out, size_t outlen) {
    uint8_t entropy[ENTROPY_LEN];

    // Récupération d'entropie
    collect_entropy(entropy, ENTROPY_LEN);

    // Hachage pour obtenir un candidat pseudo-aléatoire
    blake2s(out, outlen, entropy, ENTROPY_LEN, NULL, 0);

    // Renforce les contraintes pour l'utilisation RSA
    out[0] |= 0x80;               // bit fort pour forcer une taille de clé (MSB = 1)
    out[outlen - 1] |= 0x01;      // bit faible pour forcer l'impair (LSB = 1)
}
