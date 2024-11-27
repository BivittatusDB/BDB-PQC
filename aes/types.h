#ifndef BDB_AES_TYPES
#define BDB_AES_TYPES

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

//all for AES-256 GCM
#define AES_KEYSIZE 32
#define AES_BLOCKSIZE 16
#define AES_TAGSIZE 16
#define AES_IVSIZE 12

typedef uint8_t *KEY_t;
typedef char *PATH;
typedef EVP_CIPHER_CTX *CTX;

typedef struct {
    unsigned char *ciphertext;
    unsigned char *iv;
    unsigned char *tag;
    size_t ciphertext_len;
} __ciphertext;

typedef __ciphertext *ctxt;

#endif //BDB_AES_TYPES