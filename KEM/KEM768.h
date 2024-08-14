#ifndef __KEM768_H__
#define __KEM768_H__

#include "constants.h"

//NIST Constants for KEM-768 (FIPS 203 Table 2)
int k  = 3;
int n1 = 2;
int n2 = 2;
int du = 10;
int dv = 4;

int RBG_strength = 192;

// ENCRYPTION: USED FOR ENCRYPTION AND DECRYPTION (FIPS 203; Section 5)
// NIST FIPS 203; Algoritm 13
KeyPair KeyGen(int* d);
// NIST FIPS 203; Algoritm 14
int* Encrypt(int* ek, int* m, int* r);
// NIST FIPS 203; Algoritm 15
int* Decrypt(int* dk, int* c);

// INTERNALS: USED FOR TESTING (FIPS 203; Section 6)
// NIST FIPS 203; Algoritm 16
KeyPair KeyGen_internal(int* d, int* z);
// NIST FIPS 203; Algoritm 17
int* Encaps_internal(int* ek, int* m);
// NIST FIPS 203; Algoritm 18
int* Decaps_internal(int* dk, int* c);

// ML-KEM: USED FOR RANDOMNESS AND TESTING (FIPS 203; Section 7)
// NIST FIPS 203; Algoritm 19
KeyPair ML_KeyGen();
// NIST FIPS 203; Algoritm 20
int* Encapsulation(int* ek);
// NIST FIPS 203; Algoritm 21
int* Decapsulation(int* dk, int* c);

#endif //__KEM768_H__