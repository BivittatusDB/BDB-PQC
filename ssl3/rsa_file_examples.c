#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <stdio.h>
#include <stdlib.h>

#define RSA_KEYSIZE 4096

void handle_openssl_error() {
    ERR_print_errors_fp(stderr);
    abort();
}

int main() {
    // Initialize OpenSSL
    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();

    // Generate RSA key pair using EVP API
    EVP_PKEY *pkey = NULL;
    EVP_PKEY_CTX *keygen_ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);
    if (!keygen_ctx) handle_openssl_error();

    if (EVP_PKEY_keygen_init(keygen_ctx) <= 0) handle_openssl_error();
    if (EVP_PKEY_CTX_set_rsa_keygen_bits(keygen_ctx, RSA_KEYSIZE) <= 0) handle_openssl_error();
    if (EVP_PKEY_keygen(keygen_ctx, &pkey) <= 0) handle_openssl_error();

    // Save public and private keys to PEM files
    FILE *pub_file = fopen("public_key.pem", "wb");
    if (!pub_file) {
        perror("Failed to open public_key.pem");
        return 1;
    }
    if (!PEM_write_PUBKEY(pub_file, pkey)) {
        handle_openssl_error();
    }
    fclose(pub_file);

    FILE *priv_file = fopen("private_key.pem", "wb");
    if (!priv_file) {
        perror("Failed to open private_key.pem");
        return 1;
    }
    if (!PEM_write_PrivateKey(priv_file, pkey, NULL, NULL, 0, NULL, NULL)) {
        handle_openssl_error();
    }
    fclose(priv_file);

    printf("RSA keys have been saved to 'public_key.pem' and 'private_key.pem'.\n");





    // Load public and private keys from PEM files
    FILE *pub_file_load = fopen("public_key.pem", "rb");
    if (!pub_file_load) {
        perror("Failed to open public_key.pem");
        return 1;
    }
    EVP_PKEY *loaded_pub_key = PEM_read_PUBKEY(pub_file_load, NULL, NULL, NULL);
    if (!loaded_pub_key) handle_openssl_error();
    fclose(pub_file_load);

    FILE *priv_file_load = fopen("private_key.pem", "rb");
    if (!priv_file_load) {
        perror("Failed to open private_key.pem");
        return 1;
    }
    EVP_PKEY *loaded_priv_key = PEM_read_PrivateKey(priv_file_load, NULL, NULL, NULL);
    if (!loaded_priv_key) handle_openssl_error();
    fclose(priv_file_load);

    printf("RSA keys have been loaded from 'public_key.pem' and 'private_key.pem'.\n");

    // Cleanup
    EVP_PKEY_free(pkey);
    EVP_PKEY_free(loaded_pub_key);
    EVP_PKEY_free(loaded_priv_key);
    EVP_PKEY_CTX_free(keygen_ctx);
    EVP_cleanup();
    ERR_free_strings();

    return 0;
}
