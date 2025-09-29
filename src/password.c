#include "img2pass.h"

static const char PASSWORD_CHARSET[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!@#$%&*-_=+";
static const size_t CHARSET_SIZE = sizeof(PASSWORD_CHARSET) - 1;

char* procesar_imagen_y_clave(const char* img_path, const char* key_path, hash_algorithm_t algoritmo) {
    size_t img_size = 0;
    unsigned char* img_data = NULL;
    unsigned char* signature = NULL;
    size_t sig_len = 0;
    unsigned char* hash_result = NULL;
    char* password = NULL;

    printf("Leyendo imagen...\n");
    img_data = leer_archivo_completo(img_path, &img_size);
    if (!img_data) {
        printf("Error: No se pudo leer la imagen.\n");
        return NULL;
    }

    printf("Firmando datos de la imagen...\n");
    signature = firmar_datos(img_data, img_size, key_path, &sig_len);
    if (!signature) {
        printf("Error: No se pudo firmar la imagen.\n");
        free(img_data);
        return NULL;
    }

    printf("Generando hash %s...\n", get_hash_name(algoritmo));
    hash_result = malloc(64);
    if (!hash_result) {
        printf("Error: No se pudo asignar memoria para el hash.\n");
        goto cleanup;
    }

    size_t hash_len = 0;
    switch(algoritmo) {
        case HASH_MD5:
            MD5(signature, sig_len, hash_result);
            hash_len = MD5_DIGEST_LENGTH;
            break;
        case HASH_SHA1:
            SHA1(signature, sig_len, hash_result);
            hash_len = SHA_DIGEST_LENGTH;
            break;
        case HASH_SHA256:
            SHA256(signature, sig_len, hash_result);
            hash_len = SHA256_DIGEST_LENGTH;
            break;
        case HASH_SHA512:
            SHA512(signature, sig_len, hash_result);
            hash_len = SHA512_DIGEST_LENGTH;
            break;
        default:
            printf("Error: Algoritmo de hash no soportado.\n");
            goto cleanup;
    }

    printf("Convirtiendo a contraseña de %d caracteres...\n", PASSWORD_LENGTH);
    password = hash_to_password(hash_result, hash_len);

cleanup:
    if (img_data) free(img_data);
    if (signature) free(signature);
    if (hash_result) free(hash_result);
    return password;
}

void copiar_imagen_con_nombre(const char* origen, const char* destino) {
    FILE *src = fopen(origen, "rb");
    if (!src) {
        printf("Error: No se pudo abrir la imagen origen: %s\n", origen);
        return;
    }

    FILE *dst = fopen(destino, "wb");
    if (!dst) {
        printf("Error: No se pudo crear la copia de la imagen: %s\n", destino);
        fclose(src);
        return;
    }

    char buffer[8192];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), src)) > 0) {
        fwrite(buffer, 1, bytes_read, dst);
    }

    fclose(src);
    fclose(dst);
    printf("✓ Copia creada: %s\n", destino);
}

unsigned char* leer_archivo_completo(const char* filepath, size_t* file_size) {
    FILE *file = fopen(filepath, "rb");
    if (!file) {
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    *file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    unsigned char* buffer = malloc(*file_size);
    if (!buffer) {
        fclose(file);
        return NULL;
    }

    size_t bytes_read = fread(buffer, 1, *file_size, file);
    fclose(file);

    if (bytes_read != *file_size) {
        free(buffer);
        return NULL;
    }

    return buffer;
}

char* hash_to_password(const unsigned char* hash, size_t hash_len) {
    char* password = malloc(PASSWORD_LENGTH + 1);
    if (!password) {
        return NULL;
    }

    for (int i = 0; i < PASSWORD_LENGTH; i++) {
        size_t hash_index = (i * 2) % hash_len;

        unsigned int combined = (hash[hash_index] << 8) | hash[(hash_index + 1) % hash_len];

        combined ^= (i * 0x9E3779B9);

        password[i] = PASSWORD_CHARSET[combined % CHARSET_SIZE];
    }

    password[PASSWORD_LENGTH] = '\0';
    return password;
}