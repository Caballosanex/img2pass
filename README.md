# IMG2PASS - Generador de Contraseñas desde Imágenes

## 📋 Descripción

IMG2PASS es un generador de contraseñas determinístico que utiliza imágenes, claves privadas RSA y algoritmos de hash para crear contraseñas únicas y reproducibles de 20 caracteres. La principal ventaja de este sistema es que **la misma imagen + clave privada + algoritmo de hash siempre producirá exactamente la misma contraseña**, lo que permite recuperar contraseñas sin necesidad de almacenarlas.

## ✨ Características

- **🔒 Generación determinística**: Mismos inputs = misma contraseña siempre
- **🔑 Claves RSA robustas**: Soporte para claves de 2048, 3072 y 4096 bits
- **📷 Cualquier formato de imagen**: Compatible con JPG, PNG, BMP, GIF, etc.
- **🛡️ Múltiples algoritmos hash**: MD5, SHA1, SHA256, SHA512
- **🖥️ Multiplataforma**: Windows, Linux, macOS
- **📁 Gestión automática**: Organización automática de claves e imágenes
- **🎯 Interfaz amigable**: Menús interactivos paso a paso
- **🔐 Contraseñas seguras**: 20 caracteres con letras, números y símbolos

## 🔒 ¿Cómo funciona?

IMG2PASS utiliza criptografía moderna para crear contraseñas determinísticas mediante el siguiente proceso:

1. **Lectura de imagen**: Lee todos los bytes de la imagen original
2. **Firma digital**: Firma los bytes de la imagen con tu clave privada RSA
3. **Hash criptográfico**: Aplica el algoritmo hash seleccionado a la firma digital
4. **Conversión a contraseña**: Convierte el hash resultante en una contraseña de exactamente 20 caracteres

**El proceso es 100% determinístico**: La misma combinación de imagen + clave privada + algoritmo hash siempre generará la misma contraseña. Esto significa que puedes "recuperar" cualquier contraseña simplemente volviendo a usar los mismos tres componentes.

## 📦 Requisitos del Sistema

### Dependencias
- **OpenSSL 1.1.1 o superior**
- **CMake 3.12 o superior**
- **Compilador C99** (GCC, Clang, MSVC)

### Compatibilidad
- ✅ **Linux** (Ubuntu 18.04+, CentOS 7+, etc.)
- ✅ **macOS** (10.14+)
- ✅ **Windows** (Windows 10+)

## 🚀 Instalación

### Compilación Rápida

#### Linux/Ubuntu
```bash
# Instalar dependencias
sudo apt update
sudo apt install build-essential cmake libssl-dev

# Clonar y compilar
git clone <repository-url>
cd img2pass
mkdir build && cd build
cmake ..
make

# Instalación automática
./install.sh
```

#### macOS
```bash
# Instalar dependencias (usando Homebrew)
brew install cmake openssl

# Clonar y compilar
git clone <repository-url>
cd img2pass
mkdir build && cd build
cmake ..
make

# Instalación automática
./install.sh
```

#### Windows (MinGW)
```powershell
# Instalar dependencias con Chocolatey
choco install mingw openssl cmake

# Compilar
git clone <repository-url>
cd img2pass
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
cmake --build . --config Release

# Instalación automática
install.bat
```

### Instalación Automática Post-Build

Para instalar automáticamente después de compilar:

```bash
# Linux/macOS
cmake .. -DAUTO_INSTALL=ON
make

# Windows
cmake .. -DAUTO_INSTALL=ON -G "MinGW Makefiles"
cmake --build .
```

### Características de Instalación

✅ **Detección automática del PATH**
✅ **Instalación en ~/.local/bin (Unix) o %USERPROFILE%\.local\bin (Windows)**
✅ **Actualización automática del shell config**
✅ **Soporte para múltiples shells** (bash, zsh, fish)
✅ **Detección de binarios existentes**
✅ **Interfaz user-friendly con drag & drop de archivos**

