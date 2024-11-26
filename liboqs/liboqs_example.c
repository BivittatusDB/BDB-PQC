//gcc -o liboqs_example liboqs/liboqs_example.c -loqs -lcrypto -lssl3
#include "liboqs.h"

int main(){
    KEY key=gen_key();
    save_privkey(key, "./");
    save_pubkey(key, "./");
    free(key);

    KEY pub_key = load_pubkey("./");
    KEY priv_key = load_privkey("./");

    oqs_fencrypt(pub_key, "./test.txt");
    unsigned char *decrypted = oqs_fdecrypt(priv_key, "./test.txt");

    printf("%02x, %d\n", decrypted, strlen(decrypted-1)); //may be wrong length due to null binary

    free(pub_key);
    free(priv_key);

    return 0;
}