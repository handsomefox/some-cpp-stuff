#include "pch.h"
#include "Matrix.h"
HANDLE h_console = GetStdHandle(STD_OUTPUT_HANDLE);

void change_console_color(const unsigned short color)
{
	SetConsoleTextAttribute(h_console, color);
}

void get_build_type()
{
	change_console_color(11);
#ifdef _DEBUG
  printf("Build type: Debug\n\n");
  // std::cout << "--------------------------------\n\n";
#else
	printf("Build type: Release\n\n");
	// std::cout << "--------------------------------\n\n";
#endif
}

int* generate_array(const size_t size)
{
	srand(time(nullptr));
	int* array = new int[size];

	for (size_t i = 0; i < size; ++i)
		array[i] = 1 + rand() % 100;

	return array;
}

void array_addition(const size_t size)
{
	int* array = generate_array(1000);
	double sum = 0;

	for (size_t i = 0; i < size; ++i)
		sum = sum + array[i];
}

uint64_t rdtsc_clock_cycle_count()
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

double get_max_cpu_frequency()
{
	ULONG maxfreq = 0;
	SYSTEM_INFO si = {{0}};
	GetSystemInfo(&si);
	std::vector<PROCESSOR_POWER_INFORMATION> a(si.dwNumberOfProcessors);
	const DWORD dw_size = sizeof(PROCESSOR_POWER_INFORMATION) * si.dwNumberOfProcessors;
	CallNtPowerInformation(ProcessorInformation, nullptr, 0, &a[0], dw_size);
	auto* ppi = reinterpret_cast<PPROCESSOR_POWER_INFORMATION>(&a);
	for (DWORD i = 0; i < si.dwNumberOfProcessors; ++i)
	{
		maxfreq = std::max(maxfreq, ppi->MhzLimit);
		maxfreq = std::max(maxfreq, ppi->MaxMhz);
		maxfreq = std::max(maxfreq, ppi->CurrentMhz);
		maxfreq = std::max(maxfreq, ppi->Number); // Numbers sometimes contain max
		// cpu frequency for some reason.
		ppi++;
	}
	a.clear();
	return maxfreq / 1e6;
}

double max_cpu_frequency = get_max_cpu_frequency();

void max32_bit_time()
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

double time_accuracy()
{
	time_t start, end;
	time(&start);
	end = start;

	while (end == start)
		time(&end);

	return difftime(end, start); // sec
}

float clock_accuracy()
{
	const clock_t start = clock();
	clock_t end = start;

	while (end == start)
		end = clock();

	return static_cast<float>(end - start) / CLOCKS_PER_SEC * 1000; // ms
}

long long filetime_accuracy()
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

long long filetime_precise_accuracy()
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

unsigned long tick_accuracy()
{
	const DWORD start = GetTickCount();
	DWORD end = start;

	while (end == start)
		end = GetTickCount();

	return end - start; // ms
}

unsigned long long rdtsc_accuracy()
{
	const uint64_t start = rdtsc_clock_cycle_count();
	uint64_t end = start;

	while (end == start)
	{
		end = rdtsc_clock_cycle_count();
	}

	return end - start;
}

unsigned long long intrin_rdtsc_accuracy()
{
	const uint64_t start = __rdtsc();
	uint64_t end = start;

	while (end == start)
		end = __rdtsc();

	return end - start;
}

LONGLONG qpc_accuracy()
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

long long chrono_accuracy()
{
	const auto start = std::chrono::high_resolution_clock::now();
	auto end = start;

	while (end == start)
		end = std::chrono::high_resolution_clock::now();

	const std::chrono::duration<double> duration = end - start;
	const auto accuracy =
		std::chrono::duration_cast<std::chrono::nanoseconds>(duration);

	return accuracy.count(); // ns
}

double omp_accuracy()
{
	const double start = omp_get_wtime();
	double end = start;

	while (end == start)
		end = omp_get_wtime();

	return (end - start) * 1000; // ms
}

void array_rdtsc_benchmark()
{
	unsigned long long duration = ULLONG_MAX;
	const unsigned int n = 10;
	DWORD64 start = 0, end = 0;
	for (unsigned int i = 0; i < n; ++i)
	{
		start = __rdtsc();
		array_addition(1000);
		end = __rdtsc();
		duration = std::min(duration, end - start);
	}
	std::cout << std::setfill(' ') << "__rdtsc() " << std::setw(34)
		<< "minimal time out of " << n << ": " << duration << " ticks"
		<< "(~" << (end - start) / max_cpu_frequency << " us)\n";
}

