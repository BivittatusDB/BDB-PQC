#ifndef __KEM512_H__
#define __KEM512_H__

#include "constants.h"
#include "random.h"
#include "ntt.h"
#include "sha3.h"

// NIST Constants for KEM-512 (FIPS 203 Table 2)
const int k  = 2;
const int n1 = 3;
const int n2 = 2;
const int du = 10;
const int dv = 4;

const int RBG_strength = 128;

// ENCRYPTION: USED FOR ENCRYPTION AND DECRYPTION (FIPS 203; Section 5)
// NIST FIPS 203; Algorithm 13
typedef struct {
    int* publicKey;
    int* privateKey;
} KeyPair;

KeyPair KeyGen(int* d){
    d[33]=k;
    int** split_digest = G(d);
    int* rho = split_digest[0];
    int* sigma = split_digest[1];
    int n = 0;

    int ***A = (int ***)malloc(k*sizeof(int**));
    for (int i=0; i<k; i++){
        A[i]=(int **)malloc(k*sizeof(int*));
        for (int j=0; j<k;j++){
            rho[33]=j;
            rho[34]=i;
            A[i][j]=SampleNTT(rho, 34);
        }
    }
    int** s = (int **)malloc(k*sizeof(int*));
    for (int i=0; i<k;i++){
        int* digest=PRF(sigma, n, n1);
        s[i]=SamplePolyCBD((uint8_t *)digest, n1, n);
        n++;
    }
    int** e = (int **)malloc(k*sizeof(int*));
    for (int i =0; i<k; i++){
        int* digest = PRF(sigma, n, n1);
        e[i]=SamplePolyCBD((uint8_t *)digest, n1, n);
        n++;
    }
    
}
// NIST FIPS 203; Algorithm 14
int* Encrypt(const int* ek, const int* m, const int* r);
// NIST FIPS 203; Algorithm 15
int* Decrypt(const int* dk, const int* c);

// INTERNALS: USED FOR TESTING (FIPS 203; Section 6)
// NIST FIPS 203; Algorithm 16
KeyPair KeyGen_internal(const int* d, const int* z);
// NIST FIPS 203; Algorithm 17
int* Encaps_internal(const int* ek, const int* m);
// NIST FIPS 203; Algorithm 18
int* Decaps_internal(const int* dk, const int* c);

// ML-KEM: USED FOR RANDOMNESS AND TESTING (FIPS 203; Section 7)
// NIST FIPS 203; Algorithm 19
KeyPair ML_KeyGen();
// NIST FIPS 203; Algorithm 20
int* Encapsulation(const int* ek);
// NIST FIPS 203; Algorithm 21
int* Decapsulation(const int* dk, const int* c);

#endif // __KEM512_H__