## 📖 Guía de Uso

### Primera ejecución

Al ejecutar IMG2PASS por primera vez, verás esta pantalla:

```
=== IMG2PASS - Primera Configuración ===
¡Bienvenido! Este parece ser tu primer uso de img2pass.

Selecciona dónde crear el directorio de trabajo:
1. Directorio actual: /home/usuario/img2pass
2. Directorio home: /home/usuario
3. Ruta personalizada
Seleccione (1-3):
```

**Recomendación**: Selecciona opción 2 (directorio home) para facilitar el acceso desde cualquier ubicación.

### Generando tu primera clave privada

1. En el menú principal, selecciona **opción 1**
2. Ingresa un nombre descriptivo (ej: "trabajo", "personal", "email")
3. Selecciona el nivel de seguridad:
   - **RSA 2048 bits**: Rápido, seguridad estándar
   - **RSA 3072 bits**: Balance seguridad/velocidad
   - **RSA 4096 bits**: Máxima seguridad

```
=== Generar Nueva Clave Privada ===
Nombre para la clave (sin extensión): trabajo
Nivel de encriptación:
1. RSA 2048 bits
2. RSA 3072 bits  ← Recomendado
3. RSA 4096 bits
Seleccione: 2

Generando clave privada trabajo de 3072 bits...
✓ Clave creada: /home/usuario/img2pass_vault/claves/trabajo.pem
```

### Creando una contraseña

1. Selecciona **opción 2** en el menú principal
2. Proporciona la ruta a tu imagen
3. Selecciona tu clave privada
4. Elige el algoritmo hash
5. Asigna un nombre a la copia de la imagen

```
=== Generar Contraseña ===
Ruta de la imagen: /home/usuario/Imágenes/foto_vacaciones.jpg
Ruta de la clave privada: /home/usuario/img2pass_vault/claves/trabajo.pem

Algoritmos disponibles:
1. MD5
2. SHA1
3. SHA256  ← Recomendado
4. SHA512
Seleccione: 3

Nombre para la copia de imagen: gmail_trabajo
✓ Copia creada: ./img2pass_vault/imagenes/gmail_trabajo.jpg

Generando contraseña...
========================================
Contraseña: K8mN#pQ2vX9s&L4jD7wE
========================================
```

### Recuperando una contraseña existente

Para recuperar una contraseña previamente generada:

1. Usa **la misma imagen** (desde el vault o la original)
2. Usa **la misma clave privada**
3. Selecciona **el mismo algoritmo hash**

**Importante**: Cualquier cambio en estos tres elementos producirá una contraseña diferente.

## 📁 Estructura de Archivos

IMG2PASS organiza automáticamente tus archivos en la siguiente estructura:

```
img2pass_vault/
├── claves/
│   ├── trabajo.pem      # Tus claves privadas RSA
│   ├── personal.pem
│   └── gaming.pem
└── imagenes/
    ├── gmail_trabajo.jpg    # Copias de imágenes con nombres descriptivos
    ├── facebook_personal.png
    └── steam_gaming.gif

.img2pass_config             # Archivo de configuración (mismo directorio que ejecutable)
```

## 🔐 Seguridad

### ¿Por qué es seguro?

1. **Firmas RSA**: Utiliza criptografía de clave pública robusta
2. **Hash criptográficos**: Los algoritmos SHA son resistentes a colisiones
3. **No almacenamiento**: Las contraseñas nunca se guardan en disco
4. **Determinismo**: Imposible de reversar sin todos los componentes

### Fortalezas del sistema

- **Sin punto único de falla**: Necesitas imagen + clave + algoritmo
- **Resistente a ataques de diccionario**: Las contraseñas no siguen patrones
- **Escalable**: Genera ilimitadas contraseñas únicas
- **Portátil**: Funciona en cualquier sistema con OpenSSL

## ⚠️ Consideraciones Importantes

