// Put headers that need to be precompiled here
#pragma once
#define NOMINMAX
#include <Windows.h>
typedef struct _PROCESSOR_POWER_INFORMATION {
	ULONG Number;
	ULONG MaxMhz;
	ULONG CurrentMhz;
	ULONG MhzLimit;
	ULONG MaxIdleState;
	ULONG CurrentIdleState;
} PROCESSOR_POWER_INFORMATION, * PPROCESSOR_POWER_INFORMATION;

#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <vector>
#include <omp.h>
#include <powrprof.h>
#pragma comment(lib, "Powrprof.lib")

#include "MatrixMultiplication.h"
#include "Utility.h"
