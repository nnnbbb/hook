#include "common.h"


void DriverUnload(IN PDRIVER_OBJECT DriverObject) {
    UNREFERENCED_PARAMETER(DriverObject);
    print("Driver Unload");
}

extern "C" NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING path) {
    UNREFERENCED_PARAMETER(path);
    
    KeGenericCallDpc(IsSupportVT, NULL);
    DriverObject->DriverUnload = DriverUnload;
    return STATUS_SUCCESS;
}