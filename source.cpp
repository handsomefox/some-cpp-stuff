#include <chrono>
#include <iomanip>
#include <iostream>
#include <omp.h>
#include <time.h>
#include <Windows.h>

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
const unsigned int n = 100;

DWORD Basefreq()
{
	DWORD value;
	DWORD buffer[512];

	RegGetValueA(HKEY_LOCAL_MACHINE, R"(HARDWARE\DESCRIPTION\System\CentralProcessor\0)", "~MHz", RRF_RT_REG_DWORD,
				 nullptr, &value, buffer);

	return value;
}

DWORD cpufreq = Basefreq();

void ChangeConsoleColor(const int color)
{
	SetConsoleTextAttribute(hConsole, color);
}

void Max32BitTime()
{
	const LONGLONG time = Int32x32To64(0x7FFFFFFF, 10000000) + 116444736000000000;
	FILETIME ft;
	SYSTEMTIME st;

	ft.dwLowDateTime = static_cast<DWORD>(time);
	ft.dwHighDateTime = time >> 32;
	FileTimeToSystemTime(&ft, &st);

	std::cout << st.wYear << "-"
			  << std::setw(2) << std::setfill('0') << st.wMonth << "-"
			  << std::setw(2) << std::setfill('0') << st.wDay << " "
			  << std::setw(2) << std::setfill('0') << st.wHour << ":"
			  << std::setw(2) << std::setfill('0') << st.wMinute << ":"
			  << std::setw(2) << std::setfill('0') << st.wSecond << "."
			  << std::setw(3) << std::setfill('0') << st.wMilliseconds << std::endl;
}

double TimeAccuracy()
{
	time_t currtime, endtime;
	time(&currtime);
	time(&endtime);

	while (endtime == currtime)
	{
		for (unsigned int i = 0; i < n; ++i)
			time(&endtime);
	}

	return difftime(endtime, currtime); //sec
}

long ClockAccuracy()
{
	const clock_t start = clock();
	clock_t end = start;

	while (end == start)
	{
		for (unsigned int i = 0; i < n; ++i)
			end = clock();
	}

	return end / CLOCKS_PER_SEC; //ms
}

long long FiletimeAccuracy()
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
		for (unsigned int i = 0; i < n; ++i)
		{
			GetSystemTimeAsFileTime(&end);
			time2.HighPart = end.dwHighDateTime;
			time2.LowPart = end.dwLowDateTime;
		}
	}

	return time2.QuadPart - time1.QuadPart; // 100ns
}

long long FiletimePreciseAccuracy()
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
		for (unsigned int i = 0; i < n; ++i)
		{
			GetSystemTimePreciseAsFileTime(&end);
			time2.HighPart = end.dwHighDateTime;
			time2.LowPart = end.dwLowDateTime;
		}
	}

	return time2.QuadPart - time1.QuadPart; // 100ns
}

int *GenerateArray(size_t size)
{
	srand(time(nullptr));
	int *array = new int[size];

	for (size_t i = 0; i < size; ++i)
		array[i] = 1 + rand() % 1000;

	return array;
}

unsigned long TickAccuracy()
{
	const DWORD start = GetTickCount();
	DWORD end = start;

	while (end == start)
	{
		for (unsigned int i = 0; i < n; ++i)
			end = GetTickCount();
	}

	return end - start; //ms
}

uint64_t RdtscClockCycleCount()
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

unsigned long long RdtscAccuracy()
{
	const uint64_t start = RdtscClockCycleCount();
	uint64_t end = start;

	while (end == start)
	{
		for (unsigned int i = 0; i < n; ++i)
			end = RdtscClockCycleCount();
	}

	return end - start;
}

unsigned long long __RdtscAccuracy()
{
	const uint64_t start = __rdtsc();
	uint64_t end = start;

	while (end == start)
	{
		for (unsigned int i = 0; i < n; ++i)
			end = __rdtsc();
	}

	return end - start;
}

