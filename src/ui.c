#include "img2pass.h"

void mostrar_menu_principal(void) {
    printf("=== IMG2PASS - Generador de Contraseñas ===\n");
    printf("1. Generar nueva clave privada\n");
    printf("2. Generar contraseña desde imagen\n");
    printf("3. Validar contraseña existente\n");
    printf("4. Salir\n");
    printf("Seleccione una opción: ");
}

void mostrar_menu_generar_clave(void) {
    printf("\n=== Generar Nueva Clave Privada ===\n");
}

void mostrar_menu_generar_contraseña(void) {
    printf("\n=== Generar Contraseña ===\n");
}

void mostrar_menu_validar_contraseña(void) {
    printf("\n=== Validar Contraseña ===\n");
}

void mostrar_algoritmos_hash(void) {
    printf("\nAlgoritmos disponibles:\n");
    printf("1. MD5\n");
    printf("2. SHA1\n");
    printf("3. SHA256\n");
    printf("4. SHA512\n");
    printf("Seleccione: ");
}

void mostrar_niveles_encriptacion(void) {
    printf("Nivel de encriptación:\n");
    printf("1. RSA 2048 bits\n");
    printf("2. RSA 3072 bits\n");
    printf("3. RSA 4096 bits\n");
    printf("Seleccione: ");
}

void limpiar_pantalla(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

int solicitar_opcion_numerica(int min, int max) {
    char buffer[10];
    int opcion;

    while (1) {
        printf("Seleccione (%d-%d): ", min, max);
        fflush(stdout);

        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            buffer[strcspn(buffer, "\n")] = 0;

            if (sscanf(buffer, "%d", &opcion) == 1) {
                if (opcion >= min && opcion <= max) {
                    return opcion;
                }
            }
        }

        printf("Por favor, ingrese un número válido entre %d y %d.\n", min, max);
    }
}

void solicitar_string(const char* prompt, char* buffer, size_t buffer_size) {
    printf("%s", prompt);
    fflush(stdout);

    if (fgets(buffer, buffer_size, stdin) != NULL) {
        buffer[strcspn(buffer, "\n")] = 0;
    }
}

void solicitar_archivo(const char* prompt, char* buffer, size_t buffer_size) {
    char input[MAX_PATH_LENGTH];

    while (1) {
        printf("%s", prompt);
        printf("\n💡 Arrastra el archivo aquí o escribe la ruta completa:\n");
        printf("Ruta: ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) != NULL) {
            input[strcspn(input, "\n")] = 0;

            // Limpiar espacios al inicio y final
            char *start = input;
            while (*start == ' ' || *start == '\t') start++;

            char *end = start + strlen(start) - 1;
            while (end > start && (*end == ' ' || *end == '\t' || *end == '\'' || *end == '\"')) {
                *end = '\0';
                end--;
            }

            // Remover comillas si las hay (drag & drop suele agregarlas)
            if (*start == '\'' || *start == '\"') {
                start++;
            }

            // Verificar si existe
            FILE *test = fopen(start, "r");
            if (test) {
                fclose(test);
                strncpy(buffer, start, buffer_size - 1);
                buffer[buffer_size - 1] = '\0';
                return;
            } else {
                printf("❌ Archivo no encontrado: %s\n", start);
                printf("💡 ¿Continuar de todas formas? (s/n): ");

                char c;
                if (scanf(" %c", &c) == 1) {
                    getchar(); // limpiar buffer
                    if (c == 's' || c == 'S') {
                        strncpy(buffer, start, buffer_size - 1);
                        buffer[buffer_size - 1] = '\0';
                        return;
                    }
                }
            }
        }
    }
}