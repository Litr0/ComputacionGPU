#include "Matrix.h"
#include <iostream>
#include <stdexcept>

Matrix::Matrix() : n(0), m(0), mat(nullptr) {}

Matrix::Matrix(int n) : n(n), m(1) {
    // TODO: Inicializar vector [1xn] en ceros
}

Matrix::Matrix(int n, int m) : n(n), m(m) {
    // TODO: Inicializar matriz [nxm] en ceros
}

Matrix::Matrix(const std::string &filename) {
    // TODO: Leer desde archivo
}

Matrix::Matrix(const Matrix &matrix) {
    // TODO: Copy constructor
}

Matrix::~Matrix() {
    // TODO: Destructor
}

double &Matrix::operator[](std::size_t x, std::size_t y) {
    // TODO: Set value
    throw std::logic_error("[MATRIX] Setter no implementado");
}

const double &Matrix::operator[](std::size_t x, std::size_t y) const {
    // TODO: Get value
    throw std::logic_error("[MATRIX] Getter no implementado");
}

void Matrix::fill(double value) {
    // TODO: Llenar la matriz con un valor
}

std::tuple<int, int> Matrix::size() const {
    return std::make_tuple(n, m);
}

std::ostream &operator<<(std::ostream &os, const Matrix &mat) {
    // TODO: Imprimir en consola
    return os;
}

void Matrix::save_to_file(const std::string &filename) const {
    // TODO: Guardar a archivo
}

bool Matrix::operator==(const Matrix &matrix) const {
    // TODO: Equal operator
    return false;
}

bool Matrix::operator!=(const Matrix &matrix) const {
    return !(*this == matrix);
}

Matrix &Matrix::operator=(const Matrix &matrix) {
    // TODO: Assignment operator
    return *this;
}

Matrix &Matrix::operator*=(const Matrix &matrix) {
    // TODO: Multiplicación matricial
    return *this;
}

Matrix &Matrix::operator*=(double a) {
    // TODO: Multiplicar por constante
    return *this;
}

Matrix &Matrix::operator+=(const Matrix &matrix) {
    // TODO: Suma
    return *this;
}

Matrix &Matrix::operator-=(const Matrix &matrix) {
    // TODO: Resta
    return *this;
}

Matrix &Matrix::transpose() {
    // TODO: Traspuesta
    return *this;
}