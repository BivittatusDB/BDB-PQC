#include <stdlib.h>
#include <string.h>

#define b 1600
#define w 64
#define l 6

int exp(int base, int exp) {
    int result = 1;
    while (exp > 0) {
        if (exp % 2 == 1) {
            result = (result * base);
        }
        exp = exp >> 1;
        base = (base * base);
    }
    return result;
}

// Function to create and return the 3D state array A from string S
int*** constructA(const int* S) {
    int*** A = (int***)malloc(5 * sizeof(int**));
    for (int x = 0; x < 5; x++) {
        A[x] = (int**)malloc(5 * sizeof(int*));
        for (int y = 0; y < 5; y++) {
            A[x][y] = (int*)malloc(w * sizeof(int));
        }
    }

    for (int x = 0; x < 5; x++) {
        for (int y = 0; y < 5; y++) {
            for (int z = 0; z < w; z++) {
                int index = w * (5 * y + x) + z;
                A[x][y][z] = S[index];
            }
        }
    }

    return A;
}

// Function to create and return the string S from the 3D state array A
int* constructS(int*** A) {
    int* S = (int*)malloc((b) * sizeof(int));

    for (int x = 0; x < 5; x++) {
        for (int y = 0; y < 5; y++) {
            for (int z = 0; z < w; z++) {
                int index = w * (5 * y + x) + z;
                S[index] = A[x][y][z];
            }
        }
    }
    return S;
}

// NIST FIPS 202; Algorithm 1; Θ(A)
int*** theta(int*** A){
    int** C = (int**)malloc(5*sizeof(int*));
    for (int i = 0; i<5; i++){
        C[i]=(int*)malloc(w*sizeof(int));
    }
    for (int x=0;x<5; x++){
        for (int z=0; z<w; z++){
            C[x][z]=((((A[x][0][z]^A[x][1][z])^A[x][2][z])^A[x][3][z])^A[x][4][z]);
        }
    }

    int** D = (int**)malloc(5*sizeof(int*));
    for (int i = 0; i<5; i++){
        D[i]=(int*)malloc(w*sizeof(int));
    }
    for (int x=0;x<5;x++){
        for (int z=0; z<w; z++){
            D[x][z]=C[((x-1)%5+5)%5][z]^C[(x+1)%5][((z-1)%w+w)%w];
        }
    }

    int*** nA = (int***)malloc(5 * sizeof(int**));
    for (int x = 0; x < 5; x++) {
        nA[x] = (int**)malloc(5 * sizeof(int*));
        for (int y = 0; y < 5; y++) {
            nA[x][y] = (int*)malloc(w * sizeof(int));
        }
    }
    for (int x =0; x<5; x++){
        for (int y; y<5; y++){
            for (int z=0; z<w; z++){
                nA[x][y][z]=A[x][y][z]^D[x][z];
            }
        }
    }
    return nA;
}

// NIST FIPS 202; Algorithm 2; ρ(A)
int*** rho(int*** A){
    int*** nA = (int***)malloc(5 * sizeof(int**));
    for (int x = 0; x < 5; x++) {
        nA[x] = (int**)malloc(5 * sizeof(int*));
        for (int y = 0; y < 5; y++) {
            nA[x][y] = (int*)malloc(w * sizeof(int));
        }
    }
    for (int z=0; z<w;z++){
        nA[0][0][z]=A[0][0][z];
    }
    int x=1, y = 0;
    for (int t=0; t<24;t++){
        for (int z=0; z<w; z++){
            nA[x][y][z]=A[x][y][((z-(t+1)*(t+2)/2)%w+w)%w];
        }
        x,y=y,((2*x+3*y)%5+5)%5;
    }
    return nA;
}

// NIST FIPS 202; Algorithm 3; π(A)
int*** pi(int*** A){
    int*** nA = (int***)malloc(5 * sizeof(int**));
    for (int x = 0; x < 5; x++) {
        nA[x] = (int**)malloc(5 * sizeof(int*));
        for (int y = 0; y < 5; y++) {
            nA[x][y] = (int*)malloc(w * sizeof(int));
        }
    }
    for (int x=0;x<5;x++){
        for (int y=0;y<5;y++){
            for (int z=0; z<w; z++){
                nA[x][y][z]=A[((x+3*y)%5+5)%5][x][z];
            }
        }
    }
    return nA;
}

// NIST FIPS 202; Algorithm 4; χ(A);
int*** chi(int*** A){
    int*** nA = (int***)malloc(5 * sizeof(int**));
    for (int x = 0; x < 5; x++) {
        nA[x] = (int**)malloc(5 * sizeof(int*));
        for (int y = 0; y < 5; y++) {
            nA[x][y] = (int*)malloc(w * sizeof(int));
        }
    }
    for (int x=0; x<5; x++){
        for (int y=0;y<5; y++){
            for (int z=0; z<w; z++){
                nA[x][y][z]=A[x][y][z]^((A[(x+1)%5][y][z]^1)&A[(x+2)%5][y][z]);
            }
        }
    }
    return nA;
}

int* Trunc(int* R, int i){
    int* tR = (int*)malloc(i*sizeof(int));
    for (int j=0; j<i;j++){
        tR[j]=R[j];
    }
    return tR;
}

// NIST FIPS 202; Algorithm 5; rc(t)
int rc(int t){
    if (t%255==0){
        return 1;
    }
    int* R = (int*)malloc(8*sizeof(int));
    for (int i=1; i<(t%255); i++){
        for (int j=0;j<8; j++){
            R[j]=0||R[j];
        }
        R[0]=R[0]^R[8];
        R[4]=R[4]^R[8];
        R[5]=R[5]^R[8];
        R[6]=R[6]^R[8];
        R=Trunc(R,8);
    }
    return R[0];
}

// NIST FIPS 202; Algorithm 6; ι(A)
int*** iota(int*** A, int ir){
    int*** nA = (int***)malloc(5 * sizeof(int**));
    for (int x = 0; x < 5; x++) {
        nA[x] = (int**)malloc(5 * sizeof(int*));
        for (int y = 0; y < 5; y++) {
            nA[x][y] = (int*)malloc(w * sizeof(int));
        }
    }
    for (int x=0; x<5; x++){
        for (int y=0; y<5; y++){
            for (int z=0; z<w; z++){
                nA[x][y][z]=A[x][y][z];
            }
        }
    }
    int* RC = (int*)malloc(w*sizeof(int));
    for (int i=0; i<w; i++){
        RC[i]=0;
    }
    for (int j=0; j<l; j++){
        RC[exp(2,j)-1]=rc(j+(7*ir));
    }
    for (int z=0; z<w; z++){
        nA[0][0][z]=nA[0][0][z]^RC[z];
    }
    return nA;
}

// Keccak Round permutations. NIST FIPS 202; Section 
int*** Rnd(int*** A, int ir){
    int*** nA = iota(chi(pi(rho(theta(A)))), ir);
    return nA;
}