#ifndef BLAKE2S_H
#define BLAKE2S_H

#include <stdint.h>
#include <stddef.h>


#define BLAKE2S_OUTLEN 32
#define BLAKE2S_BLOCKBYTES 64

typedef struct {
    uint8_t buf[BLAKE2S_BLOCKBYTES];
    uint32_t h[8];
    uint32_t t[2];
    uint32_t f[2];
    size_t buflen;
    int last_node;
} blake2s_ctx_t;

void blake2s_init(blake2s_ctx_t *S);
void blake2s_update(blake2s_ctx_t *S, const uint8_t *data, uint16_t len);
void blake2s_final(blake2s_ctx_t *S, uint8_t *digest);

// fonction raccourcie à usage unique (optionnelle mais utile)
static inline void blake2s(uint8_t *out, size_t outlen,
                           const void *in, size_t inlen,
                           const void *key, size_t keylen)
{
    blake2s_ctx_t ctx;
    blake2s_init(&ctx);
    blake2s_update(&ctx, (const uint8_t *)in, (uint16_t)inlen);
    blake2s_final(&ctx, out);
}

#endif
