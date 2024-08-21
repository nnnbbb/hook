#include "Test.h"


int ReadMemoryTest() {

    std::cout << "[*] resolving NtConvertBetweenAuxiliaryCounterAndPerformanceCounter from ntdll.dll" << std::endl;
    HMODULE Nt = GetModuleHandle(L"ntdll.dll");
    CHECK_NOT_NULL(Nt);

    *(PVOID*)&NtPerformanceCounter = GetProcAddress(Nt, "NtConvertBetweenAuxiliaryCounterAndPerformanceCounter");

    if (!NtPerformanceCounter) {
        std::cerr << "[*] failed to resolve NtConvertBetweenAuxiliaryCounterAndPerformanceCounter" << std::endl;
        return -1;
    }
    std::cout << "[*] resolved NtConvertBetweenAuxiliaryCounterAndPerformanceCounter" << std::endl;

    std::cout << "[*] sending command to driver" << std::endl;

    HWND handle = FindWindowA(NULL, "FateÊó±ê¾«Áé");
    DWORD ProcessID;
    DWORD actualProcId = GetWindowThreadProcessId(handle, &ProcessID);

    HOOK_DATA Data;
    PVOID pData = (PVOID)&Data;
    INT64 Status = 0;

    PREAD_MEMORY ReadMemoryData = (PREAD_MEMORY)malloc(sizeof(PREAD_MEMORY));
    CHECK_NOT_NULL(ReadMemoryData);
    memset(ReadMemoryData, 0, sizeof(PREAD_MEMORY));
    ULONG64 ReadBuffer = 0;
    ReadMemoryData->Pid = ProcessID;
    ReadMemoryData->ReadAddress = 0x400001;
    ReadMemoryData->ReadBuffer = (ULONG64)&ReadBuffer;
    ReadMemoryData->ReadSize = sizeof(ULONG64);

    Data.Magic = 0x77FF77FF;
    Data.ControlCode = ReadMemory;
    Data.Body = ReadMemoryData;

    NtPerformanceCounter(1, &pData, &Status, 0);
    printf("ReadBuffer Address = 0x%016llX\n", (ULONG64)&ReadBuffer);
    printf("ReadBuffer Value = 0x%016llX\n", ReadBuffer);
    return 0;
}
