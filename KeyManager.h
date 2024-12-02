#include <stdbool.h>
#include <sys/stat.h>

#ifndef KEYMANAGER_H
#define KEYMANAGER_H

#define __need_fsize

//#define BDB_USE_EXPERIMENTAL //uncomment to test under KEM instead of RSA
#ifdef BDB_USE_EXPERIMENTAL
    #include "liboqs/liboqs.h"
    #define bdb_encrypt oqs_fencrypt
    #define bdb_decrypt oqs_fdecrypt
#else 
    #include "rsa/rsa.h"
    #define bdb_encrypt RSA_fencrypt
    #define bdb_decrypt RSA_fdecrypt
#endif

#include "aes/aes.h"
#include "types.h"

typedef unsigned char key;

bool mkdb(char* db_name){
    if (mkdir(db_name, 0755)==0){
        return true;
    }
    return false;
}


//TODO: edit this to not use sys/stat
bool db_exists(char* db_name){
    struct stat st;
    //check if path exists
    if (stat(db_name, &st)!=0) {
        //if not make the path (return false on fail)
        return mkdb(db_name);
    }
    //check if path is a directory (return false if file)
    if (!S_ISDIR(st.st_mode)) {
        return false;
    }
    return true;
}

bool sym_key_exists(PATH dir){
    PATH filepath = catpath(dir, SYMKEY_FILE);
    FILE *file =fopen(filepath, "r");
    if (!file) {
        fclose(file);
        return false;
    }
    fclose(file);
    return true;
}

int gen_keys(PATH dir){
    if (sym_key_exists(dir)){
        remove(catpath(dir, SYMKEY_FILE));
    }
    KEY keys = gen_key();
    save_pubkey(keys, dir);
    save_privkey(keys, dir);
    return 0;
}

int key_check(PATH dir){
    FILE *privkeyfile = fopen(catpath(dir, PRIV_KEYFILE), "r");
    FILE *pubkeyfile = fopen(catpath(dir, PUB_KEYFILE), "r");
    if ((privkeyfile==NULL) && (pubkeyfile==NULL)){
        gen_keys(dir);
        key_check(dir);
    }
    return 0;
}


#endif // KEYMANAGER_H