#include "SimpleVT.h"

SimpleVT* VT_CPU[128];

_IRQL_requires_max_(DISPATCH_LEVEL) void* __cdecl
operator new(unsigned __int64 size) {
    PHYSICAL_ADDRESS highest;

    highest.QuadPart = 0xFFFFFFFFFFFFFFFF;
    return MmAllocateContiguousMemory(size, highest);
}

_IRQL_requires_max_(DISPATCH_LEVEL) void __cdecl
operator delete(void* p, size_t size) {
    UNREFERENCED_PARAMETER(size);

    if (p) {
        MmFreeContiguousMemory(p);
        p = NULL;
    }
}


VOID VTLoadProc(
    _In_ struct _KDPC* Dpc,
    _In_opt_ PVOID DeferredContext,
    _In_opt_ PVOID SystemArgument1,
    _In_opt_ PVOID SystemArgument2
) {
    UNREFERENCED_PARAMETER(Dpc);
    UNREFERENCED_PARAMETER(DeferredContext);

    ULONG uCPU = KeGetCurrentProcessorNumber();
    print("当前CPU------> %d", uCPU);

    VT_CPU[uCPU] = new SimpleVT(uCPU);

    if (VT_CPU[uCPU]->Initialize()) {
        VT_CPU[uCPU]->Install();
    }

    KeSignalCallDpcDone(SystemArgument1);
    KeSignalCallDpcSynchronize(SystemArgument2);
}

VOID VTUnLoadProc(
    _In_ struct _KDPC* Dpc,
    _In_opt_ PVOID DeferredContext,
    _In_opt_ PVOID SystemArgument1,
    _In_opt_ PVOID SystemArgument2
) {
    UNREFERENCED_PARAMETER(Dpc);
    UNREFERENCED_PARAMETER(DeferredContext);

    ULONG uCPU = KeGetCurrentProcessorNumber();
    print("当前CPU------> %d", uCPU);

    VT_CPU[uCPU]->UnInstall();
    VT_CPU[uCPU]->UnInitialize();

    if (VT_CPU[uCPU]) {
        delete VT_CPU[uCPU];
    }
    
    KeSignalCallDpcDone(SystemArgument1);
    KeSignalCallDpcSynchronize(SystemArgument2);
}

VOID VTLoad() {
    KeGenericCallDpc(VTLoadProc, NULL);
}

VOID VTUnLoad() {
    KeGenericCallDpc(VTUnLoadProc, NULL);
}


void DriverUnload(IN PDRIVER_OBJECT DriverObject) {
    UNREFERENCED_PARAMETER(DriverObject);
    VTUnLoad();
    print("Driver Unload");
}

extern "C" NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING path) {
    UNREFERENCED_PARAMETER(path);
    VTLoad();
    DriverObject->DriverUnload = DriverUnload;
    return STATUS_SUCCESS;
}