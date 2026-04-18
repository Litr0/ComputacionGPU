cmake_minimum_required(VERSION 3.20)
project(Tarea1_Matrix)

# Requerimiento: Utilizar el estándar C++23 
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()

add_library(MatrixLib Matrix.cpp Matrix.h)

# Configuración de Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
)
FetchContent_MakeAvailable(googletest)
include(GoogleTest)

# Ejecutable para los tests
add_executable(matrix_tests tests/test_matrix.cpp)
target_link_libraries(matrix_tests PRIVATE MatrixLib gtest_main)
gtest_discover_tests(matrix_tests)

# Ejecutable de ejemplo
add_executable(example example.cpp)
target_link_libraries(example PRIVATE MatrixLib)