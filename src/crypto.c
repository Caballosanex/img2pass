#include "img2pass.h"

void generar_clave_privada(const char* nombre, rsa_key_size_t key_size, const char* keys_dir) {
    EVP_PKEY_CTX *ctx = NULL;
    EVP_PKEY *pkey = NULL;
    FILE *private_key_file = NULL;
    char key_path[MAX_PATH_LENGTH];
    int bits = get_rsa_bits(key_size);

    printf("\nGenerando clave privada %s de %d bits...\n", nombre, bits);

    ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);
    if (!ctx) {
        printf("Error: No se pudo crear el contexto para generar la clave.\n");
        goto cleanup;
    }

    if (EVP_PKEY_keygen_init(ctx) <= 0) {
        printf("Error: No se pudo inicializar la generación de claves.\n");
        goto cleanup;
    }

    if (EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, bits) <= 0) {
        printf("Error: No se pudo establecer el tamaño de la clave.\n");
        goto cleanup;
    }

    if (EVP_PKEY_keygen(ctx, &pkey) <= 0) {
        printf("Error: No se pudo generar la clave privada.\n");
        goto cleanup;
    }

    snprintf(key_path, sizeof(key_path), "%s%s%s.pem", keys_dir, PATH_SEPARATOR, nombre);

    private_key_file = fopen(key_path, "w");
    if (!private_key_file) {
        printf("Error: No se pudo crear el archivo de clave privada: %s\n", key_path);
        goto cleanup;
    }

    if (!PEM_write_PrivateKey(private_key_file, pkey, NULL, NULL, 0, NULL, NULL)) {
        printf("Error: No se pudo escribir la clave privada al archivo.\n");
        goto cleanup;
    }

    printf("✓ Clave creada: %s\n", key_path);

cleanup:
    if (private_key_file) fclose(private_key_file);
    if (pkey) EVP_PKEY_free(pkey);
    if (ctx) EVP_PKEY_CTX_free(ctx);
}

bool validar_clave_privada(const char* ruta) {
    FILE *file = fopen(ruta, "r");
    if (!file) {
        return false;
    }

    EVP_PKEY *pkey = PEM_read_PrivateKey(file, NULL, NULL, NULL);
    fclose(file);

    if (pkey) {
        EVP_PKEY_free(pkey);
        return true;
    }
    return false;
}

unsigned char* firmar_datos(const unsigned char* data, size_t data_len, const char* key_path, size_t* sig_len) {
    FILE *key_file = NULL;
    EVP_PKEY *pkey = NULL;
    EVP_PKEY_CTX *ctx = NULL;
    unsigned char *signature = NULL;

    key_file = fopen(key_path, "r");
    if (!key_file) {
        printf("Error: No se pudo abrir la clave privada: %s\n", key_path);
        return NULL;
    }

    pkey = PEM_read_PrivateKey(key_file, NULL, NULL, NULL);
    fclose(key_file);
    if (!pkey) {
        printf("Error: No se pudo leer la clave privada.\n");
        return NULL;
    }

    ctx = EVP_PKEY_CTX_new(pkey, NULL);
    if (!ctx) {
        printf("Error: No se pudo crear el contexto de firma.\n");
        EVP_PKEY_free(pkey);
        return NULL;
    }

    if (EVP_PKEY_sign_init(ctx) <= 0) {
        printf("Error: No se pudo inicializar la firma.\n");
        goto cleanup;
    }

    if (EVP_PKEY_sign(ctx, NULL, sig_len, data, data_len) <= 0) {
        printf("Error: No se pudo determinar el tamaño de la firma.\n");
        goto cleanup;
    }

    signature = malloc(*sig_len);
    if (!signature) {
        printf("Error: No se pudo asignar memoria para la firma.\n");
        goto cleanup;
    }

    if (EVP_PKEY_sign(ctx, signature, sig_len, data, data_len) <= 0) {
        printf("Error: No se pudo crear la firma.\n");
        free(signature);
        signature = NULL;
        goto cleanup;
    }

cleanup:
    if (ctx) EVP_PKEY_CTX_free(ctx);
    if (pkey) EVP_PKEY_free(pkey);
    return signature;
}