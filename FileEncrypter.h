#ifndef BDB_FILE_ENCRYPTER_H
#define BDB_FILE_ENCRYPTER_H

#include "KeyManager.h"

int encrypt_file(PATH filepath){
    FILE *file = fopen(filepath, "rb");
    if (file==NULL) return 1;
    fseek(file, 0, SEEK_END);
    if (ftell(file)==0) return 1;
    rewind(file);
    
}

#endif