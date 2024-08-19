//Implementation of SHAKE for use in the testing
// FIPS 203 Section 4

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#define SHAKE128_RATE 168
#define KECCAK_ROUNDS 24

// Round constants for the permutation Keccak-f[1600]
static const uint64_t keccak_round_constants[KECCAK_ROUNDS] = {
    0x0000000000000001ULL, 0x0000000000008082ULL, 0x800000000000808aULL,
    0x8000000080008000ULL, 0x000000000000808bULL, 0x0000000080000001ULL,
    0x8000000080008081ULL, 0x8000000000008009ULL, 0x000000000000008aULL,
    0x0000000000000088ULL, 0x0000000080008009ULL, 0x000000008000000aULL,
    0x000000008000808bULL, 0x800000000000008bULL, 0x8000000000008089ULL,
    0x8000000000008003ULL, 0x8000000000008002ULL, 0x8000000000000080ULL,
    0x000000000000800aULL, 0x800000008000000aULL, 0x8000000080008081ULL,
    0x8000000000008080ULL, 0x0000000080000001ULL, 0x8000000080008008ULL
};

// Left-hand circular rotation of a 64-bit integer
static inline uint64_t ROTL64(uint64_t x, uint8_t n) {
    return ((x << n) | (x >> (64 - n)));
}

// Macro for error handling
#define HANDLE_ERROR(msg) \
    do { fprintf(stderr, "Error: %s\n", msg); return -1; } while (0)

// Keccak permutation function
void keccak_permutation(uint64_t *state) {
    uint64_t B[5], T, C[5];
    
    for (int round = 0; round < KECCAK_ROUNDS; round++) {
        // Theta Step
        for (int i = 0; i < 5; i++) {
            C[i] = state[i] ^ state[i + 5] ^ state[i + 10] ^ state[i + 15] ^ state[i + 20];
        }
        for (int i = 0; i < 5; i++) {
            T = C[(i + 4) % 5] ^ ROTL64(C[(i + 1) % 5], 1);
            for (int j = 0; j < 25; j += 5) {
                state[i + j] ^= T;
            }
        }

        // Rho and Pi steps
        T = state[1];
        for (int i = 0; i < 24; i++) {
            int j = (i + 1) * (i + 2) / 2 % 64;
            B[0] = state[5 * ((i + 1) % 5) + (i + 3) % 5];
            state[5 * ((i + 1) % 5) + (i + 3) % 5] = ROTL64(T, j);
            T = B[0];
        }

        // Chi step
        for (int j = 0; j < 25; j += 5) {
            for (int i = 0; i < 5; i++) {
                B[i] = state[j + i];
            }
            for (int i = 0; i < 5; i++) {
                state[j + i] ^= (~B[(i + 1) % 5]) & B[(i + 2) % 5];
            }
        }

        // Iota step
        state[0] ^= keccak_round_constants[round];
    }
}

int keccak_absorb(uint64_t *state, const uint8_t *input, size_t inlen, uint8_t pad) {
    if (inlen > SHAKE128_RATE) {
        HANDLE_ERROR("Input length exceeds SHAKE128 rate.");
    }

    uint8_t temp[SHAKE128_RATE] = {0};
    memcpy(temp, input, inlen);
    temp[inlen] = pad;
    temp[SHAKE128_RATE - 1] |= 0x80;

    for (size_t i = 0; i < SHAKE128_RATE / 8; i++) {
        state[i] ^= ((uint64_t *)temp)[i];
    }

    // Call to the Keccak permutation function
    keccak_permutation(state);

    return 0;
}

int keccak_squeezeblocks(uint8_t *output, size_t nblocks, uint64_t *state) {
    if (nblocks == 0) {
        HANDLE_ERROR("Number of blocks to squeeze is zero.");
    }

    while (nblocks > 0) {
        // Call to the Keccak permutation function
        keccak_permutation(state);

        for (size_t i = 0; i < SHAKE128_RATE / 8; i++) {
            ((uint64_t *)output)[i] = state[i];
        }

        output += SHAKE128_RATE;
        nblocks--;
    }
    return 0;
}

int shake128(uint8_t *output, size_t outlen, const uint8_t *input, size_t inlen) {
    uint64_t state[25] = {0};
    uint8_t temp[SHAKE128_RATE];
    size_t nblocks = outlen / SHAKE128_RATE;

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
