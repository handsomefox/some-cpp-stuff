#include "pch.h"

int* GenArray(const size_t size)
{
	srand(static_cast<unsigned>(time(nullptr)));

	int* array = new int[size];

	for (size_t i = 0; i < size; ++i)
		array[i] = 1 + rand() % 100;

	return array;
}
void ArrayAdd(const size_t size)
{
	int* array = GenArray(1000);
	double sum = 0;

	for (size_t i = 0; i < size; ++i)
		sum = sum + array[i];
}
uint64_t GetRDTSCCount()
{
	uint64_t c;

	__asm {
		cpuid
		rdtsc
		mov dword ptr[c + 0], eax
		mov dword ptr[c + 4], edx
	}

	return c;
}
void GetMax32BitTime()
{
	const LONGLONG time = Int32x32To64(0x7FFFFFFF, 10000000) + 116444736000000000;

	FILETIME ft;
	SYSTEMTIME st;

	ft.dwLowDateTime = static_cast<DWORD>(time);
	ft.dwHighDateTime = time >> 32;

	FileTimeToSystemTime(&ft, &st);

	std::cout << st.wYear << "-" << std::setw(2) << std::setfill('0') << st.wMonth
		<< "-" << std::setw(2) << std::setfill('0') << st.wDay << " "
		<< std::setw(2) << std::setfill('0') << st.wHour << ":"
		<< std::setw(2) << std::setfill('0') << st.wMinute << ":"
		<< std::setw(2) << std::setfill('0') << st.wSecond << "."
		<< std::setw(3) << std::setfill('0') << st.wMilliseconds << "\n";
}
double GetTimeAccuracy()
{
	time_t start, end;

	time(&start);
	end = start;

	while (end == start)
		time(&end);

	return difftime(end, start); // sec
}
float GetClockAccuracy()
{
	const clock_t start = clock();
	clock_t end = start;

	while (end == start)
		end = clock();

	return static_cast<float>(end - start) / CLOCKS_PER_SEC * 1000; // ms
}
LONGLONG GetFiletimeAccuracy()
{
	LARGE_INTEGER time1, time2;
	FILETIME start, end;

	GetSystemTimeAsFileTime(&start);
	end = start;

	time1.HighPart = start.dwHighDateTime;
	time1.LowPart = start.dwLowDateTime;
	time2.HighPart = end.dwHighDateTime;
	time2.LowPart = end.dwLowDateTime;

	while (time2.QuadPart == time1.QuadPart)
	{
		GetSystemTimeAsFileTime(&end);
		time2.HighPart = end.dwHighDateTime;
		time2.LowPart = end.dwLowDateTime;
	}

	return time2.QuadPart - time1.QuadPart; // 100ns
}
LONGLONG GetFiletimePreciseAccuracy()
{
	LARGE_INTEGER time1, time2;
	FILETIME start, end;

	GetSystemTimePreciseAsFileTime(&start);
	end = start;

	time1.HighPart = start.dwHighDateTime;
	time1.LowPart = start.dwLowDateTime;
	time2.HighPart = end.dwHighDateTime;
	time2.LowPart = end.dwLowDateTime;

	while (time2.QuadPart == time1.QuadPart)
	{
		GetSystemTimePreciseAsFileTime(&end);
		time2.HighPart = end.dwHighDateTime;
		time2.LowPart = end.dwLowDateTime;
	}

	return time2.QuadPart - time1.QuadPart; // 100ns
}
ULONGLONG GetTickCountAccuracy()
{
	const ULONGLONG start = GetTickCount64();
	ULONGLONG end = start;

	while (end == start)
		end = GetTickCount64();

	return end - start; // ms
}
uint64_t GetRDTSCAccuracy()
{
	const uint64_t start = GetRDTSCCount();
	uint64_t end = start;

	while (end == start)
		end = GetRDTSCCount();

	return end - start;
}
unsigned long long GetIntrinsicRDTSCAccuracy()
{
	const uint64_t start = __rdtsc();
	uint64_t end = start;

	while (end == start)
		end = __rdtsc();

	return end - start;
}
LONGLONG GetQPCAccuracy()
{
	LARGE_INTEGER start, end, freq, elapsed;

	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&start);

	end = start;

	while (end.QuadPart == start.QuadPart)
		QueryPerformanceCounter(&end);

	elapsed.QuadPart = end.QuadPart - start.QuadPart;
	elapsed.QuadPart *= 1000000000; // convert to nanoseconds
	elapsed.QuadPart /= freq.QuadPart;
	return elapsed.QuadPart; // ns
}
long long GetChronoAccuracy()
{
	const auto start = std::chrono::high_resolution_clock::now();
	auto end = start;

	while (end == start)
		end = std::chrono::high_resolution_clock::now();

	const std::chrono::duration<double> duration = end - start;
	const auto accuracy = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);

	return accuracy.count(); // ns
}
double GetOMPAccuracy()
{
	const double start = omp_get_wtime();
	double end = start;

	while (end <= start)
		end = omp_get_wtime();

	return (end - start) * 1000; // ms
}
void ArrayBenchmarkRDTSC()
{
	ULONGLONG duration = ULLONG_MAX;
	const size_t n = 10;

	DWORD64 start = 0, end = 0;
	for (size_t i = 0; i < n; ++i)
	{
		start = __rdtsc();
		ArrayAdd(1000);
		end = __rdtsc();
		duration = std::min(duration, end - start);
	}
	std::cout << std::setfill(' ') << "__rdtsc() " << std::setw(34) << "minimal time out of " << n
		<< ": " << duration << " ticks" << "(~" << (end - start) / GetCPUFreq() << " us)\n";
}
void ArrayBenchmarkQPC()
{
	const size_t n = 10;
	LARGE_INTEGER start, freq, elapsed, end;
	elapsed.QuadPart = LLONG_MAX;
	QueryPerformanceFrequency(&freq);

	for (unsigned int i = 0; i < n; ++i)
	{
		QueryPerformanceCounter(&start);
		ArrayAdd(1000);
		QueryPerformanceCounter(&end);
		elapsed.QuadPart =
			std::min(elapsed.QuadPart, end.QuadPart - start.QuadPart);
	}

	elapsed.QuadPart *= 1000000; // convert to microseconds
	elapsed.QuadPart /= freq.QuadPart;

	std::cout << std::setfill(' ')
		<< "GetPerformanceCounter() minimal time out of " << n << ": "
		<< elapsed.QuadPart << " us\n\n";
}
double ArrayBenchmarkAbsolute(const size_t size)
{
	int* array = GenArray(size);
	double add = 0;
	const double start = omp_get_wtime();

	for (size_t i = 0; i < size; ++i)
		add += array[i];

	const double end = omp_get_wtime();
	const double duration = end - start;

	std::cout << "\nAdding " << size << " elements of array\n";
	std::cout << "Sum = " << add << "\n";
	std::cout << "Time elapsed: " << duration << " seconds\n";

	return duration;
}
int ArrayBenchmarkRelative(const size_t size)
{
	int* array = GenArray(size), cycles = 0;
	double add = 0;
	ULONGLONG elapsed = 0;
	const ULONGLONG start = GetTickCount64();

	while (elapsed <= 2000)
	{
		for (size_t i = 0; i < size; ++i)
		{
			add += array[i];
			elapsed = GetTickCount64() - start;
		}
		cycles++;
	}
	std::cout << "\nAdding " << size << " elements of array\n";
	std::cout << "Max possible cycles in 2 sec: " << cycles << "\n";

	return cycles;
}
void Task1()
{
	SetConsoleColor(10);
	std::cout << "Max possible 32-bit time\n";
	std::cout << "------------------------\n";
	SetConsoleColor(14);
	GetMax32BitTime();
}
void Task2()
{
	SetConsoleColor(10);
	std::cout << "\nTimer functions benchmark\n";
	std::cout << "-------------------------\n";
	SetConsoleColor(9);
	std::cout << "\nFunction:" << std::setfill(' ') << std::setw(41) << "Accuracy:\n\n";
	SetConsoleColor(14);
	std::cout << "Time()" << std::setfill(' ') << std::setw(40) << GetTimeAccuracy() << " sec\n";
	std::cout << "Clock()" << std::setfill(' ') << std::setw(39) << GetClockAccuracy() << " ms\n";
	std::cout << "GetSystemTimeAsFileTime()" << std::setfill(' ') << std::setw(21) << GetFiletimeAccuracy() * 100 << " ns\n";
	std::cout << "GetSystemTimePreciseAsFileTime()" << std::setfill(' ') << std::setw(14) << GetFiletimePreciseAccuracy() * 100 << " ns\n";
	std::cout << "GetTickCount()" << std::setfill(' ') << std::setw(32) << GetTickCountAccuracy() << " ms\n";
	std::cout << "Rdtsc()" << std::setfill(' ') << std::setw(39) << GetRDTSCAccuracy() << " ticks(" << GetRDTSCAccuracy() / GetCPUFreq() << " ms)\n";
	std::cout << "__Rdtsc()" << std::setfill(' ') << std::setw(37) << GetIntrinsicRDTSCAccuracy() << " ticks(" << GetIntrinsicRDTSCAccuracy() / GetCPUFreq() << " ms)\n";
	std::cout << "QueryPerformanceCounter()" << std::setfill(' ') << std::setw(21) << GetQPCAccuracy() << " ns\n";
	std::cout << "Chrono Class" << std::setfill(' ') << std::setw(34) << GetChronoAccuracy() << " ns\n";
	std::cout << "omp_get_wtime()" << std::setfill(' ') << std::setw(31) << GetOMPAccuracy() << " ms\n";
}
void Task3()
{
	SetConsoleColor(10);
	std::cout << "\nArray benchmark\n";
	std::cout << "---------------\n";
	SetConsoleColor(14);
	ArrayBenchmarkRDTSC();
	ArrayBenchmarkQPC();
}
void Task4()
{
	SetConsoleColor(10);
	std::cout << "\nArray addition benchmark\n";
	std::cout << "------------------------\n";
	SetConsoleColor(9);
	std::cout << "\nAbsolute benchmark:\n";
	SetConsoleColor(14);
	const double time1 = ArrayBenchmarkAbsolute(100000);
	const double time2 = ArrayBenchmarkAbsolute(200000);
	const double time3 = ArrayBenchmarkAbsolute(300000);
	SetConsoleColor(1);
	const double ttime2 = time2 / time1;
	const double ttime3 = time3 / time1;
	std::cout << "\nT(200000/100000) = " << ttime2 << "\n";
	std::cout << "T(300000/100000) = " << ttime3 << "\n";
	SetConsoleColor(9);
	std::cout << "\nRelative benchmark:\n";
	SetConsoleColor(14);
	const double ops1 = ArrayBenchmarkRelative(100000);
	const double ops2 = ArrayBenchmarkRelative(200000);
	const double ops3 = ArrayBenchmarkRelative(300000);
	const double tops2 = ops2 / ops1;
	const double tops3 = ops3 / ops1;
	SetConsoleColor(1);
	std::cout << "\nT(200000/100000) = " << tops2 << "\n";
	std::cout << "T(300000/100000) = " << tops3 << "\n";
	SetConsoleColor(10);
	std::cout << "\nDifference between absolute and relative:\n";
	SetConsoleColor(14);
	std::cout << "\nDifference in T(200000/100000) = " << std::setprecision(4) << 100 * abs(1 - ttime2 * tops2) << "%\n";
	std::cout << "Difference in T(300000/100000) = " << std::setprecision(4) << 100 * abs(1 - ttime3 * tops3) << "%\n";
}
void Task5()
{
	size_t MatrixSize = 512;
	SetConsoleColor(10);
	std::cout << "\nMatrix multiplication benchmark\n";
	std::cout << "-------------------------------\n";
	for (auto i = 0; i < 3; ++i)
	{
		SetConsoleColor(14);
		std::cout << "\nSize = " << MatrixSize << "\n";
		Matrix<int> object(MatrixSize);
		int** matrix = CreateFilledMatrix(matrix, MatrixSize);
		std::cout << "Matrix multiplication with objects took: " << RunMatrixBenchmark(object, object, MatrixSize) << " sec\n";
		std::cout << "Matrix multiplication without objects took: " << RunMatrixBenchmark(matrix, matrix, MatrixSize) << " sec\n\n";

		for (size_t j = 0; j < MatrixSize; ++j)
		{
			delete[] matrix[j];
		}
		delete[] matrix;
		MatrixSize *= 2;
	}
}
void Task9()
{
	size_t matrix_size = 1024;
	SetConsoleColor(10);
	std::cout << "\nInfluence of data types on the Matrix multiplication benchmark\n";
	std::cout << "--------------------------------------------------------------\n";
	SetConsoleColor(14);

	int8_t** MatrixInt8 = CreateFilledMatrix(MatrixInt8, matrix_size);
	std::cout << "Matrix multiplication with int8 took: " << RunMatrixBenchmark(MatrixInt8, MatrixInt8, matrix_size) << " sec\n\n";

	for (size_t i = 0; i < matrix_size; ++i)
		delete[] MatrixInt8[i];
	delete[] MatrixInt8;

	int16_t** MatrixInt16 = CreateFilledMatrix(MatrixInt16, matrix_size);
	std::cout << "Matrix multiplication with int16 took: " << RunMatrixBenchmark(MatrixInt16, MatrixInt16, matrix_size) << " sec\n\n";

	for (size_t i = 0; i < matrix_size; ++i)
		delete[] MatrixInt16[i];
	delete[] MatrixInt16;

	int32_t** MatrixInt32 = CreateFilledMatrix(MatrixInt32, matrix_size);
	std::cout << "Matrix multiplication with int32 took: " << RunMatrixBenchmark(MatrixInt32, MatrixInt32, matrix_size) << " sec\n\n";

	for (size_t i = 0; i < matrix_size; ++i)
		delete[] MatrixInt32[i];
	delete[] MatrixInt32;

	int64_t** MatrixInt64 = CreateFilledMatrix(MatrixInt64, matrix_size);
	std::cout << "Matrix multiplication with int64 took: " << RunMatrixBenchmark(MatrixInt64, MatrixInt64, matrix_size) << " sec\n\n";

	for (size_t i = 0; i < matrix_size; ++i)
		delete[] MatrixInt64[i];
	delete[] MatrixInt64;


	double** MatrixDouble = CreateFilledMatrix(MatrixDouble, matrix_size);
	std::cout << "Matrix multiplication with double took: " << RunMatrixBenchmark(MatrixDouble, MatrixDouble, matrix_size) << " sec\n\n";

	for (size_t i = 0; i < matrix_size; ++i)
		delete[] MatrixDouble[i];
	delete[] MatrixDouble;

	float** MatrixFloat = CreateFilledMatrix(MatrixFloat, matrix_size);
	std::cout << "Matrix multiplication with float took: " << RunMatrixBenchmark(MatrixFloat, MatrixFloat, matrix_size) << " sec\n\n";

	for (size_t i = 0; i < matrix_size; ++i)
		delete[] MatrixFloat[i];
	delete[] MatrixFloat;

}
int main()
{
	PrintBuildType();
	SetConsoleColor(10);
	std::cout << "Max CPU Freq\n-------------\n";
	SetConsoleColor(14);
	std::cout << GetCPUFreq() << " Mhz\n\n";
	Task1();
	Task2();
	Task3();
	Task4();
	Task5();
	Task9();
	SetConsoleColor(15);
	system("PAUSE");
	return 0;
}