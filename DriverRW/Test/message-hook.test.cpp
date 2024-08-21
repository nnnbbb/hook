#include "Test.h"


int TestMessageHook() {
    std::cout << "[*] resolving NtConvertBetweenAuxiliaryCounterAndPerformanceCounter from ntdll.dll" << std::endl;
    *(PVOID*)&NtPerformanceCounter =
        GetProcAddress(GetModuleHandle(L"ntdll.dll"), "NtConvertBetweenAuxiliaryCounterAndPerformanceCounter");
    if (!NtPerformanceCounter) {
        std::cerr << "[*} failed to resolve NtConvertBetweenAuxiliaryCounterAndPerformanceCounter" << std::endl;
        return -1;
    }
    std::cout << "[*] resolved NtConvertBetweenAuxiliaryCounterAndPerformanceCounter" << std::endl;

    std::cout << "[*} sending command to driver" << std::endl;
    HOOK_DATA Data;
    Data.Magic = 0x77FF77FF;
    Data.ControlCode = 7;
    PVOID pData = (PVOID)&Data;
    INT64 Status = 0;

    NtPerformanceCounter(1, &pData, &Status, 0);
    return 0;
}