LONGLONG QpcAccuracy()
{
	LARGE_INTEGER start, end, freq, elapsed;
	QueryPerformanceCounter(&start);
	QueryPerformanceFrequency(&freq);

	end = start;

	while (end.QuadPart == start.QuadPart)
	{
		for (unsigned int i = 0; i < n; ++i)
			QueryPerformanceCounter(&end);
	}

	elapsed.QuadPart = end.QuadPart - start.QuadPart;
	elapsed.QuadPart *= 1000000;
	elapsed.QuadPart /= freq.QuadPart;

	return elapsed.QuadPart; //us
}

long long ChronoAccuracy()
{
	const auto start = std::chrono::high_resolution_clock::now();
	auto end = start;

	while (end == start)
	{
		for (unsigned int i = 0; i < n; ++i)
			end = std::chrono::high_resolution_clock::now();
	}

	const std::chrono::duration<double> duration = end - start;
	const auto accuracy = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);

	return accuracy.count(); //ns
}

double OmpAccuracy()
{
	const double start = omp_get_wtime();
	double end = start;

	while (end == start)
	{
		for (unsigned int i = 0; i < n; ++i)
			end = omp_get_wtime();
	}

	return (end - start) * 1000; //ms
}

void ArrayAddition(size_t size)
{
	srand(time(nullptr));
	int *array = GenerateArray(1000);
	double sum = 0;

	for (size_t i = 0; i < size; ++i)
		sum = sum + array[i];

	delete[] array;
}

void ArrayRdtscBenchmark()
{
	const DWORD64 start = __rdtsc();

	for (unsigned int i = 0; i < n; ++i)
		ArrayAddition(1000);

	const DWORD64 end = __rdtsc();
	std::cout << std::setfill(' ') << "__rdtsc() array addition " << std::setw(17) << n << " times took " << std::setw(11) << end - start << " ticks"
			  << "(" << (end - start) / cpufreq << " us)\n";
}

void ArrayQPCBenchmark()
{
	LARGE_INTEGER start, freq, elapsed, end;
	QueryPerformanceCounter(&start);
	QueryPerformanceFrequency(&freq);

	for (unsigned int i = 0; i < n; ++i)
		ArrayAddition(1000);

	QueryPerformanceCounter(&end);

	elapsed.QuadPart = end.QuadPart - start.QuadPart;
	elapsed.QuadPart *= 1000000;
	elapsed.QuadPart /= freq.QuadPart;

	std::cout << std::setfill(' ') << "GetPerformanceCounter() array addition " << n << " times took " << std::setw(11) << elapsed.QuadPart << " us\n"
			  << std::endl;
}

double ArrayAbsoluteBenchmark(size_t size)
{
	int *array = GenerateArray(size);
	double add = 0;
	const double start = omp_get_wtime();

	for (size_t i = 0; i < size; ++i)
		add += array[i];

	double end = omp_get_wtime();
	double duration = end - start;

	std::cout << "\nAdding " << size << " elements of array\n";
	std::cout << "Sum = " << add << std::endl;
	std::cout << "Time elapsed: " << duration << " seconds\n";

	delete[] array;
	return duration;
}

int ArrayRelativeBenchmark(size_t size)
{
	int *array = GenerateArray(size), cycles = 0;
	double add = 0, elapsed = 0;
	DWORD start = GetTickCount();

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
	std::cout << "Max possible cycles in 2 sec: " << cycles << std::endl;

	delete[] array;
	return cycles;
}
void Task1()
{
	ChangeConsoleColor(9);
	std::cout << "Max possible 32-bit time = ";
	Max32BitTime();
}

