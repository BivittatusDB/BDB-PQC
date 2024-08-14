#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#define N 256
#define Q 3329
#define zeta 17
#define Z zeta

int BitRev7(int r){
    unsigned int res = 0;
    for (int i = 0; i<7;i++){
        res <<= 1;
        res |= (r&1);
        r >>= 1;
    }
    return res;
}


#endif //__COSNTANTS_H__