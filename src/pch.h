// Put headers that need to be precompiled here
#pragma once
#define NOMINMAX
#include <Windows.h>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <omp.h>
#include <powrprof.h>
#include <ctime>
#include <vector>
#pragma comment(lib, "Powrprof.lib")
typedef struct _PROCESSOR_POWER_INFORMATION {
  ULONG Number;
  ULONG MaxMhz;
  ULONG CurrentMhz;
  ULONG MhzLimit;
  ULONG MaxIdleState;
  ULONG CurrentIdleState;
} PROCESSOR_POWER_INFORMATION, *PPROCESSOR_POWER_INFORMATION;