void Task2()
{
	ChangeConsoleColor(10);
	std::cout << "\nTimer functions benchmark\n";
	ChangeConsoleColor(9);
	std::cout << "\nFunction:" << std::setfill(' ') << std::setw(41) << "Accuracy:\n\n";
	ChangeConsoleColor(14);
	std::cout << "Time()" << std::setfill(' ') << std::setw(40) << TimeAccuracy() << " sec\n";
	std::cout << "Clock()" << std::setfill(' ') << std::setw(39) << ClockAccuracy() << " ms\n";
	std::cout << "GetSystemTimeAsFileTime()" << std::setfill(' ') << std::setw(21) << FiletimeAccuracy() * 100 << " ns\n";
	std::cout << "GetSystemTimePreciseAsFileTime()" << std::setfill(' ') << std::setw(14) << FiletimePreciseAccuracy() * 100 << " ns\n";
	std::cout << "GetTickCount()" << std::setfill(' ') << std::setw(32) << TickAccuracy() << " ms\n";

	unsigned long long elapsed = RdtscAccuracy();
	std::cout << "Rdtsc()" << std::setfill(' ') << std::setw(39) << elapsed << " ticks(" << elapsed / (cpufreq * 1e3) << " ms)\n";
	elapsed = __RdtscAccuracy();
	std::cout << "__Rdtsc()" << std::setfill(' ') << std::setw(37) << elapsed << " ticks(" << elapsed / (cpufreq * 1e3) << " ms)\n";

	std::cout << "QueryPerformanceCounter()" << std::setfill(' ') << std::setw(21) << QpcAccuracy() << " us\n";
	std::cout << "Chrono Class" << std::setfill(' ') << std::setw(34) << ChronoAccuracy() << " ns\n";
	std::cout << "omp_get_wtime()" << std::setfill(' ') << std::setw(31) << OmpAccuracy() << " ms\n";
}

void Task3()
{
	ChangeConsoleColor(10);
	std::cout << "\nArray benchmark\n";
	ChangeConsoleColor(14);
	ArrayRdtscBenchmark();
	ArrayQPCBenchmark();
}

void Task4()
{
	ChangeConsoleColor(10);

	std::cout << "\nArray addition benchmark\n";
	ChangeConsoleColor(10);
	std::cout << "\nAbsolute benchmark:\n";
	ChangeConsoleColor(14);
	double time1 = ArrayAbsoluteBenchmark(100000);
	double time2 = ArrayAbsoluteBenchmark(200000);
	double time3 = ArrayAbsoluteBenchmark(300000);
	ChangeConsoleColor(9);
	double ttime2 = time2 / time1;
	double ttime3 = time3 / time1;
	std::cout << "\nT(200000/100000) = " << ttime2 << std::endl;
	std::cout << "T(300000/100000) = " << ttime3 << std::endl;
	ChangeConsoleColor(10);

	std::cout << "\nRelative benchmark:\n";
	ChangeConsoleColor(14);
	double ops1 = ArrayRelativeBenchmark(100000);
	double ops2 = ArrayRelativeBenchmark(200000);
	double ops3 = ArrayRelativeBenchmark(300000);
	double tops2 = ops2 / ops1;
	double tops3 = ops3 / ops1;
	ChangeConsoleColor(9);
	std::cout << "\nT(200000/100000) = " << tops2 << std::endl;
	std::cout << "T(300000/100000) = " << tops3 << std::endl;
	ChangeConsoleColor(10);

	std::cout << "\nDifference between absolute and relative:\n";
	ChangeConsoleColor(14);
	std::cout << "\nDifference in T(200000/100000) = " << ttime2 * tops2 << std::endl;
	std::cout << "Difference in T(300000/100000) = " << ttime3 * tops3 << std::endl;
}

int main()
{
	ChangeConsoleColor(10);
	std::cout << "Base CPU Freq = " << cpufreq << " Mhz" << std::endl;
	Task1();
	Task2();
	Task3();
	Task4();
	std::cout << "\n";
	ChangeConsoleColor(15);
	system("PAUSE");
	return 0;
}
