#include <chrono>
#include <iomanip>
#include <iostream>
#include <omp.h>
#include <time.h>
#include <Windows.h>

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
using namespace std;
const unsigned int n = 100;

void change_color(const int color)
{
    SetConsoleTextAttribute(hConsole, color);
}

DWORD basefreq()
{
    DWORD value;
    DWORD buffer[512];
    RegGetValueA(HKEY_LOCAL_MACHINE, R"(HARDWARE\DESCRIPTION\System\CentralProcessor\0)", "~MHz", RRF_RT_REG_DWORD,
                 nullptr, &value, buffer);
    return value;
}

DWORD cpufreq = basefreq();

void max_32_bit_time()
{
    const time_t time = 0x7FFFFFFF;
    const LONGLONG ll = Int32x32To64(time, 10000000) + 116444736000000000;
    FILETIME ft;
    SYSTEMTIME st;
    ft.dwLowDateTime = static_cast<DWORD>(ll);
    ft.dwHighDateTime = ll >> 32;
    FileTimeToSystemTime(&ft, &st);
    cout << st.wYear << "-"
         << setw(2) << setfill('0') << st.wMonth << "-"
         << setw(2) << setfill('0') << st.wDay << " "
         << setw(2) << setfill('0') << st.wHour << ":"
         << setw(2) << setfill('0') << st.wMinute << ":"
         << setw(2) << setfill('0') << st.wSecond << "."
         << setw(3) << setfill('0') << st.wMilliseconds << endl;
}

double time_accuracy()
{
    time_t currtime, endtime;
    time(&currtime);
    time(&endtime);
    while (endtime == currtime)
    {
        for (unsigned int i = 0; i < n; i++)
            time(&endtime);
    }
    const double diff = difftime(endtime, currtime);
    return diff; //sec
}

long clock_accuracy()
{
    const clock_t start = clock();
    clock_t end = start;
    while (end == start)
    {
        for (unsigned int i = 0; i < n; ++i)
            end = clock();
    }
    const long result = end / CLOCKS_PER_SEC;
    return result; //ms
}

long long filetime_accuracy()
{
    FILETIME start, end;
    GetSystemTimeAsFileTime(&start);
    end = start;
    LARGE_INTEGER time1, time2;
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
    long long duration = time2.QuadPart - time1.QuadPart;
    return duration; // 100ns
}

long long filetime_precise_accuracy()
{
    FILETIME start, end;
    GetSystemTimePreciseAsFileTime(&start);
    end = start;
    LARGE_INTEGER time1, time2;
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
    long long duration = time2.QuadPart - time1.QuadPart;
    return duration; // 100ns
}

int *generate_array(const int size)
{
    srand(time(nullptr));
    int *array = new int[size];
    for (int i = 0; i < size; ++i)
        array[i] = 1 + rand() % 1000;
    return array;
}

