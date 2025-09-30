#ifndef IMG2PASS_H
#define IMG2PASS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/evp.h>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <openssl/err.h>

#ifdef _WIN32
    #include <windows.h>
    #include <direct.h>
    #include <conio.h>
    #define PATH_SEPARATOR "\\"
    #define mkdir(path, mode) _mkdir(path)
#else
    #include <unistd.h>
    #include <sys/types.h>
    #include <dirent.h>
    #include <termios.h>
    #define PATH_SEPARATOR "/"
#endif

#define MAX_PATH_LENGTH 4096
#define MAX_FILENAME_LENGTH 256
#define PASSWORD_LENGTH 20
#define CONFIG_FILENAME ".img2pass_config"
#define VAULT_DIR_NAME "img2pass_vault"
#define KEYS_DIR_NAME "claves"
#define IMAGES_DIR_NAME "imagenes"

typedef enum {
    HASH_MD5,
    HASH_SHA1,
    HASH_SHA256,
    HASH_SHA512
} hash_algorithm_t;

typedef enum {
    RSA_2048,
    RSA_3072,
    RSA_4096
} rsa_key_size_t;

typedef struct {
    char vault_path[MAX_PATH_LENGTH];
    bool first_run;
} config_t;

// config.c
bool es_primera_ejecucion(void);
void configuracion_inicial(void);
void cargar_configuracion(config_t *config);
void guardar_configuracion(const config_t *config);

// directory.c
void crear_estructura_directorios(const char* ruta_base);
bool validar_permisos_directorio(const char* ruta);
void get_home_directory(char* home_path);
void get_current_directory(char* current_path);

// crypto.c
void generar_clave_privada(const char* nombre, rsa_key_size_t key_size, const char* keys_dir);
bool validar_clave_privada(const char* ruta);
unsigned char* firmar_datos(const unsigned char* data, size_t data_len, const char* key_path, size_t* sig_len);

// password.c
char* procesar_imagen_y_clave(const char* img_path, const char* key_path, hash_algorithm_t algoritmo);
void copiar_imagen_con_nombre(const char* origen, const char* destino);
unsigned char* leer_archivo_completo(const char* filepath, size_t* file_size);
char* hash_to_password(const unsigned char* hash, size_t hash_len);
char* validar_contraseña_desde_vault(const char* nombre_imagen, const char* nombre_clave, hash_algorithm_t algoritmo, const config_t* config);

// ui.c
void mostrar_menu_principal(void);
void mostrar_menu_generar_clave(void);
void mostrar_menu_generar_contraseña(void);
void mostrar_menu_validar_contraseña(void);
void mostrar_algoritmos_hash(void);
void mostrar_niveles_encriptacion(void);
void limpiar_pantalla(void);
int solicitar_opcion_numerica(int min, int max);
void solicitar_string(const char* prompt, char* buffer, size_t buffer_size);
void solicitar_archivo(const char* prompt, char* buffer, size_t buffer_size);
void solicitar_archivo_con_autocompletado(const char* prompt, char* buffer, size_t buffer_size);
void solicitar_nombre_archivo_vault(const char* prompt, char* buffer, size_t buffer_size, const char* directorio, const char* extension_filtro);
int autocompletar_archivos_vault(char* buffer, size_t buffer_size, int cursor_pos, const char* directorio, const char* extension_filtro);

// utils.c
bool archivo_existe(const char* ruta);
bool directorio_existe(const char* ruta);
void crear_directorio_recursivo(const char* path);
const char* get_hash_name(hash_algorithm_t algo);
const char* get_rsa_size_name(rsa_key_size_t size);
int get_rsa_bits(rsa_key_size_t size);

#endif // IMG2PASS_H