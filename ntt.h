#ifndef __NTT_H__
#define __NTT_H__

#include "constants.h"
#include "BinOp.h"
#include "sha3.h"
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

// NIST FIPS 203; Algoritm 9
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

// NIST FIPS 203; Algoritm 10
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

// NIST FIPS 203; Algoritm 12
binomial BaseCaseMultiply(binomial a, binomial b, int gamma){
    binomial c;
    c.a = a.a*b.a + a.b*b.b*gamma;
    c.b = a.a*b.b + a.b*b.a;
    return b;
}

// NIST FIPS 203; Algoritm 11
int* MultiplyNTTs(int* f_hat, int* g_hat, size_t size){
    int* h_hat = (int*)malloc(size*sizeof(int));
    for (int i = 0; i<128; i++){
        binomial a, b;
        a.a, a.b = f_hat[2*i], f_hat[2*i + 1];
        b.a, b.b = g_hat[2*i], g_hat[2*i + 1];
        int gamma= mod_exp(Z, 2*BitRev7(i)+1, Q);
        binomial c = BaseCaseMultiply(a,b,gamma);
        h_hat[2*i], h_hat[2*i + 1] = c.a, c.b;
    }
    return h_hat;
}

// NIST FIPS 203; Algoritm 7
int* SampleNTT(int* B, size_t B_Size){
    int* a_hat = (int*)malloc(256*sizeof(int));
    EVP_MD_CTX *ctx = XOF_init();
    XOF_Absorb(ctx, (char *)B, B_Size);
    int j = 0;
    while (j < 256){
        int* C = (int *)XOF_Squeeze(ctx, 3);
        int d1 = C[0] + (256*C[1]%16);
        int d2 = floor(C[1]/16)+ 16 * C[2];
        if (d1 < Q) {
            a_hat[j] = d1;
            j++;
        }
        if (d2 < Q && j < 256){
            a_hat[j] = d2;
            j++;
        }
    }
    return a_hat;
}

#endif // __NTT_H__