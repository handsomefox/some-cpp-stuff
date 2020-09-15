#pragma once
#include "pch.h"
template <typename T> class Matrix {
    T** m_Matrix;
    size_t m_Size = 0;

public:
    Matrix(size_t& size) {
        m_Size = size;
        m_Matrix = new T * [size];

        for (size_t i = 0; i < size; ++i)
            m_Matrix[i] = new T[size];
    }

    T** generate() {
        srand(time(0));

        for (size_t i = 0; i < m_Size; ++i)
            for (size_t j = 0; j < m_Size; ++j)
                m_Matrix[i][j] = 1 + rand() % 10;

        return m_Matrix;
    }

    const size_t size() { return m_Size; }

    T** getMatrix() { return m_Matrix; }
};

template <typename T> void MultiplyMatrix(Matrix<T>& mt1, Matrix<T>& mt2, size_t& size) {
    T** temp1 = mt1.getMatrix();
    T** temp2 = mt2.getMatrix();

    T** res = new T * [size];
    for (size_t i = 0; i < size; ++i)
        res[i] = new T[size];

    for (size_t i = 0; i < size; ++i)
        for (size_t k = 0; k < size; ++k)
            for (size_t j = 0; j < size; ++j)
                res[i][j] = temp1[i][k] * temp2[k][j];

}
template <typename T> void MultiplyMatrix(T**& mt1, T**& mt2, size_t& size) {
    T** res = new T * [size];

    for (size_t i = 0; i < size; ++i)
        res[i] = new T[size];
    for (size_t i = 0; i < size; ++i)
        for (size_t k = 0; k < size; ++k)
            for (size_t j = 0; j < size; ++j)
                res[i][j] = mt1[i][k] * mt2[k][j];
}
template <typename T> double MatrixBenchmark(T**& mt1, T**& mt2, size_t& size) {
    double start, end;
    start = omp_get_wtime();
    MultiplyMatrix(mt1, mt2, size);
    end = omp_get_wtime();

    return end - start;
}
template <typename T>
double MatrixBenchmark(Matrix<T>& mt1, Matrix<T>& mt2, size_t& size) {
    double start, end;
    start = omp_get_wtime();
    MultiplyMatrix(mt1, mt2, size);
    end = omp_get_wtime();

    return end - start;
}
template <typename T> T** CreateFilledMatrix(T** matrix, size_t& size) {
    srand(time(0));
    matrix = new T * [size];

    for (size_t i = 0; i < size; ++i)
        matrix[i] = new T[size];

    for (size_t i = 0; i < size; ++i)
        for (size_t j = 0; j < size; ++j)
            matrix[i][j] = 1 + rand() % 10;

    return matrix;
}
