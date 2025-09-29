#include "img2pass.h"

bool es_primera_ejecucion(void) {
    return !archivo_existe(CONFIG_FILENAME);
}

void configuracion_inicial(void) {
    config_t config;
    char opcion_str[10];
    int opcion;
    char ruta_personalizada[MAX_PATH_LENGTH];
    char current_dir[MAX_PATH_LENGTH];
    char home_dir[MAX_PATH_LENGTH];

    printf("=== IMG2PASS - Primera Configuración ===\n");
    printf("¡Bienvenido! Este parece ser tu primer uso de img2pass.\n\n");

    get_current_directory(current_dir);
    get_home_directory(home_dir);

    printf("Selecciona dónde crear el directorio de trabajo:\n");
    printf("1. Directorio actual: %s\n", current_dir);
    printf("2. Directorio home: %s\n", home_dir);
    printf("3. Ruta personalizada\n");

    opcion = solicitar_opcion_numerica(1, 3);

    switch(opcion) {
        case 1:
            snprintf(config.vault_path, sizeof(config.vault_path), "%s%s%s",
                    current_dir, PATH_SEPARATOR, VAULT_DIR_NAME);
            break;
        case 2:
            snprintf(config.vault_path, sizeof(config.vault_path), "%s%s%s",
                    home_dir, PATH_SEPARATOR, VAULT_DIR_NAME);
            break;
        case 3:
            printf("Ingrese la ruta personalizada: ");
            solicitar_string("", ruta_personalizada, sizeof(ruta_personalizada));
            snprintf(config.vault_path, sizeof(config.vault_path), "%s%s%s",
                    ruta_personalizada, PATH_SEPARATOR, VAULT_DIR_NAME);
            break;
    }

    printf("\nCreando estructura de directorios en: %s\n", config.vault_path);
    crear_estructura_directorios(config.vault_path);

    if (!validar_permisos_directorio(config.vault_path)) {
        printf("Error: No se pueden crear los directorios en la ruta especificada.\n");
        printf("Por favor, verifica los permisos e intenta de nuevo.\n");
        exit(1);
    }

    config.first_run = false;
    guardar_configuracion(&config);

    printf("✓ Configuración completada exitosamente.\n");
    printf("Presione Enter para continuar...");
    getchar();
}

void cargar_configuracion(config_t *config) {
    FILE *file = fopen(CONFIG_FILENAME, "r");
    if (!file) {
        config->first_run = true;
        strcpy(config->vault_path, "");
        return;
    }

    char linea[MAX_PATH_LENGTH];
    config->first_run = false;
    strcpy(config->vault_path, "");

    while (fgets(linea, sizeof(linea), file)) {
        linea[strcspn(linea, "\n")] = 0;

        if (strncmp(linea, "vault_path=", 11) == 0) {
            strcpy(config->vault_path, linea + 11);
        } else if (strncmp(linea, "first_run=", 10) == 0) {
            config->first_run = (strcmp(linea + 10, "true") == 0);
        }
    }

    fclose(file);
}

void guardar_configuracion(const config_t *config) {
    FILE *file = fopen(CONFIG_FILENAME, "w");
    if (!file) {
        printf("Error: No se pudo guardar la configuración.\n");
        return;
    }

    fprintf(file, "vault_path=%s\n", config->vault_path);
    fprintf(file, "first_run=%s\n", config->first_run ? "true" : "false");

    fclose(file);
}