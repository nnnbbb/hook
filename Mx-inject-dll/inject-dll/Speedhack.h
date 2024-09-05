#pragma once

#include <Windows.h>
#include <detours/detours.h>

namespace Speedhack {
	extern double speed;
	extern bool initialised;

	typedef DWORD(WINAPI* _tGetTickCount)(void);
	extern _tGetTickCount _GTC;
	extern DWORD _GTC_BaseTime, _GTC_OffsetTime;

	typedef ULONGLONG(WINAPI* _tGetTickCount64)(void);
	extern _tGetTickCount64 _GTC64;
	extern ULONGLONG _GTC64_BaseTime, _GTC64_OffsetTime;

	typedef BOOL(WINAPI* _tQueryPerformanceCounter)(LARGE_INTEGER*);
	extern _tQueryPerformanceCounter _QPC;
	extern LARGE_INTEGER _QPC_BaseTime, _QPC_OffsetTime;

	DWORD WINAPI _HookGetTickCount();
	ULONGLONG WINAPI _HookGetTickCount64();
	BOOL WINAPI _HookQueryPerformanceCounter(LARGE_INTEGER* lpPerformanceCount);

	void Setup();
	void Detach();
	void SetSpeed(double relSpeed);
}  // namespace Speedhack