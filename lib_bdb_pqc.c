#include <stdio.h>
#include <string.h>
#include <oqs/oqs.h>

int main() {
    OQS_KEM *kem = OQS_KEM_new(OQS_KEM_alg_kyber_1024);
    if (kem == NULL) {
        printf("Failed to initialize KEM.\n");
        return EXIT_FAILURE;
    }

    // Allocate memory for keys and ciphertext
    uint8_t *public_key = malloc(kem->length_public_key);
    uint8_t *secret_key = malloc(kem->length_secret_key);
    uint8_t *ciphertext = malloc(kem->length_ciphertext);
    uint8_t *shared_secret_enc = malloc(kem->length_shared_secret);
    uint8_t *shared_secret_dec = malloc(kem->length_shared_secret);

    if (!public_key || !secret_key || !ciphertext || !shared_secret_enc || !shared_secret_dec) {
        printf("Memory allocation failed.\n");
        return EXIT_FAILURE;
    }

    // Key generation
    if (OQS_KEM_keypair(kem, public_key, secret_key) != OQS_SUCCESS) {
        printf("Key pair generation failed.\n");
        return EXIT_FAILURE;
    }
    printf("Key pair generated successfully.\n%c\n%c\n", *public_key, *secret_key);

    // Encapsulation
    if (OQS_KEM_encaps(kem, ciphertext, shared_secret_enc, public_key) != OQS_SUCCESS) {
        printf("Encapsulation failed.\n");
        return EXIT_FAILURE;
    }
    printf("Encapsulation successful.\n");

    // Decapsulation
    if (OQS_KEM_decaps(kem, shared_secret_dec, ciphertext, secret_key) != OQS_SUCCESS) {
        printf("Decapsulation failed.\n");
        return EXIT_FAILURE;
    }
    printf("Decapsulation successful.\n");

    // Verify shared secrets match
    if (memcmp(shared_secret_enc, shared_secret_dec, kem->length_shared_secret) == 0) {
        printf("Shared secrets match!\n");
    } else {
        printf("Shared secrets do not match.\n");
    }

    // Clean up
    free(public_key);
    free(secret_key);
    free(ciphertext);
    free(shared_secret_enc);
    free(shared_secret_dec);
    OQS_KEM_free(kem);

    return EXIT_SUCCESS;
}
