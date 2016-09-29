#ifndef TIMING_H

#include <cstdio>
#include <cstdint>

#if defined(_MSC_VER) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(WIN64) || defined(_WIN64) || defined(__WIN64__)
#define TIMING_WIN32
#endif

#define timing_t double

void printInfo(timing_t times, uint64_t mem)
{
	printf("--->Elapsed Timing(ms): %.3f\n", times);
	if (mem)
		printf("--->Memory  Usage(KB) : %llu\n", mem);
	printf("----------------------------------------\n");
}

#ifdef TIMING_WIN32

#include <windows.h>
#include <Psapi.h>

#pragma comment(lib, "Psapi.lib")

SIZE_T memoryUsed()
{
	SIZE_T res = 0;
	if (HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, GetCurrentProcessId())) {
		PROCESS_MEMORY_COUNTERS pmc;
		if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
			res = pmc.PeakWorkingSetSize >> 10;
		}
		CloseHandle(hProcess);
	}
	return res;
}

LARGE_INTEGER time_start, time_over;
LONGLONG dqFreq;

inline void startTiming()
{
	LARGE_INTEGER f;
	QueryPerformanceFrequency(&f);
	dqFreq = f.QuadPart;

	QueryPerformanceCounter(&time_start);
}

// unit: ms
inline timing_t stopTiming()
{
	QueryPerformanceCounter(&time_over);
	return ((time_over.QuadPart - time_start.QuadPart) * 1000 / static_cast<timing_t>(dqFreq));
}

inline timing_t stopWithPrintTiming(bool getMem = false)
{
	QueryPerformanceCounter(&time_over);
	timing_t timing = ((time_over.QuadPart - time_start.QuadPart) * 1000 / static_cast<timing_t>(dqFreq));
	printInfo(timing, getMem ? memoryUsed() : 0);
	return timing;
}

#else //	Other OS

#include <time.h>
#include <unistd.h>

timespec time_start, time_end;

inline void startTiming()
{
	clock_gettime(CLOCK_MONOTONIC, &time_start);
}

// unit: ms
inline timing_t stopTiming()
{
	clock_gettime(CLOCK_MONOTONIC, &time_end);
	return (time_end.tv_sec - time_start.tv_sec) * 1000 + (time_end.tv_nsec - time_start.tv_nsec) / 1000000.0;
}

inline timing_t stopWithPrintTiming()
{
	clock_gettime(CLOCK_MONOTONIC, &time_end);

	timing_t timing = (time_end.tv_sec - time_start.tv_sec) * 1000 + (time_end.tv_nsec - time_start.tv_nsec) / 1000000.0;
	printInfo(timing, 0);
	return timing;
}

#endif // end of ifdef TIMING_WIN32

// unit: ms
inline void wait(size_t ms)
{
#ifdef TIMING_WIN32
	Sleep(ms);
#else
	usleep(ms * 1000);
#endif
}

#endif // !TIMING_H