### 🚨 Backup crítico
- **Haz backup de tu directorio `img2pass_vault` completo**
- **Guarda copias de seguridad en ubicaciones separadas**
- **Considera usar almacenamiento cifrado para los backups**

### ⚡ Pérdida de datos
- **Perder la clave privada = perder TODAS las contraseñas que usan esa clave**
- **Perder una imagen = perder la contraseña específica de esa imagen**
- **Olvidar el algoritmo hash usado = no poder recuperar la contraseña**

### 🔄 Cambios que afectan contraseñas
- **Renombrar archivos de imagen no afecta** (se usa el contenido, no el nombre)
- **Modificar la imagen (recorte, filtros, etc.) genera contraseña diferente**
- **Usar diferente algoritmo hash genera contraseña diferente**

### 💡 Mejores prácticas
1. **Documenta tus combinaciones**: Anota qué imagen y algoritmo usaste para cada servicio
2. **Usa nombres descriptivos**: "gmail_trabajo" es mejor que "img1"
3. **Backup regular**: Programa copias de seguridad automáticas
4. **Prueba la recuperación**: Verifica que puedes regenerar contraseñas importantes

## 🛠️ Compilación desde Código Fuente

### Requisitos de desarrollo
```bash
# Ubuntu/Debian
sudo apt install build-essential cmake libssl-dev git

# CentOS/RHEL
sudo yum groupinstall "Development Tools"
sudo yum install cmake openssl-devel git

# macOS
brew install cmake openssl git
```

### Proceso de compilación
```bash
# Clonar repositorio
git clone <repository-url>
cd img2pass

# Crear directorio de build
mkdir build && cd build

# Configurar con CMake
cmake ..

# Compilar
make -j$(nproc)

# Instalar (opcional)
sudo make install
```

### Verificar instalación
```bash
./img2pass
# Debe mostrar el menú de configuración inicial
```

## 🤝 Contribuciones

### Reportar problemas
- Usa GitHub Issues para reportar bugs
- Incluye información del sistema operativo
- Proporciona pasos para reproducir el problema

### Contribuir código
1. Fork del repositorio
2. Crea rama para tu feature (`git checkout -b feature/nueva-funcionalidad`)
3. Commit con mensajes descriptivos
4. Push a tu rama (`git push origin feature/nueva-funcionalidad`)
5. Abre Pull Request

## 📄 Licencia

Este proyecto está licenciado bajo la Licencia MIT. Ver archivo `LICENSE` para detalles completos.

## ❓ FAQ

### ¿Puedo usar la misma imagen para múltiples contraseñas?
**Sí**, siempre que uses diferentes claves privadas o algoritmos hash. Cada combinación única generará una contraseña diferente.

### ¿Qué pasa si pierdo mi clave privada?
**No hay forma de recuperar las contraseñas** generadas con esa clave. Es fundamental hacer backups regulares de tus claves privadas.

### ¿Puedo cambiar el tamaño de la contraseña?
Actualmente, todas las contraseñas tienen exactamente 20 caracteres. Esta limitación asegura compatibilidad y consistencia.

### ¿Es seguro usar MD5?
MD5 está incluido para compatibilidad, pero **recomendamos usar SHA256 o SHA512** para nuevas contraseñas debido a las vulnerabilidades conocidas de MD5.

### ¿Funciona con imágenes muy grandes?
Sí, el programa puede procesar imágenes de cualquier tamaño. Sin embargo, imágenes muy grandes (>100MB) pueden tomar más tiempo en procesarse.

### ¿Puedo usar el mismo programa en diferentes computadoras?
**Sí**, siempre que copies tu directorio `img2pass_vault` completo a la nueva computadora. Las contraseñas generadas serán idénticas.

### ¿Qué formatos de imagen son compatibles?
Cualquier formato que pueda leerse como archivo binario: JPG, PNG, GIF, BMP, TIFF, WebP, etc. El programa lee los bytes directamente, no interpreta el formato.

---

**IMG2PASS - Genera contraseñas únicas y recuperables usando tus propias imágenes** 🔐✨