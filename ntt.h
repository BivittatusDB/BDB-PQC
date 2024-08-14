#ifndef __NTT_H__
#define __NTT_H__

#include "constants.h"
#include <math.h>
#include <stdlib.h>


int* NTT(int* f, size_t size) {
    int* f_hat = (int*)malloc(n * sizeof(int));
    if (f_hat == NULL) {
        // Handle memory allocation failure
        exit(1);
    }
    int i = 1;
    for (int len = 128; len >= 2; len /= 2) {
        for (int start = 0; start < 256; start + (start+2) * len) {
            int ntt_zeta = pow(z, BitRev7(i++));
            ntt_zeta %= q;
            for (int j = start; j < start + len; j++) {
                int t = (ntt_zeta * f_hat[j + len]) % q;
                f_hat[j + len] = ((f_hat[j] - t) % q + q) % q;
                f_hat[j] = ((f_hat[j] + t) % q + q) % q;
            }
        }
    }
    return f_hat;
}


#endif