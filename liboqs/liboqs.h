#ifndef BDB_OQS_H
#define BDB_OQS_H

#include "types.h"

PATH catpath(PATH path1, PATH path2){
    PATH full_path = (PATH )malloc(PATH_MAX);
    snprintf(full_path, PATH_MAX, "%s%s", path1, path2);
    return full_path;
}

size_t fsize(FILE *file){
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);
    return size;
}

KEY gen_key(){
    KEY_CTX kem = OQS_KEM_new(KEYSIZE);
    uint8_t *public_key = malloc(kem->length_public_key);
    uint8_t *secret_key = malloc(kem->length_secret_key);
    uint8_t *ciphertext = malloc(kem->length_ciphertext);
    uint8_t *shared_secret_enc = malloc(kem->length_shared_secret);
    uint8_t *shared_secret_dec = malloc(kem->length_shared_secret);
    if (!public_key || !secret_key || !ciphertext || !shared_secret_enc || !shared_secret_dec) return NULL;
    if (OQS_KEM_keypair(kem, public_key, secret_key)!=OQS_SUCCESS) return NULL;
    KEY key = (KEY)malloc(sizeof(KEY));
    key->ctx = kem;
    key->pub_key = public_key;
    key->priv_key = secret_key;
    return key;
}

int save_pubkey(KEY pubkey, PATH dir){
    FILE *file = fopen(catpath(dir, PUB_KEYFILE), "wb");
    if (!file) return 1;
    fwrite(PEM_HEADER_PUB,1, PEM_HF_SIZE, file);
    size_t key_size=pubkey->ctx->length_public_key;
    KEY_t key = pubkey->pub_key;
    size_t enc_size = 4*((key_size+2)/3);
    char *encoded=(char *)malloc(enc_size);
    b64encode((unsigned char *)encoded, key, (int)key_size);
    fwrite(encoded,1,enc_size, file);
    fwrite(PEM_FOOTER_PUB,1, PEM_HF_SIZE, file);
    fclose(file);
    free(encoded);
    return 0;
}

int save_privkey(KEY privkey, PATH dir){
    FILE *file = fopen(catpath(dir, PRIV_KEYFILE), "wb");
    if (!file) return 1;
    fwrite(PEM_HEADER_PRIV,1, PEM_HF_SIZE, file);
    size_t key_size=privkey->ctx->length_secret_key;
    KEY_t key = privkey->priv_key;
    size_t enc_size = 4*((key_size+2)/3);
    char *encoded=(char *)malloc(enc_size);
    b64encode((unsigned char *)encoded, key, (int)key_size);
    fwrite(encoded,1,enc_size, file);
    fwrite(PEM_FOOTER_PRIV,1, PEM_HF_SIZE, file);
    fclose(file);
    free(encoded);
    return 0;
}

int check_header(FILE *file, char *HEADER){
    char *header=malloc(PEM_HF_SIZE);
    fread(header, 1, PEM_HF_SIZE, file);
    if (strcmp(header, HEADER)!=0) {
        free(header);
        return -1;
    }
    free(header);
    return 0;
}

KEY load_pubkey(PATH dir){
    FILE *file = fopen(catpath(dir, PUB_KEYFILE), "rb");
    if (!file) return NULL;
    if (check_header(file, PEM_HEADER_PUB)<0) return NULL;
    fseek(file, -PEM_HF_SIZE, SEEK_END);
    size_t key_size = ftell(file)-PEM_HF_SIZE;
    fseek(file, PEM_HF_SIZE, SEEK_SET);
    char *encoded = malloc(key_size);
    fread(encoded, 1, key_size, file);
    if (check_header(file, PEM_FOOTER_PUB)<0) return NULL;
    fclose(file);
    size_t decoded_len = ((key_size * 3) / 4)-1;
    unsigned char *decoded = malloc(decoded_len + 1);
    if (!decoded) return NULL;
    b64decode(decoded, (unsigned char *)encoded, key_size);
    KEY_CTX kem =OQS_KEM_new(KEYSIZE);
    if (kem==NULL) return NULL;
    size_t keysize=kem->length_public_key;
    if (decoded_len != keysize) return NULL;
    KEY_t pub_key = decoded;
    KEY key = (KEY)malloc(sizeof(KEY));
    key->ctx=kem;
    key->pub_key=pub_key;
    return key;
}

KEY load_privkey(PATH dir){
    FILE *file = fopen(catpath(dir, PRIV_KEYFILE), "rb");
    if (!file) return NULL;
    if (check_header(file, PEM_HEADER_PRIV)<0) return NULL;
    fseek(file, -PEM_HF_SIZE, SEEK_END);
    size_t key_size = ftell(file)-PEM_HF_SIZE;
    fseek(file, PEM_HF_SIZE, SEEK_SET);
    char *encoded = malloc(key_size);
    fread(encoded, 1, key_size, file);
    if (check_header(file, PEM_FOOTER_PRIV)<0) return NULL;
    fclose(file);
    size_t decoded_len = ((key_size * 3) / 4);
    unsigned char *decoded = malloc(decoded_len + 1);
    if (!decoded) return NULL;
    b64decode(decoded, (unsigned char *)encoded, key_size);
    KEY_CTX kem =OQS_KEM_new(KEYSIZE);
    if (kem==NULL) return NULL;
    size_t keysize=kem->length_secret_key;
    if (decoded_len != keysize) return NULL;
    KEY_t priv_key = decoded;
    KEY key = (KEY)malloc(sizeof(KEY));
    key->ctx=kem;
    key->priv_key=priv_key;
    return key;
}
unsigned char *oqs_encrypt(KEY pub_key, size_t *encrypted_len){
    //encapsulates a secret derived from the keys, which is used to create a symetric key
    KEY_CTX kem = pub_key->ctx;
    KEY_t shared_secret = malloc(kem->length_shared_secret);
    KEY_t ciphertext = malloc(kem->length_ciphertext);
    if (OQS_KEM_encaps(kem, ciphertext, shared_secret, pub_key->pub_key)!=0) return NULL;
    encrypted_len=&(kem->length_ciphertext);
    return ciphertext;
}

unsigned char *oqs_decrypt(KEY priv_key, unsigned char *ciphertext, size_t *decrypted_len){
    KEY_CTX kem = priv_key->ctx;
    KEY_t shared_secret = malloc(kem->length_shared_secret);
    decrypted_len = &(kem->length_shared_secret);
    if (OQS_KEM_decaps(kem, shared_secret, ciphertext, priv_key->priv_key)!=0) return NULL;
    return shared_secret;
}

int oqs_fencrypt(KEY pub_key, PATH filepath){
    size_t enc_len;
    unsigned char *encrypted = oqs_encrypt(pub_key, &enc_len);
    FILE *outfile = fopen(filepath, "wb");
    if (!outfile) return 1;
    fwrite(encrypted, 1, enc_len, outfile);
    fclose(outfile);
    return 0;
}

unsigned char *oqs_fdecrypt(KEY priv_key, PATH filepath){
    FILE *infile = fopen(filepath, "rb");
    if (!infile) return NULL;
    size_t size = fsize(infile);
    unsigned char *ciphertext = (unsigned char *)malloc(size);
    if (!ciphertext) return NULL;
    fread(ciphertext, 1, size, infile);
    fclose(infile);
    size_t dec_len;
    unsigned char *decrypted = oqs_decrypt(priv_key, ciphertext, &dec_len);
    return decrypted;
}

#endif //BDB_OQS_H