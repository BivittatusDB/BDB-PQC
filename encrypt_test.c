//#define BDB_USE_EXPERIMENTAL //make sure this is defined before imports

#include "FileEncrypter.h"

#include "unistd.h"

int main(){
    if (gen_keys(LOCAL)!=0) return 1;
    KEY pub = load_pubkey(LOCAL);
    KEY priv = load_privkey(LOCAL);
    ensure_shared_key(pub, LOCAL);
    if (encrypt_file(LOCAL, "test.txt", priv)!=0) return 2;
    sleep(10);
    if (decrypt_file(LOCAL, "test.txt", priv)!=0) return 3;
    return 0;
}
