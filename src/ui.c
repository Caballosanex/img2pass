#include "img2pass.h"

void mostrar_menu_principal(void) {
    printf("=== IMG2PASS - Generador de Contraseñas ===\n");
    printf("1. Generar nueva clave privada\n");
    printf("2. Generar contraseña desde imagen\n");
    printf("3. Salir\n");
    printf("Seleccione una opción: ");
}

void mostrar_menu_generar_clave(void) {
    printf("\n=== Generar Nueva Clave Privada ===\n");
}

void mostrar_menu_generar_contraseña(void) {
    printf("\n=== Generar Contraseña ===\n");
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