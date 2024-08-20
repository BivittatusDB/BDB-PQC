#ifndef __KECCAK_H__
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "step_mapping.h"

#define nr 24

// NIST FIPS 202; Algorithm 7; "Keccak-p[b, nr](S)"
int* Keccak(int* S){
    int*** A = constructA(S);
    int round_start = 12+(2*l)-nr;
    int round_end = 12+(2*l)-1;
    for (int ir = round_start; ir<round_end; ir++){
        int*** nA = Rnd(A, ir);
        free(A);
        int*** A = nA;
    }
    int* nS = constructS(A);
    return nS;
}

// NIST FIPS 202; algorithm 9; "pad10*1(x,m)"
int* pad(int x, int m){
    int j = ((-m-2)%x+x)%x;
    int* P = (int*)malloc((j+3)*sizeof(int));
    memset(P, 0, j+2);
    P[0]=1;
    P[j+2]=1;
    P[j+3]=-1; //terminate the pad
    return P;
}

int* Absorb(int* N, int r) {
    // Step 1: Apply padding to N
    int* P = pad(r, sizeof(N)); // pad function from your code
    int* concatenated = (int*)malloc((sizeof(N) + sizeof(P)) * sizeof(int));
    memcpy(concatenated, N, sizeof(N) * sizeof(int));
    memcpy(concatenated + sizeof(N), P, sizeof(P) * sizeof(int));

    // Step 2: Calculate number of blocks
    int n = sizeof(concatenated) / r;

    // Step 3: Initialize the state S with zeros
    int* S = (int*)calloc(b, sizeof(int));

    // Step 4: Process each block in P
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < r; j++) {
            S[j] ^= concatenated[i * r + j];
        }
        int* new_S = Keccak(S);
        free(S);
        S = new_S;
    }

    free(concatenated);
    return S;
}


int* Squeeze(int* S, int d, int r) {
    int* Z = (int*)malloc(d * sizeof(int));
    int Z_index = 0;

    while (Z_index < d) {
        int to_copy = (d - Z_index < r) ? (d - Z_index) : r;
        memcpy(Z + Z_index, S, to_copy * sizeof(int));
        Z_index += to_copy;
        if (Z_index < d) {
            int* new_S = Keccak(S);
            free(S);
            S = new_S;
        }
    }

    return Z;
}

// NIST FIPS 202; algorithm 8; "SPONGE[f,pad,r](N,d)"
int* SPONGE(int* N, int d, int r) {
    // Absorb phase with padding handled inside Absorb
    int* S = Absorb(N, r);

    // Squeeze phase
    int* Z = Squeeze(S, d, r);

    // Clean up
    free(S);

    return Z;
}

#endif //__KECCAK_H__