#include "common.h"

typedef struct _READ_MEMORY {
    ULONG64 Pid;
    ULONG64 ReadAddress;
    ULONG64 ReadBuffer;
    ULONG64 ReadSize;
} READ_MEMORY, *PREAD_MEMORY;

NTSTATUS ReadMemoryByVirtualMemory(HANDLE Pid, pv ReadAddress, u64 ReadSize, pv ReadBuffer);