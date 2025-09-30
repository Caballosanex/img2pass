#!/bin/bash

# Script de instalación automática para img2pass
# Maneja la instalación del binario y actualización del PATH

set -e

BINARY_NAME="img2pass"
INSTALL_DIR="$HOME/.local/bin"
BINARY_PATH=""
SHELL_CONFIG=""

# Detectar el shell actual y archivo de configuración
detect_shell() {
    local current_shell=$(basename "$SHELL")

    case "$current_shell" in
        "bash")
            if [[ -f "$HOME/.bashrc" ]]; then
                SHELL_CONFIG="$HOME/.bashrc"
            elif [[ -f "$HOME/.bash_profile" ]]; then
                SHELL_CONFIG="$HOME/.bash_profile"
            fi
            ;;
        "zsh")
            SHELL_CONFIG="$HOME/.zshrc"
            ;;
        "fish")
            SHELL_CONFIG="$HOME/.config/fish/config.fish"
            ;;
        *)
            SHELL_CONFIG="$HOME/.profile"
            ;;
    esac

    echo "Shell detectado: $current_shell"
    echo "Archivo de configuración: $SHELL_CONFIG"
}

# Buscar binario existente en PATH
find_existing_binary() {
    local existing_path=$(which "$BINARY_NAME" 2>/dev/null || true)

    if [[ -n "$existing_path" ]]; then
        echo "Binario existente encontrado en: $existing_path"
        return 0
    else
        echo "No se encontró binario existente en PATH"
        return 1
    fi
}

# Crear directorio de instalación si no existe
create_install_dir() {
    if [[ ! -d "$INSTALL_DIR" ]]; then
        echo "Creando directorio de instalación: $INSTALL_DIR"
        mkdir -p "$INSTALL_DIR"
    fi
}

# Compilar el proyecto si es necesario
build_project() {
    # Verificar si existe CMakeLists.txt
    if [[ ! -f "CMakeLists.txt" ]]; then
        echo "Error: No se encontró CMakeLists.txt en el directorio actual"
        echo "Asegúrate de estar en el directorio raíz del proyecto img2pass"
        exit 1
    fi

    # Crear directorio build si no existe
    if [[ ! -d "build" ]]; then
        echo "Creando directorio build..."
        mkdir build
    fi

    echo "Configurando proyecto con CMake..."
    cd build

    # Configurar con CMake
    if ! cmake ..; then
        echo "Error: Fallo en la configuración de CMake"
        cd ..
        exit 1
    fi

    echo "Compilando proyecto..."
    # Intentar make primero, luego cmake --build como fallback
    if command -v make &> /dev/null; then
        if ! make; then
            echo "Error: Fallo en la compilación con make"
            cd ..
            exit 1
        fi
    else
        if ! cmake --build .; then
            echo "Error: Fallo en la compilación con cmake --build"
            cd ..
            exit 1
        fi
    fi

    cd ..
    echo "Compilación completada"
}

# Copiar binario al directorio de instalación
install_binary() {
    local source_binary=""

    # Buscar el binario compilado
    if [[ -f "build/$BINARY_NAME" ]]; then
        source_binary="build/$BINARY_NAME"
    elif [[ -f "$BINARY_NAME" ]]; then
        source_binary="$BINARY_NAME"
    else
        echo "Binario no encontrado. Compilando proyecto..."
        build_project

        # Verificar nuevamente después de compilar
        if [[ -f "build/$BINARY_NAME" ]]; then
            source_binary="build/$BINARY_NAME"
        else
            echo "Error: No se pudo generar el binario"
            exit 1
        fi
    fi

    BINARY_PATH="$INSTALL_DIR/$BINARY_NAME"

    echo "Copiando binario..."
    echo "   Desde: $source_binary"
    echo "   Hacia: $BINARY_PATH"

    cp "$source_binary" "$BINARY_PATH"
    chmod +x "$BINARY_PATH"

    echo "Binario instalado correctamente"
}

# Verificar si el directorio está en PATH
check_path() {
    if echo "$PATH" | grep -q "$INSTALL_DIR"; then
        echo "$INSTALL_DIR ya está en PATH"
        return 0
    else
        echo "$INSTALL_DIR no está en PATH"
        return 1
    fi
}

# Agregar directorio al PATH
add_to_path() {
    if [[ -z "$SHELL_CONFIG" ]]; then
        echo "No se pudo determinar el archivo de configuración del shell"
        return 1
    fi

    echo "Agregando $INSTALL_DIR al PATH en $SHELL_CONFIG"

    # Crear archivo de configuración si no existe
    touch "$SHELL_CONFIG"

    # Verificar si ya existe la entrada
    if grep -q "export PATH.*$INSTALL_DIR" "$SHELL_CONFIG"; then
        echo "La entrada PATH ya existe en $SHELL_CONFIG"
    else
        echo "" >> "$SHELL_CONFIG"
        echo "# img2pass binary path" >> "$SHELL_CONFIG"
        echo "export PATH=\"\$PATH:$INSTALL_DIR\"" >> "$SHELL_CONFIG"
        echo "PATH actualizado en $SHELL_CONFIG"
    fi
}

# Función principal
main() {
    echo "Iniciando instalación de $BINARY_NAME"
    echo "=================================================="

    detect_shell

    if find_existing_binary; then
        read -p "¿Reemplazar el binario existente? (y/N): " -n 1 -r
        echo
        if [[ ! $REPLY =~ ^[Yy]$ ]]; then
            echo "Instalación cancelada"
            exit 0
        fi
    fi

    create_install_dir
    install_binary

    if ! check_path; then
        read -p "¿Agregar $INSTALL_DIR al PATH? (Y/n): " -n 1 -r
        echo
        if [[ $REPLY =~ ^[Nn]$ ]]; then
            echo "PATH no modificado. Ejecuta manualmente:"
            echo "   export PATH=\"\$PATH:$INSTALL_DIR\""
        else
            add_to_path
        fi
    fi

    echo ""
    echo "Instalación completada!"
    echo "=================================================="
    echo "Binario instalado en: $BINARY_PATH"
    echo "Reinicia tu terminal o ejecuta: source $SHELL_CONFIG"
    echo "Ahora puedes ejecutar: $BINARY_NAME"
    echo ""
}

# Ejecutar si es llamado directamente
if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    main "$@"
fi