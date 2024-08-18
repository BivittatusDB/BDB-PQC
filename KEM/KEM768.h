#ifndef __KEM768_H__
#define __KEM768_H__

#include "constants.h"

// NIST Constants for KEM-768 (FIPS 203 Table 2)
const int k  = 3;
const int n1 = 2;
const int n2 = 2;
const int du = 10;
const int dv = 4;

const int RBG_strength = 192;

// ENCRYPTION: USED FOR ENCRYPTION AND DECRYPTION (FIPS 203; Section 5)
// NIST FIPS 203; Algorithm 13
typedef struct {
    int* publicKey;
    int* privateKey;
} KeyPair;

KeyPair KeyGen(const int* d);
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

#endif // __KEM768_H__
