#ifndef BDB_OQS_TYPES
#define BDB_OQS_TYPES

#include <oqs/oqs.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <openssl/evp.h>

#define b64encode EVP_EncodeBlock
#define b64decode EVP_DecodeBlock

typedef uint8_t *KEY_t;
typedef OQS_KEM *KEY_CTX;
typedef char *PATH;

typedef struct {
    KEY_CTX ctx;
    KEY_t pub_key;
    KEY_t priv_key;
} KEY_STRUCT;

typedef KEY_STRUCT *KEY;

#define KEYSIZE OQS_KEM_alg_kyber_1024
#define SHARED_KEYSIZE (size_t)32 //32 byte key for AES 256 (matches with liboqs)
#define PUB_KEYFILE (PATH)"pub.pem"
#define PRIV_KEYFILE (PATH)"priv.pem"

#define PEM_HEADER_PUB "--------BEGIN PUBLIC KEY--------\n"
#define PEM_FOOTER_PUB "\n---------END PUBLIC KEY---------"
#define PEM_HEADER_PRIV "-------BEGIN PRIVATE KEY--------\n"
#define PEM_FOOTER_PRIV "\n--------END PRIVATE KEY---------"
#define PEM_HF_SIZE (size_t)33 //size of header/footer (32 + newline)

//no init needed for liboqs

#endif //BDB_OQS_TYPES