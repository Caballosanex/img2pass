cmake_minimum_required(VERSION 3.12)
project(img2pass VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Find OpenSSL
find_package(OpenSSL REQUIRED)

# Add executable
add_executable(img2pass
    src/main.c
    src/config.c
    src/directory.c
    src/crypto.c
    src/password.c
    src/ui.c
    src/utils.c
)

# Include directories
target_include_directories(img2pass PRIVATE include)

# Link OpenSSL
target_link_libraries(img2pass PRIVATE OpenSSL::SSL OpenSSL::Crypto)

# Platform-specific settings
if(WIN32)
    target_compile_definitions(img2pass PRIVATE _WIN32)
elseif(APPLE)
    target_compile_definitions(img2pass PRIVATE __APPLE__)
elseif(UNIX)
    target_compile_definitions(img2pass PRIVATE __linux__)
endif()

# Compiler flags
if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Clang")
    target_compile_options(img2pass PRIVATE -Wall -Wextra -O2)
endif()

# Install target
install(TARGETS img2pass DESTINATION bin)