#pragma once
#include "pch.h"
template <typename T> class Matrix {
    T** m_Matrix;
    size_t m_Size = 0;

public:
    Matrix(size_t size) {
        m_Size = size;
        m_Matrix = new T * [size];
        for (size_t i = 0; i < size; ++i)
            m_Matrix[i] = new T[size];
    }
    const void print() {
        for (size_t i = 0; i < m_Size; ++i)
            for (size_t j = 0; j < m_Size; ++j) {
                std::cout << m_Matrix[i][j] << " ";
                std::cout << std::endl;
            }
    }
    T** generate() {
        srand(time(0));
        for (size_t i = 0; i < m_Size; ++i)
            for (size_t j = 0; j < m_Size; ++j)
                m_Matrix[i][j] = rand() % 100;
        return m_Matrix;
    }
    const size_t size() { return m_Size; };
    const T** getMatrix() { return m_Matrix; };
    const size_t capacity() { return m_Size * m_Size; };
    const T operator[](const std::pair<int, int>& Index) const {
        return m_Matrix[Index.first][Index.second];
    };
    T operator[](const std::pair<int, int>& Index) {
        return m_Matrix[Index.first][Index.second];
    };
};
template <typename T> T** MultiplyMatrix(Matrix<T> mt1, Matrix<T> mt2) {
    size_t size = mt1.size();
    T** res = new T * [size];
    for (size_t i = 0; i < size; ++i)
        res[i] = new T[size];

    for (size_t i = 0; i < size; ++i)
        for (size_t j = 0; j < size; ++j) {
            res[i][j] = 0;
            for (size_t k = 0; k < size; ++k)
                res[i][j] += mt1[{i, k}] * mt2[{k, j}];
        }
    return res;
}
template <typename T> T** MultiplyMatrix(T** mt1, T** mt2, size_t size) {
    T** res = new T * [size];
    for (size_t i = 0; i < size; ++i)
        res[i] = new T[size];

    for (size_t i = 0; i < size; ++i)
        for (size_t j = 0; j < size; ++j) {
            res[i][j] = 0;
            for (size_t k = 0; k < size; ++k)
                res[i][j] += mt1[i][k] * mt2[k][j];
        }
    return res;
}
template <typename T> double MatrixBenchmark(T** mt1, T** mt2, size_t size) {
    double start, end;
    start = omp_get_wtime();
    MultiplyMatrix(mt1, mt2, size);
    end = omp_get_wtime();
    return end - start;
}
template <typename T> double MatrixBenchmark(Matrix<T> mt1, Matrix<T> mt2) {
    double start, end;
    start = omp_get_wtime();
    MultiplyMatrix(mt1, mt2);
    end = omp_get_wtime();
    return end - start;
}
TYPE** CreateFilledMatrix(size_t size) {
    srand(time(0));
    TYPE** matrix = new TYPE * [size];
    for (size_t i = 0; i < size; ++i)
        matrix[i] = new TYPE[size];
    for (size_t i = 0; i < size; ++i)
        for (size_t j = 0; j < size; ++j)
            matrix[i][j] = rand() % 100;
    return matrix;
}
