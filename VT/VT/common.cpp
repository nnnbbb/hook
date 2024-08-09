#include "common.h"


void print(const char* format, ...) {
    char message[1024] = "";
    va_list vl;
    va_start(vl, format);
    const int n = _vsnprintf(message, sizeof(message) / sizeof(char), format, vl);
    message[n] = '\0';
    va_end(vl);

    va_end(format);
    DbgPrint(message);
}

bool CheckBios() {
    size_t Bios = __readmsr(MSR_IA32_FEATURE_CONTROL);
    size_t Result = Bios & 5;
    if (Result == 5) {

        return true;
    }
    return false;
}

bool CheckCpuid() {
    int Cpuinfo[4] = {0};
    __cpuidex((int*)&Cpuinfo, 1, 0);
    return (Cpuinfo[2] >> 5) & 1;
}


bool CheckCr4() {
    size_t cr4 = __readcr4();
    cr4 = cr4 >> 13;
    if ((cr4 & 1) == 0) {
        return true;
    }
    return false;
}

VOID IsSupportVT(
    _In_ struct _KDPC* Dpc,
    _In_opt_ PVOID DeferredContext,
    _In_opt_ PVOID SystemArgument1,
    _In_opt_ PVOID SystemArgument2
) {
    UNREFERENCED_PARAMETER(Dpc);
    UNREFERENCED_PARAMETER(DeferredContext);

    bool Bios = CheckBios();
    bool Cpuid = CheckCpuid();
    bool Cr4 = CheckCr4();
    print(
        "current cpuNumber=%d,bbios=%d, bcpuid=%d, bcr4=%d",
        KeGetCurrentProcessorNumber(),
        Bios,
        Cpuid,
        Cr4
    );
    VMXInitCpu();
    KeSignalCallDpcDone(SystemArgument1);
    KeSignalCallDpcSynchronize(SystemArgument2);
}

