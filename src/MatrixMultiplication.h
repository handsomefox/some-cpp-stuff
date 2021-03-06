#pragma once
#include "pch.h"
template <typename T>
class Matrix
{
	T** m_Data;
	size_t m_Size = 0;

public:
	Matrix(size_t& size)
	{
		m_Size = size;
		m_Data = new T * [size];

		for (size_t i = 0; i < size; ++i)
			m_Data[i] = new T[size];

		Generate();
	}
	T** Generate()
	{
		for (size_t i = 0; i < m_Size; ++i) {
			for (size_t j = 0; j < m_Size; ++j)
				m_Data[i][j] = 1 + rand() % 100;
		}
		return m_Data;
	}
	size_t Size() const { return m_Size; }
	T** GetMatrix() { return m_Data; }
};
template <typename T>
T** CreateFilledMatrix(T**& matrix, size_t& size)
{
	matrix = new T * [size];
	for (size_t i = 0; i < size; ++i)
		matrix[i] = new T[size];

	for (size_t i = 0; i < size; ++i) {
		for (size_t j = 0; j < size; ++j)
			matrix[i][j] = static_cast<T>(static_cast<int64_t>(rand()) % 100);
	}
	return matrix;
}
template <typename T>
void MultiplyMatrix(Matrix<T>& mt1, Matrix<T>& mt2, size_t& size)
{
	T** temp1 = mt1.GetMatrix();
	T** temp2 = mt2.GetMatrix();

	T** res = new T * [size];
	for (size_t i = 0; i < size; ++i)
		res[i] = new T[size];

	for (size_t i = 0; i < size; ++i)
		for (size_t k = 0; k < size; ++k)
			for (size_t j = 0; j < size; ++j)
				res[i][j] = temp1[i][k] * temp2[k][j];
}
template <typename T>
void MultiplyMatrix(T**& mt1, T**& mt2, size_t& size)
{
	T** res = new T * [size];
	for (size_t i = 0; i < size; ++i)
		res[i] = new T[size];

	for (size_t i = 0; i < size; ++i)
		for (size_t k = 0; k < size; ++k)
			for (size_t j = 0; j < size; ++j)
				res[i][j] = mt1[i][k] * mt2[k][j];
}
template <typename T>
double RunMatrixBenchmark(T**& mt1, T**& mt2, size_t& size)
{
	const double start = omp_get_wtime();
	MultiplyMatrix(mt1, mt2, size);
	const double end = omp_get_wtime();

	return end - start;
}
template <typename T>
double RunMatrixBenchmark(Matrix<T>& mt1, Matrix<T>& mt2, size_t& size)
{
	const double start = omp_get_wtime();
	MultiplyMatrix(mt1, mt2, size);
	const double end = omp_get_wtime();

	return end - start;
}
