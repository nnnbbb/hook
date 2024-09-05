#include "pch.h"
#include "Speedhack.h"

namespace Speedhack {

double speed = 1.0;
bool initialised = false;

_tGetTickCount _GTC = nullptr;
DWORD _GTC_BaseTime = 0, _GTC_OffsetTime = 0;

_tGetTickCount64 _GTC64 = nullptr;
ULONGLONG _GTC64_BaseTime = 0, _GTC64_OffsetTime = 0;

_tQueryPerformanceCounter _QPC = nullptr;
LARGE_INTEGER _QPC_BaseTime = LARGE_INTEGER(), _QPC_OffsetTime = LARGE_INTEGER();

DWORD WINAPI _HookGetTickCount() {
    return _GTC_OffsetTime + ((_GTC() - _GTC_BaseTime) * speed);
}

ULONGLONG WINAPI _HookGetTickCount64() {
    return _GTC64_OffsetTime + ((_GTC64() - _GTC64_BaseTime) * speed);
}

BOOL WINAPI _HookQueryPerformanceCounter(LARGE_INTEGER *lpPerformanceCount) {
    LARGE_INTEGER x;
    _QPC(&x);
    lpPerformanceCount->QuadPart = _QPC_OffsetTime.QuadPart + ((x.QuadPart - _QPC_BaseTime.QuadPart) * speed);
    return TRUE;
}

void Setup() {
    if (initialised == false) {
        _GTC = &GetTickCount;
        _GTC_BaseTime = _GTC();
        _GTC_OffsetTime = _GTC_BaseTime;

        _GTC64 = &GetTickCount64;
        _GTC64_BaseTime = _GTC64();
        _GTC64_OffsetTime = _GTC64_BaseTime;

        _QPC = &QueryPerformanceCounter;
        _QPC(&_QPC_BaseTime);
        _QPC_OffsetTime.QuadPart = _QPC_BaseTime.QuadPart;

        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());

        DetourAttach(&(PVOID &)_GTC, _HookGetTickCount);
        DetourAttach(&(PVOID &)_GTC64, _HookGetTickCount64);
        DetourAttach(&(PVOID &)_QPC, _HookQueryPerformanceCounter);

        DetourTransactionCommit();
    }

    initialised = true;
}

void Detach() {
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    DetourDetach(&(PVOID &)_GTC, _HookGetTickCount);
    DetourDetach(&(PVOID &)_GTC64, _HookGetTickCount64);
    DetourDetach(&(PVOID &)_QPC, _HookQueryPerformanceCounter);

    DetourTransactionCommit();

    initialised = false;
}

void SetSpeed(double relSpeed) {
    if (initialised) {
        _GTC_OffsetTime = _HookGetTickCount();
        _GTC_BaseTime = _GTC();

        _GTC64_OffsetTime = _HookGetTickCount64();
        _GTC64_BaseTime = _GTC64();

        _HookQueryPerformanceCounter(&_QPC_OffsetTime);
        _QPC(&_QPC_BaseTime);
    }

    speed = relSpeed;
}
}  // namespace Speedhack