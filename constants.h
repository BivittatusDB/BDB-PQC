#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#define N 256
#define Q 3329
#define zeta 17
#define Z zeta

// Verifies that the defined values are valid
#if N <= 0
    #error "Error: The value of N must be greater than 0."
#endif

#if Q <= 0
    #error "Error: The value of Q must be greater than 0."
#endif

#if zeta <= 0
    #error "Error: The value of zeta must be greater than 0."
#endif

typedef struct{
    int a; 
    int b;
} binomial; //for a binomial ax+b

typedef struct{
    int* ek; //encapsulation key
    int* dk; //decapsulation key
} KeyPair;

#endif // __CONSTANTS_H__