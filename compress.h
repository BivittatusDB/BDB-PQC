//Compression algorithms 
// NIST FIPS 203; section 4.2.1 "Compression and decompression"
#include "constants.h"
#include <math.h>

int Compress(int d, int x){
    int mod= pow(2,d);
    int coef = round(mod/Q);
    int y = (coef*x)%mod;
    return y;
}

int decompress(int d, int y){
    int mod=pow(2,d);
    int coef = round(Q/mod);
    int x = (coef*y);
    return x;
}