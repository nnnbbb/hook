#pragma once
#include <ntifs.h>
#include <intrin.h>
#include "CPU.h"
#include "common.h"


typedef struct _VMX_VMCS {
    ULONG RevisionId;
    ULONG AbortIndicator;
    UCHAR Data[PAGE_SIZE - 2 * sizeof(ULONG)];
} VMX_VMCS, *PVMX_VMCS;

typedef struct VCPU {
    PVMX_VMCS VMXON;
    PVMX_VMCS VMCS;
} VCPU, *PVCPU;


int VMXInitCpu();
int VMXSetupVmcs();