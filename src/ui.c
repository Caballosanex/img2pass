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

void expandir_ruta_home(const char* ruta, char* buffer, size_t buffer_size) {
    if (ruta[0] == '~') {
        const char* home = getenv("HOME");
        if (!home) {
#ifdef _WIN32
            home = getenv("USERPROFILE");
#endif
        }

        if (home) {
            snprintf(buffer, buffer_size, "%s%s", home, ruta + 1);
        } else {
            strncpy(buffer, ruta, buffer_size - 1);
            buffer[buffer_size - 1] = '\0';
        }
    } else {
        strncpy(buffer, ruta, buffer_size - 1);
        buffer[buffer_size - 1] = '\0';
    }
}

#ifndef _WIN32
static struct termios old_termios;

void enable_raw_mode() {
    tcgetattr(STDIN_FILENO, &old_termios);
    struct termios raw = old_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void disable_raw_mode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &old_termios);
}
#endif

int autocompletar_ruta(char* buffer, size_t buffer_size, int cursor_pos) {
    char path_part[MAX_PATH_LENGTH];
    char dir_part[MAX_PATH_LENGTH];
    char file_part[MAX_FILENAME_LENGTH];

    // Extraer la parte de la ruta hasta el cursor
    strncpy(path_part, buffer, cursor_pos);
    path_part[cursor_pos] = '\0';

    // Encontrar la última barra para separar directorio y archivo
    char* last_slash = strrchr(path_part, '/');
#ifdef _WIN32
    char* last_backslash = strrchr(path_part, '\\');
    if (last_backslash > last_slash) {
        last_slash = last_backslash;
    }
#endif

    if (last_slash) {
        strncpy(dir_part, path_part, last_slash - path_part + 1);
        dir_part[last_slash - path_part + 1] = '\0';
        strcpy(file_part, last_slash + 1);
    } else {
        strcpy(dir_part, "./");
        strcpy(file_part, path_part);
    }

    // Expandir ~ si es necesario
    char expanded_dir[MAX_PATH_LENGTH];
    expandir_ruta_home(dir_part, expanded_dir, sizeof(expanded_dir));

    // Buscar archivos que coincidan
#ifndef _WIN32
    DIR *dir;
    struct dirent *entry;
#endif
    char matches[100][MAX_FILENAME_LENGTH];
    int match_count = 0;
    int file_part_len = strlen(file_part);

#ifdef _WIN32
    WIN32_FIND_DATA findFileData;
    HANDLE hFind;
    char search_pattern[MAX_PATH_LENGTH];
    snprintf(search_pattern, sizeof(search_pattern), "%s*", expanded_dir);

    hFind = FindFirstFile(search_pattern, &findFileData);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (strncmp(findFileData.cFileName, file_part, file_part_len) == 0 && match_count < 100) {
                strcpy(matches[match_count], findFileData.cFileName);
                match_count++;
            }
        } while (FindNextFile(hFind, &findFileData) != 0 && match_count < 100);
        FindClose(hFind);
    }
#else
    dir = opendir(expanded_dir);
    if (dir) {
        while ((entry = readdir(dir)) != NULL && match_count < 100) {
            if (entry->d_name[0] == '.' && file_part[0] != '.') continue;
            if (strncmp(entry->d_name, file_part, file_part_len) == 0) {
                strcpy(matches[match_count], entry->d_name);
                match_count++;
            }
        }
        closedir(dir);
    }
#endif

    if (match_count == 0) {
        return cursor_pos; // No hay coincidencias
    } else if (match_count == 1) {
        // Una sola coincidencia, autocompletar
        int remaining_space = buffer_size - cursor_pos - 1;
        int needed_space = strlen(matches[0]) - file_part_len;

        if (needed_space <= remaining_space) {
            strcpy(buffer + cursor_pos, matches[0] + file_part_len);
            return cursor_pos + needed_space;
        }
    } else {
        // Múltiples coincidencias, mostrar opciones
        printf("\n");
        for (int i = 0; i < match_count; i++) {
            printf("%s  ", matches[i]);
            if ((i + 1) % 5 == 0) printf("\n"); // 5 por línea
        }
        if (match_count % 5 != 0) printf("\n");

        // Encontrar prefijo común
        int common_len = strlen(matches[0]);
        for (int i = 1; i < match_count; i++) {
            int j = 0;
            while (j < common_len && j < strlen(matches[i]) &&
                   matches[0][j] == matches[i][j]) {
                j++;
            }
            common_len = j;
        }

        // Autocompletar hasta el prefijo común
        if (common_len > file_part_len) {
            int remaining_space = buffer_size - cursor_pos - 1;
            int needed_space = common_len - file_part_len;

            if (needed_space <= remaining_space) {
                strncpy(buffer + cursor_pos, matches[0] + file_part_len, needed_space);
                buffer[cursor_pos + needed_space] = '\0';
                return cursor_pos + needed_space;
            }
        }
    }

    return cursor_pos;
}

