//https://stackoverflow.com/questions/51144505/generate-sha-3-hash-in-c-using-openssl-library
//TEST COMPILE: gcc -o sha3.exe sha3.c -lssl -lcrypto && sha3.exe

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/evp.h>

char *sha3_256(const unsigned char *input, size_t inlen) {
    unsigned char *output = (unsigned char *)malloc(32);
    if (output == NULL) {
        return NULL; 
    }

    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    const EVP_MD *md = EVP_sha3_256();
    EVP_DigestInit_ex(mdctx, md, NULL);
    EVP_DigestUpdate(mdctx, input, inlen);
    EVP_DigestFinal_ex(mdctx, output, NULL);

    // Free the context
    EVP_MD_CTX_free(mdctx);

    return (char *)output;
}

char *sha3_512(const unsigned char *input, size_t inlen) {
    unsigned char *output = (unsigned char *)malloc(64);
    if (output == NULL) {
        return NULL; 
    }
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    const EVP_MD *md = EVP_sha3_512();
    EVP_DigestInit_ex(mdctx, md, NULL);
    EVP_DigestUpdate(mdctx, input, inlen);
    EVP_DigestFinal_ex(mdctx, output, NULL);

    // Free the context
    EVP_MD_CTX_free(mdctx);
    return (char *)output;
}


// will need to split into XOF
char *shake128(const unsigned char *input, size_t inlen, size_t outlen) {
    unsigned char *output = (unsigned char *)malloc(outlen);
    if (output == NULL) {
        return NULL; 
    }
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    const EVP_MD *md = EVP_shake128();
    EVP_DigestInit_ex(mdctx, md, NULL);
    EVP_DigestUpdate(mdctx, input, inlen);
    EVP_DigestFinalXOF(mdctx, output, outlen);

    // Free the context
    EVP_MD_CTX_free(mdctx); 
    return (char *)output;
}

char *shake256(const unsigned char *input, size_t inlen,size_t outlen) {
    unsigned char *output = (unsigned char *)malloc(outlen);
    if (output == NULL) {
        return NULL; 
    }
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    const EVP_MD *md = EVP_shake256();
    EVP_DigestInit_ex(mdctx, md, NULL);
    EVP_DigestUpdate(mdctx, input, inlen);
    EVP_DigestFinalXOF(mdctx, output, outlen);

    // Free the context
    EVP_MD_CTX_free(mdctx); 
    return (char *)output;
}

//ALGORITHMS NEEDED:
/*
1. H: SHA3-256
2. J: SHAKE256
3. G: SHA3-512
4: XOF: SHAKE128
*/

#define H sha3_256
#define J shake256

int *PRF(int* s, int b, int eta){
    s[33] = b;
    int* digest = (int *)shake256((const unsigned char *)s, 33, 256*eta);
    return digest;
}

//SPLIT SHA3-512 ouput (64 bytes into two 32-byte outputs)
int** G(int* input){
    int* digest = (int *)sha3_512((const unsigned char *)input, 33);

    int** output = (int **)malloc(2*sizeof(int*));
    output[0] = (int *)malloc(32);
    output[1] = (int *)malloc(32);
    for (int i=0; i<32; i++){
        output[0][i]=digest[i];
        output[1][i]=digest[i+32];
    }
    return output;
}

//SHAKE128 as an XOF 
EVP_MD_CTX *XOF_init(){
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    const EVP_MD *md = EVP_shake128();
    EVP_DigestInit_ex(mdctx, md, NULL);
    return mdctx;
}

void XOF_Absorb(EVP_MD_CTX *mdctx, char *input, size_t inlen){
    EVP_DigestUpdate(mdctx, input, inlen);
}

char *XOF_Squeeze(EVP_MD_CTX *mdctx, size_t outlen){
    unsigned char *output = (unsigned char *)malloc(outlen);
    if (output == NULL) {
        return NULL; 
    }
    EVP_DigestFinalXOF(mdctx, output, outlen);
    return (char *)output;
} 

//test print for SHAKE256, SHA3-256, SHA3-512
int print_digest() {
    const char *message = "Hello, world!";
    int outlen = 32; //change length here
    char *output = shake256((const unsigned char *)message, strlen(message), outlen); //change function here

    // Print the hash in hexadecimal format
    for (int i = 0; i < 32; i++) {
        printf("%02x", (unsigned char)output[i]);
    }
    printf("\n");

    return 0;
}

int print_XOF() {
    char *message = "Hello, world!";
    int outlen = 32;  //change length here
    EVP_MD_CTX *ctx = XOF_init();
    XOF_Absorb(ctx, message, strlen(message));
    char *output = XOF_Squeeze(ctx, outlen);
    
    // Print the hash in hexadecimal format
    for (int i = 0; i < 32; i++) {
        printf("%02x", (unsigned char)output[i]);
    }
    printf("\n");

    return 0;
}