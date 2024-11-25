#include "rsa.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main() {
    // Generate RSA key pair
    EVP_PKEY *keypair = gen_key(4096);

    // Save keys to PEM files
    save_pubkey(keypair, "./");
    save_privkey(keypair, "./");
    EVP_PKEY_free(keypair);

    // Load keys from PEM files
    EVP_PKEY *pub_key = load_pubkey("./");
    EVP_PKEY *priv_key = load_privkey("./");

    RSA_fencrypt(pub_key, "./test.txt");
    sleep(10);
    RSA_fdecrypt(priv_key, "./test.txt");

    EVP_PKEY_free(pub_key);
    EVP_PKEY_free(priv_key);

    return 0;
}
