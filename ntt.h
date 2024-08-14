#ifndef __NTT_H__
#define __NTT_H__

#include "constants.h"
#include <stdlib.h>
#include <stdio.h>

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
    if (f == NULL) {
        fprintf(stderr, "Error: Input array is NULL.\n");
        return NULL;
    }

    int* f_hat = (int*)malloc(size * sizeof(int));
    if (f_hat == NULL) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        return NULL;
    }

    for (size_t i = 0; i < size; i++) {
        f_hat[i] = f[i];  // Copy input to output
    }

    int i = 1;
    for (int len = 128; len >= 2; len /= 2) {
        for (int start = 0; start < N; start += 2 * len) {
            int ntt_zeta = mod_exp(Z, BitRev7(i++), Q);
            if (ntt_zeta == 0) {
                fprintf(stderr, "Error: Exponent calculation error at index %d.\n", i-1);
                free(f_hat);
                return NULL;
            }
            for (int j = start; j < start + len; j++) {
                int t = (ntt_zeta * f_hat[j + len]) % Q;
                f_hat[j + len] = ((f_hat[j] - t) % Q + Q) % Q;
                f_hat[j] = ((f_hat[j] + t) % Q + Q) % Q;
            }
        }
    }
    return f_hat;
}

int* inv_NTT(int* f_hat, size_t size){
    int* f = (int*)malloc(size * sizeof(int));
    for (size_t i = 0; i < size; i++) {
        f[i] = f_hat[i];  // Copy input to output
    }
    int i=127;
    for (int len=2; len<=128; len*=2){
        for (int start = 0; start < 256; start+=2*len){
            int ntt_zeta = mod_exp(Z, BitRev7(i--), Q);
            for (int j = start; j<start+len; j++){
                int t=f[j];
                f[j] = ((t+f[j+len])%Q +Q)%Q;
                f[j+len] = ((ntt_zeta * (f[j+len]-t)%Q)+Q)%Q;
            }
        }
    }
    for (size_t i = 0; i < size; i++) {
        f[i] = (f[i]*3303)%Q;  // Copy input to output
    }
    return f;
}

#endif // __NTT_H__