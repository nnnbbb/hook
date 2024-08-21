#include "common.h"
#include "hooks.h"


void DriverUnload(IN PDRIVER_OBJECT DriverObject) {
    UNREFERENCED_PARAMETER(DriverObject);
    UnHooks();
    print("DriverUnload\r\n");
}



extern "C" NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING path) {
    UNREFERENCED_PARAMETER(path);
    print("DriverEntry\r\n");

    if (!Hooks()) {
        return STATUS_UNSUCCESSFUL;
    }
    DriverObject->DriverUnload = DriverUnload;
    return STATUS_SUCCESS;
}