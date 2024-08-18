//Implementation of SHAKE for use in the testing
// FIPS 203 Section 4

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define SHAKE128_RATE 168

int keccak_absorb(uint64_t *state, const uint8_t *input, size_t inlen, uint8_t pad) {
    size_t i;
    uint8_t temp[SHAKE128_RATE];

    if (inlen > SHAKE128_RATE) {
        fprintf(stderr, "Error: Input length exceeds SHAKE128 rate.\n");
        return -1;
    }

    memset(temp, 0, SHAKE128_RATE);
    memcpy(temp, input, inlen);
    temp[inlen] = pad;
    temp[SHAKE128_RATE - 1] |= 0x80;

    for (i = 0; i < SHAKE128_RATE / 8; i++) {
        state[i] ^= ((uint64_t *)temp)[i];
    }

    // Keccak permutation function (implementation omitted for brevity)
    return 0;
}

int keccak_squeezeblocks(uint8_t *output, size_t nblocks, uint64_t *state) {
    size_t i;

    if (nblocks == 0) {
        fprintf(stderr, "Error: Number of blocks to squeeze is zero.\n");
        return -1;
    }

    while (nblocks > 0) {
        // Keccak permutation function (implementation omitted for brevity)

        for (i = 0; i < SHAKE128_RATE / 8; i++) {
            ((uint64_t *)output)[i] = state[i];
        }

        output += SHAKE128_RATE;
        nblocks--;
    }
    return 0;
}

int shake128(uint8_t *output, size_t outlen, const uint8_t *input, size_t inlen) {
    uint64_t state[25];
    uint8_t temp[SHAKE128_RATE];
    size_t nblocks = outlen / SHAKE128_RATE;
    size_t i;

    memset(state, 0, sizeof(state));
    if (keccak_absorb(state, input, inlen, 0x1F) != 0) {
        return -1;
    }

    if (keccak_squeezeblocks(output, nblocks, state) != 0) {
        return -1;
    }

    output += nblocks * SHAKE128_RATE;
    outlen -= nblocks * SHAKE128_RATE;

    if (outlen) {
        if (keccak_squeezeblocks(temp, 1, state) != 0) {
            return -1;
        }
        memcpy(output, temp, outlen);
    }
    return 0;
}