void solicitar_archivo_con_autocompletado(const char* prompt, char* buffer, size_t buffer_size) {
    char input[MAX_PATH_LENGTH] = "";
    int cursor_pos = 0;
    int c;

    printf("%s", prompt);
    printf("\n(Usa Tab para autocompletar, Enter para confirmar)\n");
    printf("Ruta: ");
    fflush(stdout);

#ifndef _WIN32
    enable_raw_mode();
#endif

    while (1) {
#ifdef _WIN32
        c = _getch();
#else
        c = getchar();
#endif

        if (c == '\n' || c == '\r') {
            // Enter presionado
            break;
        } else if (c == '\t') {
            // Tab presionado - autocompletar
            cursor_pos = autocompletar_ruta(input, sizeof(input), cursor_pos);

            // Limpiar línea y mostrar de nuevo
            printf("\r\033[K"); // Limpiar línea
            printf("Ruta: %s", input);
            fflush(stdout);
        } else if (c == 127 || c == 8) {
            // Backspace
            if (cursor_pos > 0) {
                cursor_pos--;
                input[cursor_pos] = '\0';
                printf("\r\033[K"); // Limpiar línea
                printf("Ruta: %s", input);
                fflush(stdout);
            }
        } else if (c >= 32 && c <= 126) {
            // Caracter imprimible
            if (cursor_pos < sizeof(input) - 1) {
                input[cursor_pos] = c;
                cursor_pos++;
                input[cursor_pos] = '\0';
                printf("%c", c);
                fflush(stdout);
            }
        }
    }

#ifndef _WIN32
    disable_raw_mode();
#endif

    printf("\n");

    // Limpiar entrada y expandir ~
    char *start = input;
    while (*start == ' ' || *start == '\t') start++;

    char *end = start + strlen(start) - 1;
    while (end > start && (*end == ' ' || *end == '\t' || *end == '\'' || *end == '\"')) {
        *end = '\0';
        end--;
    }

    if (*start == '\'' || *start == '\"') {
        start++;
    }

    expandir_ruta_home(start, buffer, buffer_size);
}

void solicitar_nombre_archivo_vault(const char* prompt, char* buffer, size_t buffer_size, const char* directorio, const char* extension_filtro) {
    char input[MAX_FILENAME_LENGTH] = "";
    int cursor_pos = 0;
    int c;

    printf("%s", prompt);
    printf("\n(Usa Tab para autocompletar archivos del vault, Enter para confirmar)\n");
    printf("Nombre: ");
    fflush(stdout);

#ifndef _WIN32
    enable_raw_mode();
#endif

    while (1) {
#ifdef _WIN32
        c = _getch();
#else
        c = getchar();
#endif

        if (c == '\n' || c == '\r') {
            break;
        } else if (c == '\t') {
            // Autocompletar archivos del vault
            cursor_pos = autocompletar_archivos_vault(input, sizeof(input), cursor_pos, directorio, extension_filtro);

            printf("\r\033[K");
            printf("Nombre: %s", input);
            fflush(stdout);
        } else if (c == 127 || c == 8) {
            // Backspace
            if (cursor_pos > 0) {
                cursor_pos--;
                input[cursor_pos] = '\0';
                printf("\r\033[K");
                printf("Nombre: %s", input);
                fflush(stdout);
            }
        } else if (c >= 32 && c <= 126) {
            // Caracter imprimible
            if (cursor_pos < sizeof(input) - 1) {
                input[cursor_pos] = c;
                cursor_pos++;
                input[cursor_pos] = '\0';
                printf("%c", c);
                fflush(stdout);
            }
        }
    }

#ifndef _WIN32
    disable_raw_mode();
#endif

    printf("\n");

    // Limpiar entrada
    char *start = input;
    while (*start == ' ' || *start == '\t') start++;

    char *end = start + strlen(start) - 1;
    while (end > start && (*end == ' ' || *end == '\t')) {
        *end = '\0';
        end--;
    }

    strncpy(buffer, start, buffer_size - 1);
    buffer[buffer_size - 1] = '\0';
}

