#ifndef __BINOP_H__
#define __BINOP_H__

#include <stdio.h>

// NIST FIPS 203; Algoritm 3
int* BitsToBytes(int* b);

// NIST FIPS 203; Algoritm 4
int* BytesToBits(int* B);

// NIST FIPS 203; Algoritm 5
int* ByteEncode(int* F, int d);

// NIST FIPS 203; Algoritm 6
int* ByteDecode(int* B, int d);

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