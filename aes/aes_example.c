//gcc -o aes_example aes/aes_example.c -lcrypto -lssl3

#include "aes.h"

int main(){
    KEY_t key;
    if (!(RAND_bytes(key, sizeof(KEY_t)))) return 1;
    aes_fencrypt(key, "./test.txt");
    aes_fdecrypt(key, "./test.txt");
    return 0;
}