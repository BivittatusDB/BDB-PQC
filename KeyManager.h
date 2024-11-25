#include <stdbool.h>
#include <sys/stat.h>

#ifndef KEYMANAGER_H
#define KEYMANAGER_H

#ifdef BDB_USE_EXPERIMENTAL
    #include "liboqs/types.h"
#else 
    #include "ssl3/types.h"
#endif


#define PRIVKEYFILE "/private.pem"
#define PUB_KEYFILE "/public.pem"

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

int gen_keys();
key* load_public_key();
key* load_private_key();
bool verify_keypair();
int key_check();


#endif // KEYMANAGER_H