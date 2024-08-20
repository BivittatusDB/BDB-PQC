#include "keccak.h"

// Function to concatenate two int* arrays
int* concatenate_int_arrays(const int* arr1, int len1, const int* arr2, int len2) {
    // Allocate memory for the concatenated array
    int* result = (int*)malloc((len1 + len2) * sizeof(int));
    if (result == NULL) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }

    // Copy the first array into the result
    memcpy(result, arr1, len1 * sizeof(int));

    // Copy the second array into the result
    memcpy(result + len1, arr2, len2 * sizeof(int));

    return result;
}

int* suffix(){
    int* suf = (int*)malloc(2*sizeof(int));
    suf[0], suf[1] = 0, 1;
    return suf;
}

int* xof_suffix(){
    int* suf = (int*)malloc(2*sizeof(int));
    suf[0], suf[1]=1;
    return suf;
}

// NEEDED: SHAKE128, SHAKE256, SHA3-256, SHA3-512

// SHA3-256
int* SHA256_absorb(int* M, size_t lenM){
    int* nM = concatenate_int_arrays(M,lenM,suffix(),2);
    return Absorb(nM,b-512);
}

int* SHA256_squeeze(int* S){
    return Squeeze(S,256,b-512);
}

// SHA3-512
int* SHA512_absorb(int* M, size_t lenM){
    int* nM = concatenate_int_arrays(M,lenM,suffix(),2);
    return Absorb(nM,b-1024);
}

int* SHA512_squeeze(int* S){
    return Squeeze(S,512,b-1024);
}

// SHAKE128
int* SHAKE128_absorb(int* M, size_t lenM){
    int* nM = concatenate_int_arrays(M,lenM, xof_suffix(), 2);
    return Absorb(nM,b-256);
}

int* SHAKE128_squeeze(int* S, size_t d){
    return Squeeze(S, d, b-256);
}

// SHAKE256
int* SHAKE256_absorb(int* M, size_t lenM){
    int* nM = concatenate_int_arrays(M,lenM, xof_suffix(), 2);
    return Absorb(nM,b-512);
}

int* SHAKE256_squeeze(int* S, size_t d){
    return Squeeze(S, d, b-512);
}