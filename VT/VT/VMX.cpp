#include "VMX.h"
#include "asm.h"


VCPU g_vcpu[128];

int VMXInitCpu() {
    size_t cpuNumber = KeGetCurrentProcessorNumber();
    unsigned char VmxStatus = 0;

    VCPU currentCpu = g_vcpu[cpuNumber];
    PHYSICAL_ADDRESS phys = {0};
    PHYSICAL_ADDRESS PhysicalVmon = {0};
    PHYSICAL_ADDRESS PhysicalVmcs = {0};
    phys.QuadPart = MAXULONG64;
    PVMX_VMCS Vmxon = (PVMX_VMCS)MmAllocateContiguousMemory(sizeof(VMX_VMCS), phys);
    PVMX_VMCS Vmcs = (PVMX_VMCS)MmAllocateContiguousMemory(sizeof(VMX_VMCS), phys);

    if (Vmxon == NULL || Vmcs == NULL) {
        return 1;
    }
    RtlZeroMemory(Vmxon, sizeof(VMX_VMCS));
    RtlZeroMemory(Vmcs, sizeof(VMX_VMCS));
    Vmxon->RevisionId = (ULONG)__readmsr(MSR_IA32_VMX_BASIC);
    currentCpu.VMXON = Vmxon;
    currentCpu.VMCS = Vmcs;

    ULONG64 mycr4 = __readcr4();
    mycr4 = MSR_IA32_VMX_CR4_FIXED0;
    mycr4 &= MSR_IA32_VMX_CR4_FIXED1;
    ULONG64 mycr0 = __readcr0();
    mycr0 = MSR_IA32_VMX_CR0_FIXED0;
    mycr0 &= MSR_IA32_VMX_CR0_FIXED1;
    PhysicalVmon = MmGetPhysicalAddress(Vmxon);
    PhysicalVmcs = MmGetPhysicalAddress(Vmcs);

    VmxStatus = __vmx_on((unsigned __int64 *)&PhysicalVmon);
    print("Debug: __vmx_on ====== %d", VmxStatus);

    if (VmxStatus != 0) {
        return false;
    }
    VmxStatus = __vmx_vmclear((unsigned __int64 *)&PhysicalVmcs);
    if (VmxStatus != 0) {
        __vmx_off();
        return false;
    }
    VmxStatus = __vmx_vmptrld((unsigned __int64 *)&PhysicalVmcs);
    if (VmxStatus != 0) {
        __vmx_off();
        return false;
    }
}

int VMXSetupVmcs() {
    SEGMENT_SELECTOR segMentSelector = {0};
    segMentSelector.sel = RegGetEs();
    segMentSelector.limit = __segmentlimit(segMentSelector.sel);
    segMentSelector.base = 0;

    return 0;
}
