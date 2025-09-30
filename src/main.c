#include "img2pass.h"

void menu_generar_clave(const config_t *config) {
    char nombre[MAX_FILENAME_LENGTH];
    char keys_dir[MAX_PATH_LENGTH];
    int opcion_encriptacion;
    rsa_key_size_t key_size;

    mostrar_menu_generar_clave();

    printf("Nombre para la clave (sin extensión): ");
    solicitar_string("", nombre, sizeof(nombre));

    if (strlen(nombre) == 0) {
        printf("Error: Debe ingresar un nombre para la clave.\n");
        return;
    }

    mostrar_niveles_encriptacion();
    opcion_encriptacion = solicitar_opcion_numerica(1, 3);

    switch(opcion_encriptacion) {
        case 1: key_size = RSA_2048; break;
        case 2: key_size = RSA_3072; break;
        case 3: key_size = RSA_4096; break;
        default: key_size = RSA_2048; break;
    }

    snprintf(keys_dir, sizeof(keys_dir), "%s%s%s", config->vault_path, PATH_SEPARATOR, KEYS_DIR_NAME);

    generar_clave_privada(nombre, key_size, keys_dir);

    printf("\nPresione Enter para continuar...");
    getchar();
}

void menu_validar_contraseña(const config_t *config) {
    char nombre_imagen[MAX_FILENAME_LENGTH];
    char nombre_clave[MAX_FILENAME_LENGTH];
    int opcion_algoritmo;
    hash_algorithm_t algoritmo;
    char* password = NULL;

    mostrar_menu_validar_contraseña();

    printf("Nombre de la imagen en el vault (sin extensión): ");
    solicitar_string("", nombre_imagen, sizeof(nombre_imagen));

    if (strlen(nombre_imagen) == 0) {
        printf("Error: Debe ingresar el nombre de la imagen.\n");
        return;
    }

    printf("Nombre de la clave privada (sin extensión): ");
    solicitar_string("", nombre_clave, sizeof(nombre_clave));

    if (strlen(nombre_clave) == 0) {
        printf("Error: Debe ingresar el nombre de la clave.\n");
        return;
    }

    mostrar_algoritmos_hash();
    opcion_algoritmo = solicitar_opcion_numerica(1, 4);

    switch(opcion_algoritmo) {
        case 1: algoritmo = HASH_MD5; break;
        case 2: algoritmo = HASH_SHA1; break;
        case 3: algoritmo = HASH_SHA256; break;
        case 4: algoritmo = HASH_SHA512; break;
        default: algoritmo = HASH_SHA256; break;
    }

    printf("\nValidando contraseña...\n");
    password = validar_contraseña_desde_vault(nombre_imagen, nombre_clave, algoritmo, config);

    if (password) {
        printf("\n========================================\n");
        printf("Contraseña: %s\n", password);
        printf("========================================\n");
        free(password);
    } else {
        printf("Error: No se pudo validar la contraseña.\n");
    }

    printf("\nPresione Enter para continuar...");
    getchar();
}

void menu_generar_contraseña(const config_t *config) {
    char img_path[MAX_PATH_LENGTH];
    char key_path[MAX_PATH_LENGTH];
    char nombre_imagen[MAX_FILENAME_LENGTH];
    char img_copy_path[MAX_PATH_LENGTH];
    char images_dir[MAX_PATH_LENGTH];
    int opcion_algoritmo;
    hash_algorithm_t algoritmo;
    char* password = NULL;

    mostrar_menu_generar_contraseña();

    solicitar_archivo("Ruta de la imagen:", img_path, sizeof(img_path));

    if (!archivo_existe(img_path)) {
        printf("Error: La imagen especificada no existe.\n");
        return;
    }

    solicitar_archivo("Ruta de la clave privada:", key_path, sizeof(key_path));

    if (!validar_clave_privada(key_path)) {
        printf("Error: La clave privada no es válida o no existe.\n");
        return;
    }

    mostrar_algoritmos_hash();
    opcion_algoritmo = solicitar_opcion_numerica(1, 4);

    switch(opcion_algoritmo) {
        case 1: algoritmo = HASH_MD5; break;
        case 2: algoritmo = HASH_SHA1; break;
        case 3: algoritmo = HASH_SHA256; break;
        case 4: algoritmo = HASH_SHA512; break;
        default: algoritmo = HASH_SHA256; break;
    }

    printf("Nombre para la copia de imagen: ");
    solicitar_string("", nombre_imagen, sizeof(nombre_imagen));

    if (strlen(nombre_imagen) == 0) {
        printf("Error: Debe ingresar un nombre para la copia de la imagen.\n");
        return;
    }

    snprintf(images_dir, sizeof(images_dir), "%s%s%s", config->vault_path, PATH_SEPARATOR, IMAGES_DIR_NAME);

    char *extension = strrchr(img_path, '.');
    if (extension) {
        snprintf(img_copy_path, sizeof(img_copy_path), "%s%s%s%s", images_dir, PATH_SEPARATOR, nombre_imagen, extension);
    } else {
        snprintf(img_copy_path, sizeof(img_copy_path), "%s%s%s", images_dir, PATH_SEPARATOR, nombre_imagen);
    }

    copiar_imagen_con_nombre(img_path, img_copy_path);

    printf("\nGenerando contraseña...\n");
    password = procesar_imagen_y_clave(img_copy_path, key_path, algoritmo);

    if (password) {
        printf("\n========================================\n");
        printf("Contraseña: %s\n", password);
        printf("========================================\n");
        free(password);
    } else {
        printf("Error: No se pudo generar la contraseña.\n");
    }

    printf("\nPresione Enter para continuar...");
    getchar();
}

int main(void) {
    config_t config;
    int opcion;

    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();

    if (es_primera_ejecucion()) {
        configuracion_inicial();
    }

    cargar_configuracion(&config);

    if (strlen(config.vault_path) == 0 || !directorio_existe(config.vault_path)) {
        printf("Error: Configuración inválida. Ejecute el programa de nuevo para reconfigurar.\n");
        return 1;
    }

    while (1) {
        limpiar_pantalla();
        printf("Cargando configuración...\n");
        printf("✓ Directorio de trabajo: %s\n\n", config.vault_path);

        mostrar_menu_principal();
        opcion = solicitar_opcion_numerica(1, 4);

        switch(opcion) {
            case 1:
                menu_generar_clave(&config);
                break;
            case 2:
                menu_generar_contraseña(&config);
                break;
            case 3:
                menu_validar_contraseña(&config);
                break;
            case 4:
                printf("¡Hasta luego!\n");
                EVP_cleanup();
                ERR_free_strings();
                return 0;
            default:
                printf("Opción no válida.\n");
                break;
        }
    }

    EVP_cleanup();
    ERR_free_strings();
    return 0;
}