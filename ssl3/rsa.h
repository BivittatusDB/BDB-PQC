#include "types.h"

PATH catpath(PATH path1, PATH path2){
    PATH full_path = (PATH )malloc(PATH_MAX);
    snprintf(full_path, PATH_MAX, "%s%s", path1, path2);
    return full_path;
}

void handle_openssl_error() {
    ERR_print_errors_fp(stderr);
    abort();
}

KEY gen_key() {
    KEY pkey = NULL;
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);

    if (!ctx) handle_openssl_error();
    if (EVP_PKEY_keygen_init(ctx) <= 0) handle_openssl_error();
    if (EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, RSA_KEYSIZE) <= 0) handle_openssl_error();
    if (EVP_PKEY_keygen(ctx, &pkey) <= 0) handle_openssl_error();

    EVP_PKEY_CTX_free(ctx);
    return pkey;
}

int save_pubkey(KEY key, PATH dir) {
    FILE *file = fopen(catpath(dir, PUB_KEYFILE), "wb");
    if (!file) {
        perror("Failed to open public key file");
        return 0;
    }
    if (!PEM_write_PUBKEY(file, key)) {
        fclose(file);
        handle_openssl_error();
    }
    fclose(file);
    return 1;
}

int save_privkey(KEY key, char *dir) {
    FILE *file = fopen(catpath(dir, PRIV_KEYFILE), "wb");
    if (!file) {
        perror("Failed to open private key file");
        return 0;
    }
    if (!PEM_write_PrivateKey(file, key, NULL, NULL, 0, NULL, NULL)) {
        fclose(file);
        handle_openssl_error();
    }
    fclose(file);
    return 1;
}

KEY load_pubkey(PATH dir) {
    FILE *file = fopen(catpath(dir, PUB_KEYFILE), "rb");
    if (!file) {
        perror("Failed to open public key file");
        return NULL;
    }
    KEY key = PEM_read_PUBKEY(file, NULL, NULL, NULL);
    fclose(file);
    if (!key) handle_openssl_error();
    return key;
}

KEY load_privkey(PATH dir) {
    FILE *file = fopen(catpath(dir, PRIV_KEYFILE), "rb");
    if (!file) {
        perror("Failed to open private key file");
        return NULL;
    }
    KEY key = PEM_read_PrivateKey(file, NULL, NULL, NULL);
    fclose(file);
    if (!key) handle_openssl_error();
    return key;
}

unsigned char *rsa_encrypt(KEY pub_key, const unsigned char *plaintext, size_t plaintext_len, size_t *encrypted_len) {
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(pub_key, NULL);
    if (!ctx) handle_openssl_error();

    if (EVP_PKEY_encrypt_init(ctx) <= 0) handle_openssl_error();
    if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING) <= 0) handle_openssl_error();

    // Determine buffer size for encrypted data
    if (EVP_PKEY_encrypt(ctx, NULL, encrypted_len, plaintext, plaintext_len) <= 0) handle_openssl_error();

    unsigned char *encrypted = malloc(*encrypted_len);
    if (!encrypted) {
        perror("Failed to allocate memory for encryption");
        EVP_PKEY_CTX_free(ctx);
        return NULL;
    }

    // Perform encryption
    if (EVP_PKEY_encrypt(ctx, encrypted, encrypted_len, plaintext, plaintext_len) <= 0) {
        free(encrypted);
        EVP_PKEY_CTX_free(ctx);
        handle_openssl_error();
    }

    EVP_PKEY_CTX_free(ctx);
    return encrypted;
}

unsigned char *rsa_decrypt(KEY priv_key, const unsigned char *ciphertext, size_t ciphertext_len, size_t *decrypted_len) {
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(priv_key, NULL);
    if (!ctx) handle_openssl_error();

    if (EVP_PKEY_decrypt_init(ctx) <= 0) handle_openssl_error();
    if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING) <= 0) handle_openssl_error();

    // Determine buffer size for decrypted data
    if (EVP_PKEY_decrypt(ctx, NULL, decrypted_len, ciphertext, ciphertext_len) <= 0) handle_openssl_error();

    unsigned char *decrypted = malloc(*decrypted_len);
    if (!decrypted) {
        perror("Failed to allocate memory for decryption");
        EVP_PKEY_CTX_free(ctx);
        return NULL;
    }

    // Perform decryption
    if (EVP_PKEY_decrypt(ctx, decrypted, decrypted_len, ciphertext, ciphertext_len) <= 0) {
        free(decrypted);
        EVP_PKEY_CTX_free(ctx);
        handle_openssl_error();
    }

    EVP_PKEY_CTX_free(ctx);
    return decrypted;
}

size_t fsize(FILE *file){
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);
    return size;
}

int RSA_fencrypt(KEY pub_key, PATH filepath){
    FILE *infile = fopen(filepath, "rb");
    if (!infile) return 1;
    size_t size=fsize(infile);
    unsigned char *plaintext = (unsigned char *)malloc(size);
    if (!plaintext) return 1;
    fread(plaintext, 1, size, infile);
    fclose(infile);
    size_t enc_len;
    unsigned char *encrypted = rsa_encrypt(pub_key, plaintext, size, &enc_len);
    FILE *outfile = fopen(filepath, "wb");
    if (!outfile) return 1;
    fwrite(encrypted, 1, enc_len, outfile);
    fclose(outfile);
    return 0;
}

int RSA_fdecrypt(KEY priv_key, PATH filepath){
    FILE *infile = fopen(filepath, "rb");
    if (!infile) return 1;
    size_t size = fsize(infile);
    unsigned char *ciphertext = (unsigned char *)malloc(size);
    if (!ciphertext) return 1;
    fread(ciphertext, 1, size, infile);
    fclose(infile);
    size_t dec_len;
    unsigned char *decrypted = rsa_decrypt(priv_key, ciphertext, size, &dec_len);
    FILE *outfile = fopen(filepath, "wb");
    if (!outfile) return 1;
    fwrite(decrypted, 1, dec_len, outfile);
    fclose(outfile);
    return 0;
}