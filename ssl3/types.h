#ifndef BDB_SSL_TYPES
#define BDB_SSL_TYPES

#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef EVP_PKEY *KEY;
typedef char *PATH;

#define RSA_KEYSIZE 4096
#define SHARED_KEYSIZE (size_t)32 //32 byte key for AES 256 (matches with liboqs)
#define SHARED_KEYSIZE_P (size_t *)32
#define PUB_KEYFILE (PATH)"pub.pem"
#define PRIV_KEYFILE (PATH)"priv.pem"

void init_openssl(){
    #ifndef SSL_INIT
        #define SSL_INIT
        ERR_load_crypto_strings();
        OpenSSL_add_all_algorithms();
    #endif
}

#endif //BDB_SSL_TYPES