#include "Matrix.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <utility>

namespace {
constexpr double kEpsilon = 1e-9;

std::size_t index_of(const Matrix &matrix, std::size_t x, std::size_t y) {
    auto [rows, cols] = matrix.size();
    if (x >= static_cast<std::size_t>(rows) || y >= static_cast<std::size_t>(cols)) {
        throw std::out_of_range("[MATRIX] Índice fuera de rango");
    }
    return x * static_cast<std::size_t>(cols) + y;
}
}  // namespace

Matrix::Matrix() : n(0), m(0), mat(nullptr) {}

/**
 * @brief Construye un vector columna de tamaño n inicializado en cero.
 * @param n Número de filas del vector.
 * @throws std::invalid_argument Si n es negativo.
 */
Matrix::Matrix(int n) : n(n), m(1) {
    if (n < 0) {
        throw std::invalid_argument("[MATRIX] Dimensión inválida");
    }
    if (n > 0) {
        mat = std::make_unique<double[]>(static_cast<std::size_t>(n));
        std::fill_n(mat.get(), static_cast<std::size_t>(n), 0.0);
    }
}

/**
 * @brief Construye una matriz de tamaño n x m inicializada en cero.
 * @param n Número de filas.
 * @param m Número de columnas.
 * @throws std::invalid_argument Si alguna dimensión es negativa.
 */
Matrix::Matrix(int n, int m) : n(n), m(m) {
    if (n < 0 || m < 0) {
        throw std::invalid_argument("[MATRIX] Dimensión inválida");
    }
    const auto count = static_cast<std::size_t>(n) * static_cast<std::size_t>(m);
    if (count > 0) {
        mat = std::make_unique<double[]>(count);
        std::fill_n(mat.get(), count, 0.0);
    }
}

/**
 * @brief Carga una matriz desde un archivo de texto.
 *
 * El archivo debe contener primero las dimensiones y después los valores
 * almacenados por filas.
 *
 * @param filename Ruta del archivo de entrada.
 * @throws std::runtime_error Si el archivo no puede abrirse o su formato es inválido.
 */
Matrix::Matrix(const std::string &filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("[MATRIX] No se pudo abrir el archivo");
    }

    file >> n >> m;
    if (!file || n < 0 || m < 0) {
        throw std::runtime_error("[MATRIX] Formato de archivo inválido");
    }

    const auto count = static_cast<std::size_t>(n) * static_cast<std::size_t>(m);
    if (count > 0) {
        mat = std::make_unique<double[]>(count);
        for (std::size_t i = 0; i < count; ++i) {
            if (!(file >> mat[i])) {
                throw std::runtime_error("[MATRIX] Datos insuficientes en el archivo");
            }
        }
    }
}

/**
 * @brief Crea una copia profunda de otra matriz.
 * @param matrix Matriz origen.
 */
Matrix::Matrix(const Matrix &matrix) {
    n = matrix.n;
    m = matrix.m;
    const auto count = static_cast<std::size_t>(n) * static_cast<std::size_t>(m);
    if (count > 0) {
        mat = std::make_unique<double[]>(count);
        std::copy_n(matrix.mat.get(), count, mat.get());
    }
}

Matrix::~Matrix() = default;

/**
 * @brief Accede a un elemento de la matriz para escritura.
 * @param x Índice de fila.
 * @param y Índice de columna.
 * @return Referencia al elemento solicitado.
 * @throws std::out_of_range Si el índice está fuera de rango.
 */
double &Matrix::operator[](std::size_t x, std::size_t y) {
    return mat[index_of(*this, x, y)];
}

/**
 * @brief Accede a un elemento de la matriz para lectura.
 * @param x Índice de fila.
 * @param y Índice de columna.
 * @return Referencia constante al elemento solicitado.
 * @throws std::out_of_range Si el índice está fuera de rango.
 */
const double &Matrix::operator[](std::size_t x, std::size_t y) const {
    return mat[index_of(*this, x, y)];
}

/**
 * @brief Rellena todos los elementos de la matriz con el mismo valor.
 * @param value Valor a asignar a cada celda.
 */
void Matrix::fill(double value) {
    const auto count = static_cast<std::size_t>(n) * static_cast<std::size_t>(m);
    if (count > 0) {
        std::fill_n(mat.get(), count, value);
    }
}

/**
 * @brief Devuelve las dimensiones de la matriz.
 * @return Tupla con el número de filas y columnas.
 */
std::tuple<int, int> Matrix::size() const {
    return std::make_tuple(n, m);
}

/**
 * @brief Imprime la matriz en el flujo de salida.
 * @param os Flujo de salida.
 * @param mat Matriz a imprimir.
 * @return El flujo de salida recibido.
 */
std::ostream &operator<<(std::ostream &os, const Matrix &mat) {
    auto [rows, cols] = mat.size();
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            os << mat[static_cast<std::size_t>(i), static_cast<std::size_t>(j)];
            if (j + 1 < cols) {
                os << ' ';
            }
        }
        if (i + 1 < rows) {
            os << '\n';
        }
    }
    return os;
}

