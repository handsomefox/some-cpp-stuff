#include "Matrix.h"
#include "pch.h"
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
void ChangeConsoleColor(const int color) {
  SetConsoleTextAttribute(hConsole, color);
}
void getBuildType() {
  ChangeConsoleColor(11);
#ifdef _DEBUG
  printf("Build type: Debug\n\n");
  // std::cout << "--------------------------------\n\n";
#else
  printf("Build type: Release\n\n");
  // std::cout << "--------------------------------\n\n";
#endif
}
int *GenerateArray(size_t size) {
  srand(time(0));
  int *array = new int[size];

  for (size_t i = 0; i < size; ++i)
    array[i] = 1 + rand() % 100;

  return array;
}
void ArrayAddition(size_t size) {
  int *array = GenerateArray(1000);
  double sum = 0;

  for (size_t i = 0; i < size; ++i)
    sum = sum + array[i];
}
uint64_t RdtscClockCycleCount() {
  uint64_t c;

  __asm {
		cpuid
		rdtsc
		mov dword ptr[c + 0], eax
		mov dword ptr[c + 4], edx
  }

  return c;
}
double GetMaxCpuFrequency() {
  ULONG maxfreq = 0;
  SYSTEM_INFO si = {0};
  GetSystemInfo(&si);
  std::vector<PROCESSOR_POWER_INFORMATION> a(si.dwNumberOfProcessors);
  DWORD dwSize = sizeof(PROCESSOR_POWER_INFORMATION) * si.dwNumberOfProcessors;
  CallNtPowerInformation(ProcessorInformation, NULL, 0, &a[0], dwSize);
  PPROCESSOR_POWER_INFORMATION ppi = (PPROCESSOR_POWER_INFORMATION)&a;
  for (DWORD i = 0; i < si.dwNumberOfProcessors; ++i) {
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
double max_cpu_frequency = GetMaxCpuFrequency();
void Max32BitTime() {
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
double TimeAccuracy() {
  time_t start, end;
  time(&start);
  end = start;

  while (end == start)
    time(&end);

  return difftime(end, start); // sec
}
float ClockAccuracy() {
  const clock_t start = clock();
  clock_t end = start;

  while (end == start)
    end = clock();

  return ((float)(end - start) / CLOCKS_PER_SEC) * 1000; // ms
}
long long FiletimeAccuracy() {
  LARGE_INTEGER time1, time2;
  FILETIME start, end;
  GetSystemTimeAsFileTime(&start);
  end = start;

  time1.HighPart = start.dwHighDateTime;
  time1.LowPart = start.dwLowDateTime;
  time2.HighPart = end.dwHighDateTime;
  time2.LowPart = end.dwLowDateTime;

  while (time2.QuadPart == time1.QuadPart) {
    GetSystemTimeAsFileTime(&end);
    time2.HighPart = end.dwHighDateTime;
    time2.LowPart = end.dwLowDateTime;
  }

  return (time2.QuadPart - time1.QuadPart); // 100ns
}
long long FiletimePreciseAccuracy() {
  LARGE_INTEGER time1, time2;
  FILETIME start, end;
  GetSystemTimePreciseAsFileTime(&start);
  end = start;

  time1.HighPart = start.dwHighDateTime;
  time1.LowPart = start.dwLowDateTime;
  time2.HighPart = end.dwHighDateTime;
  time2.LowPart = end.dwLowDateTime;

  while (time2.QuadPart == time1.QuadPart) {
    GetSystemTimePreciseAsFileTime(&end);
    time2.HighPart = end.dwHighDateTime;
    time2.LowPart = end.dwLowDateTime;
  }

  return (time2.QuadPart - time1.QuadPart); // 100ns
}
unsigned long TickAccuracy() {
  const DWORD start = GetTickCount();
  DWORD end = start;

  while (end == start)
    end = GetTickCount();

  return end - start; // ms
}
unsigned long long RdtscAccuracy() {
  const uint64_t start = RdtscClockCycleCount();
  uint64_t end = start;

  while (end == start) {
    end = RdtscClockCycleCount();
  }

  return end - start;
}
unsigned long long __RdtscAccuracy() {
  const uint64_t start = __rdtsc();
  uint64_t end = start;

  while (end == start)
    end = __rdtsc();

  return end - start;
}
LONGLONG QpcAccuracy() {
  LARGE_INTEGER start, end, freq, elapsed;
  double duration = 0;
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
long long ChronoAccuracy() {
  const auto start = std::chrono::high_resolution_clock::now();
  auto end = start;

  while (end == start)
    end = std::chrono::high_resolution_clock::now();

  const std::chrono::duration<double> duration = end - start;
  const auto accuracy =
      std::chrono::duration_cast<std::chrono::nanoseconds>(duration);

  return accuracy.count(); // ns
}
double OmpAccuracy() {
  const double start = omp_get_wtime();
  double end = start;

  while (end == start)
    end = omp_get_wtime();

  return (end - start) * 1000; // ms
}
void ArrayRdtscBenchmark() {
  unsigned long long duration = ULLONG_MAX;
  unsigned int n = 10;
  DWORD64 start, end;
  for (unsigned int i = 0; i < n; ++i) {
    start = __rdtsc();
    ArrayAddition(1000);
    end = __rdtsc();
    duration = std::min(duration, end - start);
  }
  std::cout << std::setfill(' ') << "__rdtsc() " << std::setw(34)
            << "minimal time out of " << n << ": " << duration << " ticks"
            << "(~" << (end - start) / max_cpu_frequency << " us)\n";
}
void ArrayQPCBenchmark() {
  unsigned int n = 10;
  LARGE_INTEGER start, freq, elapsed, end;
  elapsed.QuadPart = LLONG_MAX;
  QueryPerformanceFrequency(&freq);

  for (unsigned int i = 0; i < n; ++i) {
    QueryPerformanceCounter(&start);
    ArrayAddition(1000);
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
double ArrayAbsoluteBenchmark(size_t size) {
  int *array = GenerateArray(size);
  double add = 0;
  const double start = omp_get_wtime();

  for (size_t i = 0; i < size; ++i)
    add += array[i];

  double end = omp_get_wtime();
  double duration = end - start;

  std::cout << "\nAdding " << size << " elements of array\n";
  std::cout << "Sum = " << add << "\n";
  std::cout << "Time elapsed: " << duration << " seconds\n";

  return duration;
}
int ArrayRelativeBenchmark(size_t size) {
  int *array = GenerateArray(size), cycles = 0;
  double add = 0, elapsed = 0;
  DWORD start = GetTickCount();

  while (elapsed <= 2000) {
    for (size_t i = 0; i < size; ++i) {
      add += array[i];
      elapsed = GetTickCount() - start;
    }

    cycles++;
  }
  std::cout << "\nAdding " << size << " elements of array\n";
  std::cout << "Max possible cycles in 2 sec: " << cycles << "\n";

  return cycles;
}
void Task1() {
  ChangeConsoleColor(10);
  std::cout << "Max possible 32-bit time\n";
  std::cout << "------------------------\n";
  ChangeConsoleColor(14);
  Max32BitTime();
}
void Task2() {
  ChangeConsoleColor(10);
  std::cout << "\nTimer functions benchmark\n";
  std::cout << "-------------------------\n";
  ChangeConsoleColor(9);
  std::cout << "\nFunction:" << std::setfill(' ') << std::setw(41)
            << "Accuracy:\n\n";
  ChangeConsoleColor(14);
  std::cout << "Time()" << std::setfill(' ') << std::setw(40) << TimeAccuracy()
            << " sec\n";
  std::cout << "Clock()" << std::setfill(' ') << std::setw(39)
            << ClockAccuracy() << " ms\n";
  std::cout << "GetSystemTimeAsFileTime()" << std::setfill(' ') << std::setw(21)
            << FiletimeAccuracy() * 100 << " ns\n";
  std::cout << "GetSystemTimePreciseAsFileTime()" << std::setfill(' ')
            << std::setw(14) << FiletimePreciseAccuracy() * 100 << " ns\n";
  std::cout << "GetTickCount()" << std::setfill(' ') << std::setw(32)
            << TickAccuracy() << " ms\n";

  unsigned long long elapsed = RdtscAccuracy();
  std::cout << "Rdtsc()" << std::setfill(' ') << std::setw(39) << elapsed
            << " ticks(" << elapsed / (max_cpu_frequency) << " ms)\n";
  elapsed = __RdtscAccuracy();
  std::cout << "__Rdtsc()" << std::setfill(' ') << std::setw(37) << elapsed
            << " ticks(" << elapsed / (max_cpu_frequency) << " ms)\n";

  std::cout << "QueryPerformanceCounter()" << std::setfill(' ') << std::setw(21)
            << QpcAccuracy() << " ns\n";
  std::cout << "Chrono Class" << std::setfill(' ') << std::setw(34)
            << ChronoAccuracy() << " ns\n";
  std::cout << "omp_get_wtime()" << std::setfill(' ') << std::setw(31)
            << OmpAccuracy() << " ms\n";
}
void Task3() {
  ChangeConsoleColor(10);
  std::cout << "\nArray benchmark\n";
  std::cout << "---------------\n";
  ChangeConsoleColor(14);
  ArrayRdtscBenchmark();
  ArrayQPCBenchmark();
}
void Task4() {
  ChangeConsoleColor(10);
  std::cout << "\nArray addition benchmark\n";
  std::cout << "------------------------\n";
  ChangeConsoleColor(9);
  std::cout << "\nAbsolute benchmark:\n";
  ChangeConsoleColor(14);
  double time1 = ArrayAbsoluteBenchmark(100000);
  double time2 = ArrayAbsoluteBenchmark(200000);
  double time3 = ArrayAbsoluteBenchmark(300000);
  ChangeConsoleColor(1);
  double ttime2 = time2 / time1;
  double ttime3 = time3 / time1;
  std::cout << "\nT(200000/100000) = " << ttime2 << "\n";
  std::cout << "T(300000/100000) = " << ttime3 << "\n";
  ChangeConsoleColor(9);
  std::cout << "\nRelative benchmark:\n";
  ChangeConsoleColor(14);
  double ops1 = ArrayRelativeBenchmark(100000);
  double ops2 = ArrayRelativeBenchmark(200000);
  double ops3 = ArrayRelativeBenchmark(300000);
  double tops2 = ops2 / ops1;
  double tops3 = ops3 / ops1;
  ChangeConsoleColor(1);
  std::cout << "\nT(200000/100000) = " << tops2 << "\n";
  std::cout << "T(300000/100000) = " << tops3 << "\n";
  ChangeConsoleColor(10);
  std::cout << "\nDifference between absolute and relative:\n";
  ChangeConsoleColor(14);
  std::cout << "\nDifference in T(200000/100000) = " << std::setprecision(4)
            << 100 * abs(1 - (ttime2 * tops2)) << "%"
            << "\n";
  std::cout << "Difference in T(300000/100000) = " << std::setprecision(4)
            << 100 * abs(1 - (ttime3 * tops3)) << "%"
            << "\n";
}
void Task5() {
  size_t matrixSize = 512;
  ChangeConsoleColor(10);
  std::cout << "\nMatrix multiplication benchmark\n";
  std::cout << "-------------------------------\n";
  for (int i = 0; i < 3; ++i) {
    ChangeConsoleColor(14);
    std::cout << "\nSize = " << matrixSize << "\n";
    Matrix<int> object1(matrixSize);
    Matrix<int> object2(matrixSize);
    int **matrix1 = nullptr;
    int **matrix2 = nullptr;
    object1.generate();
    object2.generate();
    matrix1 = CreateFilledMatrix(matrix1, matrixSize);
    matrix2 = CreateFilledMatrix(matrix2, matrixSize);
    std::cout << "Matrix multiplication with objects took: "
              << MatrixBenchmark(object1, object2, matrixSize) << " sec\n";
    std::cout << "Matrix multiplication without objects took: "
              << MatrixBenchmark(matrix1, matrix2, matrixSize) << " sec\n\n";
    matrixSize *= 2;
  }
}
void Task9() {
  size_t matrixSize = 1024;
  ChangeConsoleColor(10);
  std::cout
      << "\nInfluence of data types on the matrix multiplication benchmark\n";
  std::cout
      << "--------------------------------------------------------------\n";
  ChangeConsoleColor(14);

  __int8 **matrix3 = nullptr;
  __int8 **matrix4 = nullptr;
  matrix3 = CreateFilledMatrix(matrix3, matrixSize);
  matrix4 = CreateFilledMatrix(matrix4, matrixSize);
  std::cout << "Matrix multiplication with int8 took: "
            << MatrixBenchmark(matrix3, matrix4, matrixSize) << " sec\n\n";

  __int16 **matrix5 = nullptr;
  __int16 **matrix6 = nullptr;
  matrix5 = CreateFilledMatrix(matrix5, matrixSize);
  matrix6 = CreateFilledMatrix(matrix6, matrixSize);
  std::cout << "Matrix multiplication with int16 took: "
            << MatrixBenchmark(matrix5, matrix6, matrixSize) << " sec\n\n";

  __int32 **matrix7 = nullptr;
  __int32 **matrix8 = nullptr;
  matrix7 = CreateFilledMatrix(matrix7, matrixSize);
  matrix8 = CreateFilledMatrix(matrix8, matrixSize);
  std::cout << "Matrix multiplication with int32 took: "
            << MatrixBenchmark(matrix7, matrix8, matrixSize) << " sec\n\n";

  __int64 **matrix9 = nullptr;
  __int64 **matrix10 = nullptr;
  matrix9 = CreateFilledMatrix(matrix9, matrixSize);
  matrix10 = CreateFilledMatrix(matrix10, matrixSize);
  std::cout << "Matrix multiplication with int64 took: "
            << MatrixBenchmark(matrix9, matrix10, matrixSize) << " sec\n\n";

  double **matrix11 = nullptr;
  double **matrix12 = nullptr;
  matrix11 = CreateFilledMatrix(matrix11, matrixSize);
  matrix12 = CreateFilledMatrix(matrix12, matrixSize);
  std::cout << "Matrix multiplication with double took: "
            << MatrixBenchmark(matrix11, matrix12, matrixSize) << " sec\n\n";

  float **matrix13 = nullptr;
  float **matrix14 = nullptr;
  matrix13 = CreateFilledMatrix(matrix13, matrixSize);
  matrix14 = CreateFilledMatrix(matrix14, matrixSize);
  std::cout << "Matrix multiplication with float took: "
            << MatrixBenchmark(matrix13, matrix14, matrixSize) << " sec\n\n";
}
int main() {
  getBuildType();
  ChangeConsoleColor(10);
  std::cout << "Max CPU Freq"
            << "\n";
  std::cout << "-------------\n";
  ChangeConsoleColor(14);
  std::cout << max_cpu_frequency << " Mhz"
            << "\n\n";
  Task1();
  Task2();
  Task3();
  Task4();
  Task5();
  Task9();
  ChangeConsoleColor(15);
  system("PAUSE");
  return 0;
}
