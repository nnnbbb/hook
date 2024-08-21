#include <Windows.h>
#include <iostream>

typedef NTSTATUS(NTAPI* _NtConvertBetweenAuxiliaryCounterAndPerformanceCounter)(
    ULONG64,
    PVOID,
    PVOID,
    PVOID
);
static _NtConvertBetweenAuxiliaryCounterAndPerformanceCounter NtPerformanceCounter = NULL;


typedef struct _READ_MEMORY {
    ULONG64 Pid;
    ULONG64 ReadAddress;
    ULONG64 ReadBuffer;
    ULONG64 ReadSize;
} READ_MEMORY, *PREAD_MEMORY;

typedef enum _HOOK_DATA_CONTROL_CODE {
    ReadMemory = 0x1,
} HOOK_DATA_CONTROL_CODE;

typedef struct _HOOK_DATA {
    DWORD Magic;
    ULONG ControlCode;
    PVOID Body;
    ULONG64 BodySize;
    ULONG64 Result;
} HOOK_DATA, *PHOOK_DATA;

#define CHECK_NOT_NULL(address)                                   \
    do {                                                          \
        if (address == 0) {                                       \
            throw std::runtime_error(#address "address is NULL"); \
        }                                                         \
    } while (0)


int TestMessageHook();
int ReadMemoryTest();

const HMODULE& NewFunction();
