#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#define N 256       
#define Q 3329      
#define zeta 17     

#if !defined(N) || N <= 0
    #error ‘Error: The value of N must be defined and greater than 0.’
#endif

#if !defined(Q) || Q <= 0
    #error ‘Error: The value of Q must be defined and greater than 0.’
#endif

#if !defined(zeta) || zeta <= 0
    #error ‘Error: The value of zeta must be defined and greater than 0.’
#endif

// Structure representing a binomial expression of the form ax + b
typedef struct {
    int a; 
    int b;
} binomial;

// Structure for storing a key pair, including encapsulation and decapsulation keys
typedef struct {
    int* ek; // encapsulation key (pointer to an array of integers)
    int* dk; // decapsulation key (pointer to an array of integers)
} KeyPair;

#endif // __CONSTANTS_H__