#include "img2pass.h"

bool archivo_existe(const char* ruta) {
    FILE *file = fopen(ruta, "r");
    if (file) {
        fclose(file);
        return true;
    }
    return false;
}

bool directorio_existe(const char* ruta) {
    struct stat st;
    return (stat(ruta, &st) == 0 && S_ISDIR(st.st_mode));
}

void crear_directorio_recursivo(const char* path) {
    char temp[MAX_PATH_LENGTH];
    char *pos = NULL;
    size_t len;

    snprintf(temp, sizeof(temp), "%s", path);
    len = strlen(temp);
    if(temp[len - 1] == '/' || temp[len - 1] == '\\')
        temp[len - 1] = 0;

    for(pos = temp + 1; *pos; pos++) {
        if(*pos == '/' || *pos == '\\') {
            *pos = 0;
            mkdir(temp, 0755);
            *pos = PATH_SEPARATOR[0];
        }
    }
    mkdir(temp, 0755);
}

const char* get_hash_name(hash_algorithm_t algo) {
    switch(algo) {
        case HASH_MD5: return "MD5";
        case HASH_SHA1: return "SHA1";
        case HASH_SHA256: return "SHA256";
        case HASH_SHA512: return "SHA512";
        default: return "Unknown";
    }
}

const char* get_rsa_size_name(rsa_key_size_t size) {
    switch(size) {
        case RSA_2048: return "RSA 2048 bits";
        case RSA_3072: return "RSA 3072 bits";
        case RSA_4096: return "RSA 4096 bits";
        default: return "Unknown";
    }
}

int get_rsa_bits(rsa_key_size_t size) {
    switch(size) {
        case RSA_2048: return 2048;
        case RSA_3072: return 3072;
        case RSA_4096: return 4096;
        default: return 2048;
    }
}