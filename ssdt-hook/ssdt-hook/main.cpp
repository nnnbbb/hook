#include <ntifs.h>
#include "pe-tools.h"
#include "_global.h"
#include "ssdt.h"
#include "hooks.h"

#define print(...) DbgPrint(__VA_ARGS__)

void DriverUnload(IN PDRIVER_OBJECT DriverObject) {
    PeDeinitialize();
    print("debugÇý¶¯Ð¶ÔØ2");
}

extern "C" NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING path) {
    print("debugÇý¶¯¼ÓÔØ1");
    DriverObject->DriverUnload = DriverUnload;
    PeInitialize();
    Log("[TITANHIDE] Hooks::Initialize() hooked %d functions\r\n", Hooks::Initialize());
    return STATUS_SUCCESS;
}