unsigned long tick_accuracy()
{
    const DWORD start = GetTickCount();
    DWORD end = start;
    while (end == start)
    {
        for (unsigned int i = 0; i < n; ++i)
            end = GetTickCount();
    }
    unsigned long duration = end - start; //ms
    return duration;
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

unsigned long long rdtsc_accuracy()
{
    const uint64_t start = rdtsc_clock_cycle_count();
    uint64_t end = start;
    while (end == start)
    {
        for (unsigned int i = 0; i < n; ++i)
            end = rdtsc_clock_cycle_count();
    }
    unsigned long long elapsed = end - start;
    return elapsed;
}

unsigned long long __rdtsc_accuracy()
{
    const uint64_t start = __rdtsc();
    uint64_t end = start;
    while (end == start)
    {
        for (unsigned int i = 0; i < n; ++i)
            end = __rdtsc();
    }
    unsigned long long elapsed = end - start;
    return elapsed;
}

LONGLONG query_performance_counter_accuracy()
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

long long chrono_accuracy()
{
    const auto start = chrono::high_resolution_clock::now();
    auto end = start;
    while (end == start)
    {
        for (unsigned int i = 0; i < n; ++i)
            end = chrono::high_resolution_clock::now();
    }
    const chrono::duration<double> duration = end - start;
    const auto accuracy = chrono::duration_cast<chrono::nanoseconds>(duration);
    return accuracy.count(); //ns
}

double omp_get_wtime_accuracy()
{
    const double start = omp_get_wtime();
    double end = start;
    while (end == start)
    {
        for (unsigned int i = 0; i < n; ++i)
            end = omp_get_wtime();
    }
    double duration = (end - start) * 1000;
    return duration; //ms
}

void array_addition(const int addition_size)
{
    srand(time(nullptr));
    int *a = generate_array(1000);
    double sum = 0;
    for (int i = 0; i < addition_size; ++i)
        sum = sum + a[i];
    delete a;
}

void array_bench_rdtsc()
{
    const DWORD64 start = __rdtsc();
    for (unsigned int i = 0; i < n; ++i)
        array_addition(1000);
    const DWORD64 end = __rdtsc();
    cout << setfill(' ') << "__rdtsc() array addition " << setw(17) << n << " times took " << setw(11) << end - start << " ticks"
         << "(" << (end - start) / cpufreq << " us)\n";
}

void array_bench_qpc()
{
    LARGE_INTEGER start, freq, elapsed, end;
    QueryPerformanceCounter(&start);
    QueryPerformanceFrequency(&freq);
    for (unsigned int i = 0; i < n; ++i)
        array_addition(1000);
    QueryPerformanceCounter(&end);
    elapsed.QuadPart = end.QuadPart - start.QuadPart;
    elapsed.QuadPart *= 1000000;
    elapsed.QuadPart /= freq.QuadPart;
    cout << setfill(' ') << "GetPerformanceCounter() array addition " << n << " times took " << setw(11) << elapsed.QuadPart << " us\n"
         << endl;
}

void array_abs_bench(const int size)
{
    int *array = generate_array(size);
    int add = 0;
    const double start = omp_get_wtime();
    for (int i = 0; i < size; ++i)
    {
        add += array[i];
    }
    double end = omp_get_wtime();
    cout << "\nAdding " << size << " elements of array\n";
    cout << "Sum = " << add << endl;
    cout << "Time elapsed: " << (end - start) * 1000 << " milliseconds\n";
    delete array;
}

void array_rel_bench(const int size)
{
    int *array = generate_array(size);
    int add = 0;
    int addition_cycles = 0;
    double elapsed = 0;
    DWORD start = GetTickCount();
    while (elapsed <= 2000)
    {
        for (unsigned int i = 0; i < size; ++i)
        {
            add += array[i];
            elapsed = GetTickCount() - start;
        }
        addition_cycles++;
    }
    cout << "\nAdding " << size << " elements of array\n";
    cout << "Max possible cycles in 2 sec: " << addition_cycles << endl;
    delete array;
}
void task1()
{
    change_color(9);
    cout << "Max possible 32-bit time = ";
    max_32_bit_time();
}

void task2()
{
    change_color(10);
    cout << "\nTimer functions benchmark\n";
    change_color(9);
    cout << "\nFunction:" << setfill(' ') << setw(41) << "Accuracy:\n\n";
    change_color(14);
    cout << "Time()" << setfill(' ') << setw(40) << time_accuracy() << " sec\n";
    cout << "Clock()" << setfill(' ') << setw(39) << clock_accuracy() << " ms\n";
    cout << "GetSystemTimeAsFileTime()" << setfill(' ') << setw(21) << filetime_accuracy() * 100 << " ns\n";
    cout << "GetSystemTimePreciseAsFileTime()" << setfill(' ') << setw(14) << filetime_precise_accuracy() * 100 << " ns\n";
    cout << "GetTickCount()" << setfill(' ') << setw(32) << tick_accuracy() << " ms\n";
    unsigned long long elapsed = rdtsc_accuracy();
    cout << "Rdtsc()" << setfill(' ') << setw(39) << elapsed << " ticks(" << elapsed / (cpufreq * 1e3) << " ms)\n";
    elapsed = __rdtsc_accuracy();
    cout << "__Rdtsc()" << setfill(' ') << setw(37) << elapsed << " ticks(" << elapsed / (cpufreq * 1e3) << " ms)\n";
    cout << "QueryPerformanceCounter()" << setfill(' ') << setw(21) << query_performance_counter_accuracy() << " us\n";
    cout << "Chrono Class" << setfill(' ') << setw(34) << chrono_accuracy() << " ns\n";
    cout << "omp_get_wtime()" << setfill(' ') << setw(31) << omp_get_wtime_accuracy() << " ms\n";
}

void task3()
{
    change_color(10);
    cout << "\nArray benchmark\n";
    change_color(14);
    array_bench_rdtsc();
    array_bench_qpc();
}

void task4()
{
    change_color(10);
    cout << "\nArray addition benchmark\n";
    change_color(9);
    cout << "\nAbsolute benchmark:\n";
    change_color(14);
    array_abs_bench(100000);
    array_abs_bench(200000);
    array_abs_bench(300000);
    change_color(9);
    cout << "\nRelative benchmark:\n";
    change_color(14);
    array_rel_bench(100000);
    array_rel_bench(200000);
    array_rel_bench(300000);
}

int main()
{
    change_color(10);
    cout << "Base CPU Freq = " << cpufreq << " Mhz" << endl;
    task1();
    task2();
    task3();
    task4();
    cout << "\n";
    change_color(15);
    system("PAUSE");
    return 0;
}
