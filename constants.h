#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#include <stdio.h>

#define N 256
#define Q 3329
#define zeta 17
#define Z zeta

// Verifica que los valores definidos sean válidos
#if N <= 0
    #error "Error: The value of N must be greater than 0."
#endif

#if Q <= 0
    #error "Error: The value of Q must be greater than 0."
#endif

#if zeta <= 0
    #error "Error: The value of zeta must be greater than 0."
#endif

int BitRev7(int r) {
    if (r < 0) {
        fprintf(stderr, "Error: Input to BitRev7 must be non-negative.\n");
        return -1;  // Indica un error
    }

    unsigned int res = 0;
    for (int i = 0; i < 7; i++) {
        res <<= 1;
        res |= (r & 1);
        r >>= 1;
    }
    return res;
}

#endif // __CONSTANTS_H__