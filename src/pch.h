// Put headers that need to be precompiled here
#pragma once
#define NOMINMAX
#include <Windows.h>
#include <iostream>
#include <powrprof.h>
#include <chrono>
#include <iomanip>
#include <omp.h>
#include <time.h>
#include <vector>
#pragma comment(lib, "Powrprof.lib")
typedef struct _PROCESSOR_POWER_INFORMATION {
	ULONG Number;
	ULONG MaxMhz;
	ULONG CurrentMhz;
	ULONG MhzLimit;
	ULONG MaxIdleState;
	ULONG CurrentIdleState;
} PROCESSOR_POWER_INFORMATION, * PPROCESSOR_POWER_INFORMATION;
