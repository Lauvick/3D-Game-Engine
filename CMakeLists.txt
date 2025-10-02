cmake_minimum_required(VERSION 3.15)
project(GameEngine VERSION 1.0 LANGUAGES CXX C)

# Configuration C++ moderne
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options de compilation strictes
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Trouver les bibliothèques
find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)

# Créer l'exécutable
add_executable(GameEngine
    src/main.cpp
    external/src/glad.c
)

# Headers
target_include_directories(GameEngine PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/include
)

# Linker les bibliothèques
target_link_libraries(GameEngine PRIVATE
    glm::glm
    glfw
)

# Afficher les informations de build
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")

# Instructions d'installation (optionnel)
install(TARGETS GameEngine
    RUNTIME DESTINATION bin
)