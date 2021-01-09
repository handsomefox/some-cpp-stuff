#pragma once
#include "pch.h"
template <typename T> class matrix {
  T **m_matrix_;
  size_t m_size_ = 0;

public:
  explicit matrix(size_t &size) {
    m_size_ = size;
    m_matrix_ = new T *[size];
    for (size_t i = 0; i < size; ++i)
      m_matrix_[i] = new T[size];
  }
  T **generate() {
    for (size_t i = 0; i < m_size_; ++i)
      for (size_t j = 0; j < m_size_; ++j)
        m_matrix_[i][j] = 1 + rand() % 100;
    return m_matrix_;
  }

  size_t size() const { return m_size_; }
  T **get_matrix() { return m_matrix_; }
};
template <typename T> T **create_filled_matrix(T **&matrix, size_t &size) {
  matrix = new T *[size];
  for (size_t i = 0; i < size; ++i)
    matrix[i] = new T[size];
  for (size_t i = 0; i < size; ++i)
    for (size_t j = 0; j < size; ++j)
      matrix[i][j] = 1 + rand() % 100;
  return matrix;
}
template <typename T>
void multiply_matrix(matrix<T> &mt1, matrix<T> &mt2, size_t &size) {
  T **temp1 = mt1.get_matrix();
  T **temp2 = mt2.get_matrix();

  T **res = new T *[size];
  for (size_t i = 0; i < size; ++i)
    res[i] = new T[size];

  for (size_t i = 0; i < size; ++i)
    for (size_t k = 0; k < size; ++k)
      for (size_t j = 0; j < size; ++j)
        res[i][j] = temp1[i][k] * temp2[k][j];
}
template <typename T> void multiply_matrix(T **&mt1, T **&mt2, size_t &size) {
  T **res = new T *[size];
  for (size_t i = 0; i < size; ++i)
    res[i] = new T[size];

  for (size_t i = 0; i < size; ++i)
    for (size_t k = 0; k < size; ++k)
      for (size_t j = 0; j < size; ++j)
        res[i][j] = mt1[i][k] * mt2[k][j];
}
template <typename T>
double matrix_benchmark(T **&mt1, T **&mt2, size_t &size) {
  double start, end;
  start = omp_get_wtime();
  multiply_matrix(mt1, mt2, size);
  end = omp_get_wtime();

  return end - start;
}
template <typename T>
double matrix_benchmark(matrix<T> &mt1, matrix<T> &mt2, size_t &size) {
  double start, end;
  start = omp_get_wtime();
  multiply_matrix(mt1, mt2, size);
  end = omp_get_wtime();

  return end - start;
}