int autocompletar_archivos_vault(char* buffer, size_t buffer_size, int cursor_pos, const char* directorio, const char* extension_filtro) {
    char matches[100][MAX_FILENAME_LENGTH];
    int match_count = 0;
    int input_len = cursor_pos;

#ifdef _WIN32
    WIN32_FIND_DATA findFileData;
    HANDLE hFind;
    char search_pattern[MAX_PATH_LENGTH];
    snprintf(search_pattern, sizeof(search_pattern), "%s\\*", directorio);

    hFind = FindFirstFile(search_pattern, &findFileData);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                char nombre_sin_ext[MAX_FILENAME_LENGTH];
                strcpy(nombre_sin_ext, findFileData.cFileName);

                // Remover extensión si coincide con el filtro
                if (extension_filtro) {
                    char* punto = strrchr(nombre_sin_ext, '.');
                    if (punto && strcmp(punto, extension_filtro) == 0) {
                        *punto = '\0';
                    }
                } else {
                    char* punto = strrchr(nombre_sin_ext, '.');
                    if (punto) *punto = '\0';
                }

                if (strncmp(nombre_sin_ext, buffer, input_len) == 0 && match_count < 100) {
                    strcpy(matches[match_count], nombre_sin_ext);
                    match_count++;
                }
            }
        } while (FindNextFile(hFind, &findFileData) != 0 && match_count < 100);
        FindClose(hFind);
    }
#else
    DIR *dir = opendir(directorio);
    if (dir) {
        struct dirent *entry;
        while ((entry = readdir(dir)) != NULL && match_count < 100) {
            if (entry->d_type == DT_REG) {
                char nombre_sin_ext[MAX_FILENAME_LENGTH];
                strcpy(nombre_sin_ext, entry->d_name);

                // Remover extensión
                char* punto = strrchr(nombre_sin_ext, '.');
                if (punto) {
                    if (!extension_filtro || strcmp(punto, extension_filtro) == 0) {
                        *punto = '\0';
                    } else {
                        continue; // Skip si no coincide la extensión
                    }
                }

                if (strncmp(nombre_sin_ext, buffer, input_len) == 0) {
                    strcpy(matches[match_count], nombre_sin_ext);
                    match_count++;
                }
            }
        }
        closedir(dir);
    }
#endif

    if (match_count == 0) {
        return cursor_pos;
    } else if (match_count == 1) {
        // Una coincidencia, autocompletar
        int remaining_space = buffer_size - cursor_pos - 1;
        int needed_space = strlen(matches[0]) - input_len;

        if (needed_space <= remaining_space) {
            strcpy(buffer + cursor_pos, matches[0] + input_len);
            return cursor_pos + needed_space;
        }
    } else {
        // Múltiples coincidencias
        printf("\n");
        for (int i = 0; i < match_count; i++) {
            printf("%s  ", matches[i]);
            if ((i + 1) % 5 == 0) printf("\n");
        }
        if (match_count % 5 != 0) printf("\n");

        // Prefijo común
        int common_len = strlen(matches[0]);
        for (int i = 1; i < match_count; i++) {
            int j = 0;
            while (j < common_len && j < strlen(matches[i]) &&
                   matches[0][j] == matches[i][j]) {
                j++;
            }
            common_len = j;
        }

        if (common_len > input_len) {
            int remaining_space = buffer_size - cursor_pos - 1;
            int needed_space = common_len - input_len;

            if (needed_space <= remaining_space) {
                strncpy(buffer + cursor_pos, matches[0] + input_len, needed_space);
                buffer[cursor_pos + needed_space] = '\0';
                return cursor_pos + needed_space;
            }
        }
    }

    return cursor_pos;
}