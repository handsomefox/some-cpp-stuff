#pragma once
#include "pch.h"


inline void SetConsoleColor(const unsigned short color)
{
	HANDLE h_Console = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(h_Console, color);
}

inline void PrintBuildType()
{
	SetConsoleColor(11);
#ifdef _DEBUG
	printf("Build type: Debug\n\n");
	// std::cout << "--------------------------------\n\n";
#else
	printf("Build type: Release\n\n");
	// std::cout << "--------------------------------\n\n";
#endif
}

inline ULONG GetCPUFreq()
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	const auto size = si.dwNumberOfProcessors * sizeof(PROCESSOR_POWER_INFORMATION);

	const auto pBuffer = new BYTE[size];

	CallNtPowerInformation(ProcessorInformation, nullptr, 0, pBuffer, size);

	const auto ppi = reinterpret_cast<PPROCESSOR_POWER_INFORMATION>(pBuffer);

	const ULONG CPUFrequency = ppi->MaxMhz;

	delete[] pBuffer;
	return CPUFrequency;
}
