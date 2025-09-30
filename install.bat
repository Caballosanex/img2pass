@echo off
REM Script de instalación automática para img2pass en Windows
REM Maneja la instalación del binario y actualización del PATH

setlocal EnableDelayedExpansion

set "BINARY_NAME=img2pass.exe"
set "INSTALL_DIR=%USERPROFILE%\.local\bin"
set "BINARY_PATH=%INSTALL_DIR%\%BINARY_NAME%"

echo Iniciando instalación de %BINARY_NAME%
echo ==================================================

REM Función para compilar el proyecto
:build_project
if not exist "CMakeLists.txt" (
    echo Error: No se encontró CMakeLists.txt en el directorio actual
    echo Asegúrate de estar en el directorio raíz del proyecto img2pass
    pause
    exit /b 1
)

if not exist "build" (
    echo Creando directorio build...
    mkdir build
)

echo Configurando proyecto con CMake...
cd build
cmake .. -G "MinGW Makefiles"
if %errorlevel% neq 0 (
    echo Error: Fallo en la configuración de CMake
    cd ..
    pause
    exit /b 1
)

echo Compilando proyecto...
cmake --build .
if %errorlevel% neq 0 (
    echo Error: Fallo en la compilación
    cd ..
    pause
    exit /b 1
)

cd ..
echo Compilación completada
goto :eof

REM Verificar si el binario existe
if not exist "build\%BINARY_NAME%" (
    if not exist "%BINARY_NAME%" (
        echo Binario no encontrado. Compilando proyecto...
        call :build_project

        if not exist "build\%BINARY_NAME%" (
            echo Error: No se pudo generar el binario
            pause
            exit /b 1
        ) else (
            set "SOURCE_BINARY=build\%BINARY_NAME%"
        )
    ) else (
        set "SOURCE_BINARY=%BINARY_NAME%"
    )
) else (
    set "SOURCE_BINARY=build\%BINARY_NAME%"
)

REM Buscar binario existente en PATH
where %BINARY_NAME% >nul 2>&1
if %errorlevel% equ 0 (
    echo Binario existente encontrado en PATH
    choice /c YN /m "¿Reemplazar el binario existente?"
    if errorlevel 2 goto :cancel
) else (
    echo No se encontró binario existente en PATH
)

REM Crear directorio de instalación
if not exist "%INSTALL_DIR%" (
    echo Creando directorio de instalación: %INSTALL_DIR%
    mkdir "%INSTALL_DIR%"
)

REM Copiar binario
echo Copiando binario...
echo    Desde: %SOURCE_BINARY%
echo    Hacia: %BINARY_PATH%
copy "%SOURCE_BINARY%" "%BINARY_PATH%" >nul
if %errorlevel% neq 0 (
    echo Error copiando el binario
    pause
    exit /b 1
)
echo Binario instalado correctamente

REM Verificar si está en PATH
echo %PATH% | find /i "%INSTALL_DIR%" >nul
if %errorlevel% equ 0 (
    echo %INSTALL_DIR% ya está en PATH
    goto :success
)

echo %INSTALL_DIR% no está en PATH
choice /c YN /m "¿Agregar al PATH del sistema?"
if errorlevel 2 goto :manual_path

REM Agregar al PATH del usuario
echo Agregando %INSTALL_DIR% al PATH del usuario...
for /f "tokens=2*" %%a in ('reg query "HKCU\Environment" /v PATH 2^>nul') do set "CURRENT_PATH=%%b"
if "!CURRENT_PATH!"=="" (
    set "NEW_PATH=%INSTALL_DIR%"
) else (
    echo !CURRENT_PATH! | find /i "%INSTALL_DIR%" >nul
    if errorlevel 1 (
        set "NEW_PATH=!CURRENT_PATH!;%INSTALL_DIR%"
    ) else (
        echo El directorio ya existe en PATH del registro
        goto :success
    )
)

reg add "HKCU\Environment" /v PATH /t REG_EXPAND_SZ /d "!NEW_PATH!" /f >nul
if %errorlevel% equ 0 (
    echo PATH actualizado en el registro
) else (
    echo Error actualizando PATH en el registro
    goto :manual_path
)

goto :success

:manual_path
echo PATH no modificado automáticamente
echo Agrega manualmente al PATH: %INSTALL_DIR%
echo O ejecuta desde: %BINARY_PATH%
goto :success

:cancel
echo Instalación cancelada
pause
exit /b 0

:success
echo.
echo Instalación completada!
echo ==================================================
echo Binario instalado en: %BINARY_PATH%
echo Reinicia tu terminal o cierra/abre una nueva
echo Ahora puedes ejecutar: %BINARY_NAME%
echo.
pause