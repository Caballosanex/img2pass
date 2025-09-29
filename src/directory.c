#include "img2pass.h"

void crear_estructura_directorios(const char* ruta_base) {
    char keys_path[MAX_PATH_LENGTH];
    char images_path[MAX_PATH_LENGTH];

    crear_directorio_recursivo(ruta_base);

    snprintf(keys_path, sizeof(keys_path), "%s%s%s", ruta_base, PATH_SEPARATOR, KEYS_DIR_NAME);
    snprintf(images_path, sizeof(images_path), "%s%s%s", ruta_base, PATH_SEPARATOR, IMAGES_DIR_NAME);

    crear_directorio_recursivo(keys_path);
    crear_directorio_recursivo(images_path);
}

bool validar_permisos_directorio(const char* ruta) {
    if (!directorio_existe(ruta)) {
        return false;
    }

    char test_file[MAX_PATH_LENGTH];
    snprintf(test_file, sizeof(test_file), "%s%s.test_permissions", ruta, PATH_SEPARATOR);

    FILE *file = fopen(test_file, "w");
    if (!file) {
        return false;
    }

    fclose(file);
    remove(test_file);
    return true;
}

void get_home_directory(char* home_path) {
#ifdef _WIN32
    const char* home_drive = getenv("HOMEDRIVE");
    const char* home_path_env = getenv("HOMEPATH");
    if (home_drive && home_path_env) {
        snprintf(home_path, MAX_PATH_LENGTH, "%s%s", home_drive, home_path_env);
    } else {
        strcpy(home_path, "C:\\");
    }
#else
    const char* home = getenv("HOME");
    if (home) {
        strcpy(home_path, home);
    } else {
        strcpy(home_path, "/tmp");
    }
#endif
}

void get_current_directory(char* current_path) {
#ifdef _WIN32
    if (!_getcwd(current_path, MAX_PATH_LENGTH)) {
        strcpy(current_path, ".");
    }
#else
    if (!getcwd(current_path, MAX_PATH_LENGTH)) {
        strcpy(current_path, ".");
    }
#endif
}