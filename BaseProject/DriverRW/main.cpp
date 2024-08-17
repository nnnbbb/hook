#include <ntifs.h>
#include <ntstrsafe.h>
#include "utils.hpp"


void DriverUnload(IN PDRIVER_OBJECT DriverObject) {
	UNREFERENCED_PARAMETER(DriverObject);
	print("DriverUnload\r\n");
}

extern "C" NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING path) {
	UNREFERENCED_PARAMETER(path);
	DriverObject->DriverUnload = DriverUnload;
    
	print("DriverEntry\r\n");
	return STATUS_SUCCESS;
}