#ifndef __NTT_H__
#define __NTT_H__

#include "constants.h"
#include <stdlib.h>

// Function to perform modular exponentiation: (base^exp) % mod
int mod_exp(int base, int exp, int mod) {
    int result = 1;
    base = base % mod;
    while (exp > 0) {
        if (exp % 2 == 1) {
            result = (result * base) % mod;
        }
        exp = exp >> 1;
        base = (base * base) % mod;
    }
    return result;
}

int* NTT(int* f, size_t size) {
    int* f_hat = (int*)malloc(size * sizeof(int));
    if (f_hat == NULL) {
        // Handle memory allocation failure
        exit(1);
    }

    for (size_t i = 0; i < size; i++) {
        f_hat[i] = f[i];  // Copy input to output
    }

    int i = 1;
    for (int len = 128; len >= 2; len /= 2) {
        for (int start = 0; start < N; start += 2 * len) {
            int ntt_zeta = mod_exp(Z, BitRev7(i++), Q);
            for (int j = start; j < start + len; j++) {
                int t = (ntt_zeta * f_hat[j + len]) % Q;
                f_hat[j + len] = ((f_hat[j] - t) % Q + Q) % Q;
                f_hat[j] = ((f_hat[j] + t) % Q + Q) % Q;
            }
        }
    }
    return f_hat;
}

#endif // __NTT_H__