void array_qpc_benchmark()
{
	const unsigned int n = 10;
	LARGE_INTEGER start, freq, elapsed, end;
	elapsed.QuadPart = LLONG_MAX;
	QueryPerformanceFrequency(&freq);

	for (unsigned int i = 0; i < n; ++i)
	{
		QueryPerformanceCounter(&start);
		array_addition(1000);
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

double array_absolute_benchmark(const size_t size)
{
	int* array = generate_array(size);
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

int array_relative_benchmark(const size_t size)
{
	int *array = generate_array(size), cycles = 0;
	double add = 0, elapsed = 0;
	const DWORD start = GetTickCount();

	while (elapsed <= 2000)
	{
		for (size_t i = 0; i < size; ++i)
		{
			add += array[i];
			elapsed = GetTickCount() - start;
		}

		cycles++;
	}
	std::cout << "\nAdding " << size << " elements of array\n";
	std::cout << "Max possible cycles in 2 sec: " << cycles << "\n";

	return cycles;
}

void task1()
{
	change_console_color(10);
	std::cout << "Max possible 32-bit time\n";
	std::cout << "------------------------\n";
	change_console_color(14);
	max32_bit_time();
}

void task2()
{
	change_console_color(10);
	std::cout << "\nTimer functions benchmark\n";
	std::cout << "-------------------------\n";
	change_console_color(9);
	std::cout << "\nFunction:" << std::setfill(' ') << std::setw(41)
		<< "Accuracy:\n\n";
	change_console_color(14);
	std::cout << "Time()" << std::setfill(' ') << std::setw(40) << time_accuracy()
		<< " sec\n";
	std::cout << "Clock()" << std::setfill(' ') << std::setw(39)
		<< clock_accuracy() << " ms\n";
	std::cout << "GetSystemTimeAsFileTime()" << std::setfill(' ') << std::setw(21)
		<< filetime_accuracy() * 100 << " ns\n";
	std::cout << "GetSystemTimePreciseAsFileTime()" << std::setfill(' ')
		<< std::setw(14) << filetime_precise_accuracy() * 100 << " ns\n";
	std::cout << "GetTickCount()" << std::setfill(' ') << std::setw(32)
		<< tick_accuracy() << " ms\n";

	unsigned long long elapsed = rdtsc_accuracy();
	std::cout << "Rdtsc()" << std::setfill(' ') << std::setw(39) << elapsed
		<< " ticks(" << elapsed / max_cpu_frequency << " ms)\n";
	elapsed = intrin_rdtsc_accuracy();
	std::cout << "__Rdtsc()" << std::setfill(' ') << std::setw(37) << elapsed
		<< " ticks(" << elapsed / max_cpu_frequency << " ms)\n";

	std::cout << "QueryPerformanceCounter()" << std::setfill(' ') << std::setw(21)
		<< qpc_accuracy() << " ns\n";
	std::cout << "Chrono Class" << std::setfill(' ') << std::setw(34)
		<< chrono_accuracy() << " ns\n";
	std::cout << "omp_get_wtime()" << std::setfill(' ') << std::setw(31)
		<< omp_accuracy() << " ms\n";
}

void task3()
{
	change_console_color(10);
	std::cout << "\nArray benchmark\n";
	std::cout << "---------------\n";
	change_console_color(14);
	array_rdtsc_benchmark();
	array_qpc_benchmark();
}

void task4()
{
	change_console_color(10);
	std::cout << "\nArray addition benchmark\n";
	std::cout << "------------------------\n";
	change_console_color(9);
	std::cout << "\nAbsolute benchmark:\n";
	change_console_color(14);
	const double time1 = array_absolute_benchmark(100000);
	const double time2 = array_absolute_benchmark(200000);
	const double time3 = array_absolute_benchmark(300000);
	change_console_color(1);
	const double ttime2 = time2 / time1;
	const double ttime3 = time3 / time1;
	std::cout << "\nT(200000/100000) = " << ttime2 << "\n";
	std::cout << "T(300000/100000) = " << ttime3 << "\n";
	change_console_color(9);
	std::cout << "\nRelative benchmark:\n";
	change_console_color(14);
	const double ops1 = array_relative_benchmark(100000);
	const double ops2 = array_relative_benchmark(200000);
	const double ops3 = array_relative_benchmark(300000);
	const double tops2 = ops2 / ops1;
	const double tops3 = ops3 / ops1;
	change_console_color(1);
	std::cout << "\nT(200000/100000) = " << tops2 << "\n";
	std::cout << "T(300000/100000) = " << tops3 << "\n";
	change_console_color(10);
	std::cout << "\nDifference between absolute and relative:\n";
	change_console_color(14);
	std::cout << "\nDifference in T(200000/100000) = " << std::setprecision(4)
		<< 100 * abs(1 - ttime2 * tops2) << "%"
		<< "\n";
	std::cout << "Difference in T(300000/100000) = " << std::setprecision(4)
		<< 100 * abs(1 - ttime3 * tops3) << "%"
		<< "\n";
}

void task5()
{
	size_t matrix_size = 512;
	change_console_color(10);
	std::cout << "\nMatrix multiplication benchmark\n";
	std::cout << "-------------------------------\n";
	for (auto i = 0; i < 3; ++i)
	{
		change_console_color(14);
		std::cout << "\nSize = " << matrix_size << "\n";
		matrix<int> object1(matrix_size);
		matrix<int> object2(matrix_size);
		int** matrix1 = nullptr;
		int** matrix2 = nullptr;
		object1.generate();
		object2.generate();
		matrix1 = create_filled_matrix(matrix1, matrix_size);
		matrix2 = create_filled_matrix(matrix2, matrix_size);
		std::cout << "Matrix multiplication with objects took: "
			<< matrix_benchmark(object1, object2, matrix_size) << " sec\n";
		std::cout << "Matrix multiplication without objects took: "
			<< matrix_benchmark(matrix1, matrix2, matrix_size) << " sec\n\n";
		matrix_size *= 2;
	}
}

void task9()
{
	size_t matrix_size = 1024;
	change_console_color(10);
	std::cout
		<< "\nInfluence of data types on the matrix multiplication benchmark\n";
	std::cout
		<< "--------------------------------------------------------------\n";
	change_console_color(14);

	__int8** matrix3 = nullptr;
	__int8** matrix4 = nullptr;
	matrix3 = create_filled_matrix(matrix3, matrix_size);
	matrix4 = create_filled_matrix(matrix4, matrix_size);
	std::cout << "Matrix multiplication with int8 took: "
		<< matrix_benchmark(matrix3, matrix4, matrix_size) << " sec\n\n";

	__int16** matrix5 = nullptr;
	__int16** matrix6 = nullptr;
	matrix5 = create_filled_matrix(matrix5, matrix_size);
	matrix6 = create_filled_matrix(matrix6, matrix_size);
	std::cout << "Matrix multiplication with int16 took: "
		<< matrix_benchmark(matrix5, matrix6, matrix_size) << " sec\n\n";

	__int32** matrix7 = nullptr;
	__int32** matrix8 = nullptr;
	matrix7 = create_filled_matrix(matrix7, matrix_size);
	matrix8 = create_filled_matrix(matrix8, matrix_size);
	std::cout << "Matrix multiplication with int32 took: "
		<< matrix_benchmark(matrix7, matrix8, matrix_size) << " sec\n\n";

	__int64** matrix9 = nullptr;
	__int64** matrix10 = nullptr;
	matrix9 = create_filled_matrix(matrix9, matrix_size);
	matrix10 = create_filled_matrix(matrix10, matrix_size);
	std::cout << "Matrix multiplication with int64 took: "
		<< matrix_benchmark(matrix9, matrix10, matrix_size) << " sec\n\n";

	double** matrix11 = nullptr;
	double** matrix12 = nullptr;
	matrix11 = create_filled_matrix(matrix11, matrix_size);
	matrix12 = create_filled_matrix(matrix12, matrix_size);
	std::cout << "Matrix multiplication with double took: "
		<< matrix_benchmark(matrix11, matrix12, matrix_size) << " sec\n\n";

	float** matrix13 = nullptr;
	float** matrix14 = nullptr;
	matrix13 = create_filled_matrix(matrix13, matrix_size);
	matrix14 = create_filled_matrix(matrix14, matrix_size);
	std::cout << "Matrix multiplication with float took: "
		<< matrix_benchmark(matrix13, matrix14, matrix_size) << " sec\n\n";
}

int main()
{
	get_build_type();
	change_console_color(10);
	std::cout << "Max CPU Freq"
		<< "\n";
	std::cout << "-------------\n";
	change_console_color(14);
	std::cout << max_cpu_frequency << " Mhz"
		<< "\n\n";
	task1();
	task2();
	task3();
	task4();
	task5();
	task9();
	change_console_color(15);
	system("PAUSE");
	return 0;
}
