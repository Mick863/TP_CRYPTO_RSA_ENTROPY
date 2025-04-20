#include "gener_alea.h"
#include "entropie.h"
#include "blake2s.h"
#include <string.h>
#include <stdint.h>


#define ENTROPY_LEN 128

void generate_candidate(uint8_t *out, size_t outlen) {
    uint8_t entropy[ENTROPY_LEN];
    collect_entropy(entropy, ENTROPY_LEN);

    blake2s(out, outlen, entropy, ENTROPY_LEN, NULL, 0);

    out[0] |= 0x80;               // bit fort pour la taille
    out[outlen - 1] |= 0x01;      // impair
}
