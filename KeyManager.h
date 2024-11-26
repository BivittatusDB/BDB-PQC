#include <stdbool.h>
#include <sys/stat.h>

#ifndef KEYMANAGER_H
#define KEYMANAGER_H

//#define BDB_USE_EXPERIMENTAL //uncomment to test under KEM instead of RSA

#ifdef BDB_USE_EXPERIMENTAL
    #include "liboqs/liboqs.h"
#else 
    #include "rsa/rsa.h"
#endif

typedef unsigned char key;

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

bool mkdb(char* db_name){
    if (mkdir(db_name, 0755)==0){
        return true;
    }
    return false;
}

int gen_keys(PATH dir){
    KEY keys = gen_key();
    save_pubkey(keys, dir);
    save_privkey(keys, dir);
    return 0;
}

KEY load_public_key(PATH dir){
    return load_pubkey(dir);
}
KEY load_private_key(PATH dir){
    return load_privkey(dir);
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