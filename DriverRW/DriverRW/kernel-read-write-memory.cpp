#include "kernel-read-write-memory.h"

extern "C" NTKERNELAPI NTSTATUS NTAPI MmCopyVirtualMemory(
    PEPROCESS SourceProcess,
    PVOID SourceAddress,
    PEPROCESS TargetProcess,
    PVOID TargetAddress,
    SIZE_T BufferSize,
    KPROCESSOR_MODE PreviousMode,
    PSIZE_T ReturnSize
);

NTSTATUS ReadMemoryByVirtualMemory(HANDLE Pid, pv ReadAddress, u64 ReadSize, pv ReadBuffer) {
    PEPROCESS Process;
    NTSTATUS Status = PsLookupProcessByProcessId(Pid, &Process);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }
    if (PsGetProcessExitStatus(Process) != STATUS_PENDING) {
        ObDereferenceObject(Process);
        return STATUS_INVALID_PARAMETER_1;
    }

    __try {
        SIZE_T Result;
        PEPROCESS TargetProcess = PsGetCurrentProcess();
        if (NT_SUCCESS(MmCopyVirtualMemory(Process, ReadAddress, TargetProcess, ReadBuffer, ReadSize, KernelMode, &Result))) {
            Status = STATUS_SUCCESS;
        } else {
            print("ReadMemoryByVirtualMemory ACCESS_DENIED\n");
            Status = STATUS_ACCESS_DENIED;
        }
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        print("ReadMemoryByVirtualMemory EXCEPTION_EXECUTE_HANDLER\n");
        Status = STATUS_ACCESS_DENIED;
    }
    ObDereferenceObject(Process);
    return Status;
}