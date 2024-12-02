//gcc -o examples/liboqs_example liboqs/liboqs_example.c -loqs -lcrypto -lssl3
#include "liboqs.h"

#define __need_fsize

int main(){
    KEY key=gen_key();
    save_privkey(key, "./");
    save_pubkey(key, "./");
    free(key);

    KEY pub_key = load_pubkey("./");
    KEY priv_key = load_privkey("./");

    oqs_fencrypt(pub_key, "./test.txt", NULL);
    unsigned char *decrypted = oqs_fdecrypt(priv_key, "./test.txt");

    printf("%02x\n", decrypted);

    free(pub_key);
    free(priv_key);

    return 0;
}