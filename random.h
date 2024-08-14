// Cryptographically secure random number generator
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
    #include <windows.h>
    #include <wincrypt.h>

    unsigned int get_random_bytes() {
        HCRYPTPROV hCryptProv;

        if (!CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {
            fprintf(stderr, "CryptAcquireContext failed\n");
            exit(EXIT_FAILURE);
        }
        BYTE buf;
        if (!CryptGenRandom(hCryptProv, sizeof(buf), &buf)) {
            fprintf(stderr, "CryptGenRandom failed\n");
            CryptReleaseContext(hCryptProv, 0);
            exit(EXIT_FAILURE);
        }

        CryptReleaseContext(hCryptProv, 0);
        return buf;
    }

#else
    #include <fcntl.h>
    #include <unistd.h>

    int get_random_bytes() {
        int fd = open("/dev/urandom", O_RDONLY);
        if (fd == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }
        unsigned int buf;
        ssize_t result = read(fd, &buf, sizeof(buf));
        if (result == -1) {
            perror("read");
            close(fd);
            exit(EXIT_FAILURE);
        }

        close(fd);
        return buf;
    }
#endif

int randombelow(int max){
    int value = get_random_bytes();
    return value%max;
}

int* randarray(int size, int max){
    int* array = (int*)malloc(size*sizeof(int));
    if (array==NULL){
        perror("Array Allocation Faild");
        exit(EXIT_FAILURE);
    }
    for (int i=0; i<size; i++){
        array[i] = randombelow(max);
    }
    return array;
}
