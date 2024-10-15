#ifndef __BINOP_H__
#define __BINOP_H__

#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "random.h"
#include "constants.h"

// NIST FIPS 203; Algoritm 3
uint8_t* BitsToBytes(int* b, int l){
    uint8_t* B = (uint8_t*)malloc(l*sizeof(uint8_t));
    for (int i = 0; i<8*l; i++){
        B[(int)(i/8)]=B[(int)(i/8)]+b[i]*pow(2, i%8);
    }
    return B;
}

// NIST FIPS 203; Algoritm 4
int* BytesToBits(uint8_t* B, int l){
    uint8_t* C = (uint8_t*)malloc(l*sizeof(uint8_t));
    int* b = (int*)malloc((8*l)*sizeof(int));
    for (int i =0; i<l;i++){
        for (int j=0; j<8; j++){
            b[8*i+j]=(int)(C[i]%2);
            C[i]=(uint8_t)(C[i]/2);
        }
    }
    free(C);
    return b;
}

// NIST FIPS 203; Algoritm 5
uint8_t* ByteEncode(int* F, int d){
    int* b = (int*)malloc(256*d*sizeof(int));
    for (int i = 0; i<256; i++){
        int a=F[i];
        for (int j =0; j<d; j++){
            b[i*d+j] = a%2;
            a=(a-b[i*d+j])/2;
        }
    }
    uint8_t *B = BitsToBytes(b,256);
    return B;
}

// NIST FIPS 203; Algoritm 6
int* ByteDecode(uint8_t* B, int d){
    int m;
    if (d=12){
        m = Q;
    } else {
        m = (1 << d);
    }
    int *F = (int *)malloc(256*sizeof(int));
    int* b= BytesToBits(B, 256);
    int f=0;
    for (int i = 0; i< 256; i++){
        for (int j =0; j<d; j++){
            f+=b[i*d+j]*(1<<j);
        }
        F[i]=f%m;
    }
    return F;
}

// Reversal of 7 bit binary representation of an integer r (NIST FIPS 203; Section 2.3)
int BitRev7(int r) {
    if (r < 0) {
        fprintf(stderr, "Error: Input to BitRev7 must be non-negative.\n");
        return -1;  // Indicates an error
    }

    unsigned int res = 0;
    for (int i = 0; i < 7; i++) {
        res <<= 1;
        res |= (r & 1);
        r >>= 1;
    }
    return res;
}

#endif //__BINOP_H__
