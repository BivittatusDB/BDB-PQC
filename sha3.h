#ifndef __SHA3_H__
#define __SHA3_H__

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define KECCAK_ROUNDS 24
#define KECCAK_STATE_SIZE 25
#define SHA3_256_DIGEST_SIZE 32
#define SHA3_512_DIGEST_SIZE 64
#define SHAKE128_RATE 168
#define SHAKE256_RATE 136

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

static const int keccak_rotation_offsets_0[5] = { 0,  1, 62, 28, 27};
static const int keccak_rotation_offsets_1[5] = {36, 44,  6, 55, 20};
static const int keccak_rotation_offsets_2[5] = { 3, 10, 43, 25, 39};
static const int keccak_rotation_offsets_3[5] = {41, 45, 15, 21,  8};
static const int keccak_rotation_offsets_4[5] = {18,  2, 61, 56, 14};

static const int* keccak_rotation_offsets[5] = {
    keccak_rotation_offsets_0,
    keccak_rotation_offsets_1,
    keccak_rotation_offsets_2,
    keccak_rotation_offsets_3,
    keccak_rotation_offsets_4
};

// Function to rotate bits to the left
static inline uint64_t rotate_left(uint64_t x, unsigned int n) {
    return (x << n) | (x >> (64 - n));
}

// Keccak Permutation
static void keccak_permutation(uint64_t state[KECCAK_STATE_SIZE]) {
    for (unsigned int round = 0; round < KECCAK_ROUNDS; ++round) {
        uint64_t c[5], d[5], b[5][5];

        // Theta step
        for (unsigned int i = 0; i < 5; ++i) {
            c[i] = state[i] ^ state[i + 5] ^ state[i + 10] ^ state[i + 15] ^ state[i + 20];
        }
        for (unsigned int i = 0; i < 5; ++i) {
            d[i] = c[(i + 4) % 5] ^ rotate_left(c[(i + 1) % 5], 1);
        }
        for (unsigned int i = 0; i < 5; ++i) {
            for (unsigned int j = 0; j < 5; ++j) {
                state[i + 5 * j] ^= d[i];
            }
        }

        // Rho and Pi steps
        for (unsigned int i = 0; i < 5; ++i) {
            for (unsigned int j = 0; j < 5; ++j) {
                b[j][(2 * i + 3 * j) % 5] = rotate_left(state[i + 5 * j], keccak_rotation_offsets[i][j]);
            }
        }

        // Chi step
        for (unsigned int i = 0; i < 5; ++i) {
            for (unsigned int j = 0; j < 5; ++j) {
                state[i + 5 * j] = b[i][j] ^ (~b[(i + 1) % 5][j] & b[(i + 2) % 5][j]);
            }
        }

        // Iota step
        state[0] ^= keccak_round_constants[round];
    }
}

// Keccak absorption
static void keccak_absorb(uint64_t state[KECCAK_STATE_SIZE], const uint8_t *data, size_t rate, size_t data_len) {
    size_t block_size = rate / 8;
    while (data_len >= block_size) {
        for (size_t i = 0; i < block_size / 8; ++i) {
            state[i] ^= ((const uint64_t *)data)[i];  // Línea 82 corregida
        }
        keccak_permutation(state);
        data += block_size;
        data_len -= block_size;
    }

    uint8_t *last_block = (uint8_t *)malloc(block_size);
    if (last_block == NULL) {
        fprintf(stderr, "Error: No se pudo asignar memoria para last_block.\n");
        return;
    }

    memset(last_block, 0, block_size);
    memcpy(last_block, data, data_len);
    last_block[data_len] = 0x06;  // Padding
    last_block[block_size - 1] |= 0x80;  // Multi-rate padding
    for (size_t i = 0; i < block_size / 8; ++i) {
        state[i] ^= ((uint64_t *)last_block)[i];
    }
    keccak_permutation(state);  // Apply permutation after padding

    free(last_block);  // Free memory
}

// Squeeze Keccak
static void keccak_squeeze(uint64_t state[KECCAK_STATE_SIZE], uint8_t *output, size_t rate, size_t output_len) {
    size_t block_size = rate / 8;
    while (output_len >= block_size) {
        memcpy(output, state, block_size);  // Línea 116 corregida
        keccak_permutation(state);
        output += block_size;
        output_len -= block_size;
    }
    memcpy(output, state, output_len);
}

// SHA3-256
void sha3_256(const uint8_t *data, size_t data_len, uint8_t *hash) {
    uint64_t state[KECCAK_STATE_SIZE] = {0};
    keccak_absorb(state, data, 1088, data_len);
    keccak_squeeze(state, hash, 1088, SHA3_256_DIGEST_SIZE);
}

// SHA3-512
void sha3_512(const uint8_t *data, size_t data_len, uint8_t *hash) {
    uint64_t state[KECCAK_STATE_SIZE] = {0};
    keccak_absorb(state, data, 576, data_len);
    keccak_squeeze(state, hash, 576, SHA3_512_DIGEST_SIZE);
}

// SHAKE128
void shake128(const uint8_t *data, size_t data_len, uint8_t *output, size_t output_len) {
    uint64_t state[KECCAK_STATE_SIZE] = {0};
    keccak_absorb(state, data, SHAKE128_RATE * 8, data_len);
    keccak_squeeze(state, output, SHAKE128_RATE * 8, output_len);
}

// SHAKE256
void shake256(const uint8_t *data, size_t data_len, uint8_t *output, size_t output_len) {
    uint64_t state[KECCAK_STATE_SIZE] = {0};
    keccak_absorb(state, data, SHAKE256_RATE * 8, data_len);
    keccak_squeeze(state, output, SHAKE256_RATE * 8, output_len);
}

// Auxiliary function to print a hash or hexadecimal output
void print_hex(const uint8_t* data, size_t len) {
    for (size_t i = 0; i < len; i++) {
        printf("%02x", data[i]);
    }
    printf("\n");
}

#endif //__SHA3_H__
