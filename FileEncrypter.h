#ifndef BDB_FILE_ENCRYPTER_H
#define BDB_FILE_ENCRYPTER_H

#include "KeyManager.h"

void ensure_shared_key(KEY pubkey, PATH path){
    PATH filepath=catpath(path, SYMKEY_FILE);
    if (key_check(path)==1){
        gen_keys(path);
    };
    FILE *sym_file;
    if (!(sym_file = fopen(filepath, "rb"))){
        bdb_encrypt(pubkey, filepath, NULL);
    }
}

int ensure_file(PATH filepath){
    FILE *file = fopen(filepath, "rb");
    if (file==NULL) return 1;
    fseek(file, 0, SEEK_END);
    if (ftell(file)==0) return 1;
    fclose(file);
    return 0;
}

int encrypt_file(PATH dir, PATH filename, KEY privkey){
    //use the private key to decrypt the symmetric key which decrypts the file
    PATH filepath = catpath(dir, filename);
    if (ensure_file(filepath)!=0) return 1;
    KEY_t symkey = bdb_decrypt(privkey, catpath(dir, SYMKEY_FILE));
    if (aes_fencrypt(symkey, filepath) != 0) return 1;
    return 0;
}

int decrypt_file(PATH dir, PATH filename, KEY privkey){
    PATH filepath = catpath(dir, filename);
    if (ensure_file(filepath)!=0) return 1;
    KEY_t symkey = bdb_decrypt(privkey, catpath(dir, SYMKEY_FILE));
    if (aes_fdecrypt(symkey, filepath)!=0) return 1;
    return 0;
}

#endif