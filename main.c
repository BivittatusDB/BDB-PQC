#include <stdio.h>
#include <stdlib.h>  // Para malloc y free
#include "constants.h"
#include "ntt.h"

int main() {
    // Verifica que N está definido en constants.h
    if (N <= 0) {
        fprintf(stderr, "Error: The value of N must be greater than 0.\n");
        return 1;
    }

    int f[N];  // Array de tamaño N
    // Inicializa el array con algunos valores
    for (int i = 0; i < N; i++) {
        f[i] = i + 1;
    }

    int* f_hat = NTT(f, N);

    if (f_hat == NULL) {
        fprintf(stderr, "Error: NTT could not be calculated.\n");
        return 1;
    }

    printf("NTT result:\n");
    for (int i = 0; i < N; i++) {
        printf("%d ", f_hat[i]);
    }
    printf("\n");

    // Frees the allocated memory
    free(f_hat);

    return 0;
}