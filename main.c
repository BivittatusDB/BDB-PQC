#include <stdio.h>
#include <stdlib.h>  // For malloc and free
#include "constants.h"
#include "ntt.h"

int main() {
    // Verify that N is defined in constants.h
    if (N <= 0) {
        fprintf(stderr, "Error: The value of N must be greater than 0.\n");
        return 1;
    }

    int f[N];  // Array of size N
    // Initialises the array with some values
    for (int i = 0; i < N; i++) {
        f[i] = i + 1;
    }

    int* f_hat = inv_NTT(NTT(f, N), N);

    for (int i = 0; i < N; i++) {
        printf("%d ", f_hat[i]==f[i]);
    }
    printf("\n");

    // Frees the allocated memory
    free(f_hat);
    return 0;
}
