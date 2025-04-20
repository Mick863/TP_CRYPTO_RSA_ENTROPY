#include "primalite.h"
#include <stdint.h>
#include <stdlib.h>

static uint64_t modexp(uint64_t base, uint64_t exp, uint64_t mod) {
    uint64_t result = 1;
    base %= mod;
    while (exp > 0) {
        if (exp & 1) result = (result * base) % mod;
        exp >>= 1;
        base = (base * base) % mod;
    }
    return result;
}

int miller_rabin(uint64_t n, int k) {
    if (n < 2) return 0;
    if (n % 2 == 0) return n == 2;

    // Écriture n-1 = 2^r * d
    uint64_t d = n - 1;
    int r = 0;
    while ((d & 1) == 0) {
        d >>= 1;
        r++;
    }

    for (int i = 0; i < k; i++) {
        uint64_t a = 2 + rand() % (n - 4);
        uint64_t x = modexp(a, d, n);
        if (x == 1 || x == n - 1) continue;

        int ok = 0;
        for (int j = 0; j < r - 1; j++) {
            x = modexp(x, 2, n);
            if (x == n - 1) {
                ok = 1;
                break;
            }
        }
        if (!ok) return 0;
    }
    return 1;
}
