#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <string.h>
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

    // Key generation using EVP API
    EVP_PKEY *pkey = NULL;
    EVP_PKEY_CTX *keygen_ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);
    if (!keygen_ctx) handle_openssl_error();

    if (EVP_PKEY_keygen_init(keygen_ctx) <= 0) handle_openssl_error();
    if (EVP_PKEY_CTX_set_rsa_keygen_bits(keygen_ctx, RSA_KEYSIZE) <= 0) handle_openssl_error();
    if (EVP_PKEY_keygen(keygen_ctx, &pkey) <= 0) handle_openssl_error();

    // Write keys to memory
    BIO *pub_bio = BIO_new(BIO_s_mem());
    BIO *priv_bio = BIO_new(BIO_s_mem());

    if (!PEM_write_bio_PUBKEY(pub_bio, pkey)) handle_openssl_error();
    if (!PEM_write_bio_PrivateKey(priv_bio, pkey, NULL, NULL, 0, NULL, NULL)) handle_openssl_error();

    char *pub_key = NULL, *priv_key = NULL;
    size_t pub_len = BIO_get_mem_data(pub_bio, &pub_key);
    size_t priv_len = BIO_get_mem_data(priv_bio, &priv_key);

    printf("Public Key (%zu bytes):\n%s\n", pub_len, pub_key);
    printf("Private Key (%zu bytes):\n%s\n", priv_len, priv_key);

    // Message to encrypt
    const char *message = "Hello, RSA-4096!";
    printf("Original Message: %s\n", message);

    // Encrypt with public key
    EVP_PKEY_CTX *encrypt_ctx = EVP_PKEY_CTX_new(pkey, NULL);
    if (!encrypt_ctx) handle_openssl_error();

    if (EVP_PKEY_encrypt_init(encrypt_ctx) <= 0) handle_openssl_error();
    if (EVP_PKEY_CTX_set_rsa_padding(encrypt_ctx, RSA_PKCS1_OAEP_PADDING) <= 0) handle_openssl_error();

    size_t encrypted_len = 0;
    if (EVP_PKEY_encrypt(encrypt_ctx, NULL, &encrypted_len, (unsigned char *)message, strlen(message) + 1) <= 0) handle_openssl_error();

    unsigned char *encrypted = malloc(encrypted_len);
    if (!encrypted) {
        fprintf(stderr, "Failed to allocate memory for encrypted message\n");
        return 1;
    }

    if (EVP_PKEY_encrypt(encrypt_ctx, encrypted, &encrypted_len, (unsigned char *)message, strlen(message) + 1) <= 0) handle_openssl_error();

    printf("Encrypted Message (%zu bytes):\n", encrypted_len);
    for (size_t i = 0; i < encrypted_len; i++) printf("%02x", encrypted[i]);
    printf("\n");

    // Decrypt with private key
    EVP_PKEY_CTX *decrypt_ctx = EVP_PKEY_CTX_new(pkey, NULL);
    if (!decrypt_ctx) handle_openssl_error();

    if (EVP_PKEY_decrypt_init(decrypt_ctx) <= 0) handle_openssl_error();
    if (EVP_PKEY_CTX_set_rsa_padding(decrypt_ctx, RSA_PKCS1_OAEP_PADDING) <= 0) handle_openssl_error();

    size_t decrypted_len = 0;
    if (EVP_PKEY_decrypt(decrypt_ctx, NULL, &decrypted_len, encrypted, encrypted_len) <= 0) handle_openssl_error();

    unsigned char *decrypted = malloc(decrypted_len);
    if (!decrypted) {
        fprintf(stderr, "Failed to allocate memory for decrypted message\n");
        return 1;
    }

    if (EVP_PKEY_decrypt(decrypt_ctx, decrypted, &decrypted_len, encrypted, encrypted_len) <= 0) handle_openssl_error();

    printf("Decrypted Message: %s\n", decrypted);

    // Cleanup
    free(encrypted);
    free(decrypted);
    EVP_PKEY_free(pkey);
    EVP_PKEY_CTX_free(keygen_ctx);
    EVP_PKEY_CTX_free(encrypt_ctx);
    EVP_PKEY_CTX_free(decrypt_ctx);
    BIO_free(pub_bio);
    BIO_free(priv_bio);
    EVP_cleanup();
    ERR_free_strings();

    return 0;
}