/**
 * @brief Guarda la matriz en un archivo de texto.
 * @param filename Ruta del archivo de salida.
 * @throws std::runtime_error Si el archivo no puede abrirse.
 */
void Matrix::save_to_file(const std::string &filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("[MATRIX] No se pudo abrir el archivo de salida");
    }

    file << n << ' ' << m << '\n';
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            file << (*this)[static_cast<std::size_t>(i), static_cast<std::size_t>(j)];
            if (j + 1 < m) {
                file << ' ';
            }
        }
        if (i + 1 < n) {
            file << '\n';
        }
    }
}

/**
 * @brief Compara dos matrices con tolerancia para valores double.
 * @param matrix Matriz con la que se compara.
 * @return true si ambas matrices tienen las mismas dimensiones y valores.
 */
bool Matrix::operator==(const Matrix &matrix) const {
    if (n != matrix.n || m != matrix.m) {
        return false;
    }

    const auto count = static_cast<std::size_t>(n) * static_cast<std::size_t>(m);
    for (std::size_t i = 0; i < count; ++i) {
        if (std::fabs(mat[i] - matrix.mat[i]) > kEpsilon) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Comprueba si dos matrices son diferentes.
 * @param matrix Matriz con la que se compara.
 * @return true si las matrices difieren.
 */
bool Matrix::operator!=(const Matrix &matrix) const {
    return !(*this == matrix);
}

/**
 * @brief Asigna el contenido de otra matriz mediante copia profunda.
 * @param matrix Matriz fuente.
 * @return Referencia a la matriz actual.
 */
Matrix &Matrix::operator=(const Matrix &matrix) {
    if (this == &matrix) {
        return *this;
    }

    Matrix copy(matrix);
    std::swap(n, copy.n);
    std::swap(m, copy.m);
    std::swap(mat, copy.mat);
    return *this;
}

/**
 * @brief Suma dos matrices y devuelve una nueva matriz con el resultado.
 * @param matrix Matriz a sumar.
 * @return Nueva matriz con la suma elemento a elemento.
 * @throws std::logic_error Si las dimensiones no son compatibles.
 */
Matrix Matrix::operator+(const Matrix &matrix) const {
    Matrix result(*this);
    result += matrix;
    return result;
}

/**
 * @brief Escala todos los elementos de la matriz por un factor.
 * @param a Factor de escala.
 * @return Referencia a la matriz actual.
 */
Matrix &Matrix::operator*=(double a) {
    const auto count = static_cast<std::size_t>(n) * static_cast<std::size_t>(m);
    for (std::size_t i = 0; i < count; ++i) {
        mat[i] *= a;
    }
    return *this;
}

/**
 * @brief Suma otra matriz elemento a elemento.
 * @param matrix Matriz a sumar.
 * @return Referencia a la matriz actual.
 * @throws std::logic_error Si las dimensiones no son compatibles.
 */
Matrix &Matrix::operator+=(const Matrix &matrix) {
    if (n != matrix.n || m != matrix.m) {
        throw std::logic_error("[MATRIX] Dimensiones incompatibles para suma");
    }

    const auto count = static_cast<std::size_t>(n) * static_cast<std::size_t>(m);
    for (std::size_t i = 0; i < count; ++i) {
        mat[i] += matrix.mat[i];
    }
    return *this;
}

/**
 * @brief Resta dos matrices y devuelve una nueva matriz con el resultado.
 * @param matrix Matriz a restar.
 * @return Nueva matriz con la resta elemento a elemento.
 * @throws std::logic_error Si las dimensiones no son compatibles.
 */
Matrix Matrix::operator-(const Matrix &matrix) const {
    Matrix result(*this);
    result -= matrix;
    return result;
}

/**
 * @brief Resta otra matriz elemento a elemento.
 * @param matrix Matriz a restar.
 * @return Referencia a la matriz actual.
 * @throws std::logic_error Si las dimensiones no son compatibles.
 */
Matrix &Matrix::operator-=(const Matrix &matrix) {
    if (n != matrix.n || m != matrix.m) {
        throw std::logic_error("[MATRIX] Dimensiones incompatibles para resta");
    }

    const auto count = static_cast<std::size_t>(n) * static_cast<std::size_t>(m);
    for (std::size_t i = 0; i < count; ++i) {
        mat[i] -= matrix.mat[i];
    }
    return *this;
}

/**
 * @brief Intercambia filas por columnas de la matriz.
 * @return Referencia a la matriz actual transpuesta.
 */
Matrix &Matrix::transpose() {
    const int new_rows = m;
    const int new_cols = n;
    Matrix result(new_rows, new_cols);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            result[static_cast<std::size_t>(j), static_cast<std::size_t>(i)] =
                    (*this)[static_cast<std::size_t>(i), static_cast<std::size_t>(j)];
        }
    }
    *this = result;
    return *this;
}