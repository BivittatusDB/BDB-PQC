#ifndef BDB_AES_H
#define BDB_AES_H

#include "types.h"

#include <stdlib.h>
#include <string.h>
#include <openssl/rand.h>
#include <openssl/evp.h>


ctxt aes_encrypt(unsigned char *plaintext, int plaintext_len, KEY_t key) {
    EVP_CIPHER_CTX *ctx; 
    int len;

    // Allocate and initialize the ciphertext structure
    ctxt ciphertxt = malloc(sizeof(__ciphertext));
    if (!ciphertxt) return NULL;

    ciphertxt->iv = malloc(AES_IVSIZE);
    if (!ciphertxt->iv || !RAND_bytes(ciphertxt->iv, AES_IVSIZE)) {
        free(ciphertxt);
        return NULL;
    }

    ciphertxt->ciphertext = malloc(plaintext_len + AES_BLOCKSIZE);
    if (!ciphertxt->ciphertext) {
        free(ciphertxt->iv);
        free(ciphertxt);
        return NULL;
    }

    ciphertxt->tag = malloc(AES_TAGSIZE);
    if (!ciphertxt->tag) {
        free(ciphertxt->iv);
        free(ciphertxt->ciphertext);
        free(ciphertxt);
        return NULL;
    }

    if (!(ctx = EVP_CIPHER_CTX_new())) {
        free(ciphertxt->iv);
        free(ciphertxt->ciphertext);
        free(ciphertxt->tag);
        free(ciphertxt);
        return NULL;
    }

    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, key, ciphertxt->iv) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        free(ciphertxt->iv);
        free(ciphertxt->ciphertext);
        free(ciphertxt->tag);
        free(ciphertxt);
        return NULL;
    }

    if (EVP_EncryptUpdate(ctx, ciphertxt->ciphertext, &len, plaintext, plaintext_len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        free(ciphertxt->iv);
        free(ciphertxt->ciphertext);
        free(ciphertxt->tag);
        free(ciphertxt);
        return NULL;
    }
    ciphertxt->ciphertext_len = len;

    if (EVP_EncryptFinal_ex(ctx, ciphertxt->ciphertext + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        free(ciphertxt->iv);
        free(ciphertxt->ciphertext);
        free(ciphertxt->tag);
        free(ciphertxt);
        return NULL;
    }
    ciphertxt->ciphertext_len += len;

    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, AES_TAGSIZE, ciphertxt->tag) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        free(ciphertxt->iv);
        free(ciphertxt->ciphertext);
        free(ciphertxt->tag);
        free(ciphertxt);
        return NULL;
    }

    EVP_CIPHER_CTX_free(ctx);
    return ciphertxt;
}

unsigned char *aes_decrypt(ctxt ciphertxt, KEY_t key, int *plaintext_len) {
    EVP_CIPHER_CTX *ctx; 
    int len;
    unsigned char *plaintext = malloc(ciphertxt->ciphertext_len);

    if (!plaintext){
        perror("Allocating Ciphertext");
        return NULL;
    }

    if (!(ctx = EVP_CIPHER_CTX_new())) {
        perror("Making Context");
        free(plaintext);
        return NULL;
    }

    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, key, ciphertxt->iv) != 1) {
        perror("Initializing");
        EVP_CIPHER_CTX_free(ctx);
        free(plaintext);
        return NULL;
    }

    if (EVP_DecryptUpdate(ctx, plaintext, &len, ciphertxt->ciphertext, ciphertxt->ciphertext_len) != 1) {
        perror("Updating");
        EVP_CIPHER_CTX_free(ctx);
        free(plaintext);
        return NULL;
    }
    *plaintext_len = len;

    if (EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, AES_TAGSIZE, ciphertxt->tag) != 1) {
        perror("getting tag");
        EVP_CIPHER_CTX_free(ctx);
        free(plaintext);
        return NULL;
    }
    if (EVP_DecryptFinal_ex(ctx, plaintext + len, &len)!= 1) {
        perror("finalizing");
        EVP_CIPHER_CTX_free(ctx);
        free(plaintext);
        return NULL;
    }
    *plaintext_len += len;

    EVP_CIPHER_CTX_free(ctx);
    return plaintext;
}

#ifdef __need_fsize
size_t fsize(FILE *file) {
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);
    return size;
}
#undef __need_fize
#endif

int aes_fencrypt(KEY_t key, PATH filepath) {
    FILE *file_r = fopen(filepath, "rb");
    if (!file_r) return -1;

    size_t f_size = fsize(file_r);
    unsigned char *buffer = malloc(f_size);
    fread(buffer, 1, f_size, file_r);
    fclose(file_r);

    ctxt ciphertxt = aes_encrypt(buffer, f_size, key);
    free(buffer);
    if (!ciphertxt) return -1;

    FILE *file_w = fopen(filepath, "wb");
    if (!file_w) {
        free(ciphertxt->iv);
        free(ciphertxt->ciphertext);
        free(ciphertxt->tag);
        free(ciphertxt);
        return -1;
    }

    fwrite(&ciphertxt->ciphertext_len, sizeof(ciphertxt->ciphertext_len), 1, file_w);
    fwrite(ciphertxt->iv, 1, AES_IVSIZE, file_w);
    fwrite(ciphertxt->ciphertext, 1, ciphertxt->ciphertext_len, file_w);
    fwrite(ciphertxt->tag, 1, AES_TAGSIZE, file_w);
    fclose(file_w);

    free(ciphertxt->iv);
    free(ciphertxt->ciphertext);
    free(ciphertxt->tag);
    free(ciphertxt);

    return 0;
}

int aes_fdecrypt(KEY_t key, PATH filepath) {
    FILE *file_r = fopen(filepath, "rb");
    if (!file_r) return 1;

    ctxt ciphertxt = malloc(sizeof(__ciphertext));
    if (!ciphertxt) {
        fclose(file_r);
        return 2;
    }

    fread(&ciphertxt->ciphertext_len, sizeof(ciphertxt->ciphertext_len), 1, file_r);

    ciphertxt->iv = malloc(AES_IVSIZE);
    ciphertxt->ciphertext = malloc(ciphertxt->ciphertext_len);
    ciphertxt->tag = malloc(AES_TAGSIZE);

    if (!ciphertxt->iv || !ciphertxt->ciphertext || !ciphertxt->tag) {
        fclose(file_r);
        free(ciphertxt->iv);
        free(ciphertxt->ciphertext);
        free(ciphertxt->tag);
        free(ciphertxt);
        return 3;
    }

    fread(ciphertxt->iv, 1, AES_IVSIZE, file_r);
    fread(ciphertxt->ciphertext, 1, ciphertxt->ciphertext_len, file_r);
    fread(ciphertxt->tag, 1, AES_TAGSIZE, file_r);
    fclose(file_r);

    int plain_len;
    unsigned char *buffer = aes_decrypt(ciphertxt, key, &plain_len);

    if (!buffer) {
        free(ciphertxt->iv);
        free(ciphertxt->ciphertext);
        free(ciphertxt->tag);
        free(ciphertxt);
        return 4;
    }

    FILE *file_w = fopen(filepath, "wb");
    if (!file_w) {
        free(buffer);
        free(ciphertxt->iv);
        free(ciphertxt->ciphertext);
        free(ciphertxt->tag);
        free(ciphertxt);
        return 5;
    }

    fwrite(buffer, 1, plain_len, file_w);
    fclose(file_w);

    free(buffer);
    free(ciphertxt->iv);
    free(ciphertxt->ciphertext);
    free(ciphertxt->tag);
    free(ciphertxt);

    return 0;
}

#endif // BDB_AES_H
