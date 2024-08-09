#include "SimpleVT.h"
#include "Asm.h"


// #define ENABLE_EPT

#define VMERR_RET(x, s)                        \
    if (x != 0) {                              \
        print("VMERR_RET %s 调用【失败】", s); \
        return;                                \
    }


#define VMWRITE_ERR_RET(e, v)                \
    print("VM Write %s: 0x%016llX ", #e, v); \
    VMERR_RET(vmxwrite(e, v), "vmwrite - " #e);

#define VMREAD_ERR_RET(e, v)                \
    print("VM Read %s: 0x%016llX ", #e, v); \
    VMERR_RET(vmxread(e, v), "vmxread - " #e);


// void print(const char* format, ...) {
//     const char prefix[] = "[Debug]:";
//     char message[1024];
//
//     size_t prefix_len = strlen(prefix);
//     size_t message_size = sizeof(message);
//
//     strncpy(message, prefix, message_size);
//
//     va_list vl;
//     va_start(vl, format);
//     const int n = _vsnprintf(message + prefix_len, message_size - prefix_len, format, vl);
//     va_end(vl);
//
//     strncat(message, "\r\n", message_size - strlen(message) - 1);  // 确保不会越界
//
//     DbgPrint(message);
// }

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


__forceinline unsigned char vmxon(ULONG_PTR* VmxRegion) {
    int r = __vmx_on(VmxRegion);
    if (r != 0) {
        print("__vmx_on ====== %d", r);
    }
    return r;
}

__forceinline unsigned char vmxclear(ULONG_PTR* VmcsRegion) {
    return __vmx_vmclear(VmcsRegion);
}

__forceinline unsigned char vmxptrld(ULONG_PTR* VmcsRegion) {
    return __vmx_vmptrld(VmcsRegion);
}

__forceinline unsigned char vmxwrite(VMCSFIELDS Encodeing, ULONG_PTR Value) {
    return __vmx_vmwrite(Encodeing, Value);
}

__forceinline unsigned char vmxread(VMCSFIELDS Encodeing, ULONG_PTR* Value) {
    return __vmx_vmread(Encodeing, Value);
}

__forceinline unsigned char vmxlaunch() {
    return __vmx_vmlaunch();
}

__forceinline ULONG_PTR VmxAdjustMsr(ULONG_PTR MsrValue, ULONG_PTR DesiredValue) {
    DesiredValue &= (MsrValue >> 32);
    DesiredValue |= (MsrValue & 0xFFFFFFFF);
    return DesiredValue;
}

void ShowGuestRegister(ULONG_PTR* Registers) {
    ULONG_PTR Rip = 0, Rsp = 0;
    ULONG_PTR Cr0 = 0, Cr3 = 0, Cr4 = 0;
    ULONG_PTR Cs = 0, Ss = 0, Ds = 0, Es = 0, Fs = 0, Gs = 0, Tr = 0, Ldtr = 0;
    ULONG_PTR GsBase = 0, DebugCtl = 0, Dr7 = 0, RFlags = 0;
    ULONG_PTR IdtBase = 0, GdtBase = 0, IdtLimit = 0, GdtLimit = 0;
    print(
        "RAX = 0x%01611X RCX = 0x%01611X RDX = 0x%01611X RBX = 0x%01611X",
        Registers[R_RAX],
        Registers[R_RCX],
        Registers[R_RDX],
        Registers[R_RBX]
    );
    print(
        "RSP = 0x%01611X RBP = 0x%01611X RSI = 0x%01611X RDI = 0x%01611X",
        Registers[R_RSP],
        Registers[R_RBP],
        Registers[R_RSI],
        Registers[R_RDI]
    );
    print(
        "R8 = 0x%01611X R9 = 0x%01611X R10 = 0x%01611X R11 = 0x%01611X",
        Registers[R_R8],
        Registers[R_R9],
        Registers[R_R10],
        Registers[R_R11]
    );
    print(
        "R12 = 0x%01611X R13 = 0x%01611X R14 = 0x%01611X R15 = 0x%01611X",
        Registers[R_R12],
        Registers[R_R13],
        Registers[R_R14],
        Registers[R_R15]
    );
    __vmx_vmread(GUEST_RSP, &Rsp);
    __vmx_vmread(GUEST_RIP, &Rip);
    print("RSP = 0x%01611X RIP = 0x%01611X\n", Rsp, Rip);

    __vmx_vmread(GUEST_CR0, &Cr0);
    __vmx_vmread(GUEST_CR3, &Cr3);
    __vmx_vmread(GUEST_CR4, &Cr4);
    print("Debug:CR0 = 0x%01611X CR3= 0x%01611X CR4 = 0x%01611X\n", Cr0, Cr3, Cr4);

    __vmx_vmread(GUEST_CS_SELECTOR, &Cs);
    __vmx_vmread(GUEST_DS_SELECTOR, &Ds);
    __vmx_vmread(GUEST_ES_SELECTOR, &Es);
    __vmx_vmread(GUEST_FS_SELECTOR, &Fs);
    __vmx_vmread(GUEST_GS_SELECTOR, &Gs);
    __vmx_vmread(GUEST_TR_SELECTOR, &Tr);
    __vmx_vmread(GUEST_LDTR_SELECTOR, &Ldtr);
    print(
        "CS= 0x%01611X DS= 0x%01611X ES= 0x%01611X FS= 0x%01611X GS= 0x%01611X TR= 0x%01611X LDTR = 0x%01611X",
        Cs,
        Ds,
        Es,
        Fs,
        Gs,
        Tr,
        Ldtr
    );

    __vmx_vmread(GUEST_GS_BASE, &GsBase);
    __vmx_vmread(GUEST_IA32_DEBUGCTL, &DebugCtl);
    __vmx_vmread(GUEST_DR7, &Dr7);
    __vmx_vmread(GUEST_RFLAGS, &RFlags);
    print(
        "GsBase = 0x%01611X DebugCtl = 0x%01611X Dr7 = 0x%01611X RF1ags = 0x%01611X",
        GsBase,
        DebugCtl,
        Dr7,
        RFlags
    );
    __vmx_vmread(GUEST_IDTR_BASE, &IdtBase);
    __vmx_vmread(GUEST_IDTR_LIMIT, &IdtLimit);
    print(" IdtBase = 0x%016llX IdtLimit = 0x%01611X", IdtBase, IdtLimit);

    __vmx_vmread(GUEST_GDTR_BASE, &GdtBase);
    __vmx_vmread(GUEST_GDTR_LIMIT, &GdtLimit);
    print("GdtBase = 0x%016llX GdtLimit = 0x%0161lX", GdtBase, GdtLimit);

    return VOID();
}

EXTERN_C VOID VMExitHandler(ULONG_PTR* Registers) {
    // ShowGuestRegister(Registers);
    ULONG_PTR GuestRIP = 0;
    ULONG_PTR GuestRSP = 0;
    ULONG_PTR ExitInstruetionLength = 0;
    ULONG_PTR ExitReason = 0;
    ULONG_PTR GuestRFLAGS = 0;
    ULONGLONG MsrValue = 0;
    ULONG_PTR ExitQualification = 0;
    ULONG_PTR numCR = 0, opType = 0, accType = 0, reg = 0, cr3 = 0;
    int CPUInfo[4] = {};

    VMREAD_ERR_RET(GUEST_RIP, &GuestRIP);
    VMREAD_ERR_RET(VM_EXIT_INSTRUCTION_LEN, &ExitInstruetionLength);
    VMREAD_ERR_RET(VM_EXIT_REASON, &ExitReason);
    VMREAD_ERR_RET(GUEST_RFLAGS, &GuestRFLAGS);
    VMREAD_ERR_RET(EXIT_QUALIFICATION, &ExitQualification);

    switch (ExitReason) {
        case VMX_EXIT_CPUID:
            if (Registers[R_RAX] == 0x88888888) {
                ShowGuestRegister(Registers);
                Registers[R_RBX] = 0x66666666;
                Registers[R_RCX] = 0x33333333;
                Registers[R_RDX] = 0x22222222;
            } else {
                __cpuidex(CPUInfo, Registers[R_RAX], Registers[R_RCX]);
                Registers[R_RAX] = (ULONG_PTR)CPUInfo[0];
                Registers[R_RBX] = (ULONG_PTR)CPUInfo[1];
                Registers[R_RCX] = (ULONG_PTR)CPUInfo[2];
                Registers[R_RDX] = (ULONG_PTR)CPUInfo[3];
            }
            break;
        case VMX_EXIT_VMCALL: {
            ULONG64 JmpEIP;
            if (Registers[R_RAX] == 'BYE') {
                print("【VMCALL被调用】");
                JmpEIP = GuestRIP + ExitInstruetionLength;
                __vmx_off();
                Asm_AfterVMXOff(GuestRSP, JmpEIP);
            }
        } break;
        case VMX_EXIT_VMCLEAR:  // 拒绝嵌套VM指令的运行
        case VMX_EXIT_VMLAUNCH:
        case VMX_EXIT_VMPTRLD:
        case VMX_EXIT_VMPTRST:
        case VMX_EXIT_VMREAD:
        case VMX_EXIT_VMWRITE:
        case VMX_EXIT_VMRESUME:
        case VMX_EXIT_VMXON:
        case VMX_EXIT_VMXOFF:
            VMWRITE_ERR_RET(GUEST_RFLAGS, GuestRFLAGS | 0x1);
            break;
        case VMX_EXIT_RDMSR:
            MsrValue = __readmsr(Registers[R_RCX]);
            Registers[R_RAX] = LODWORD(MsrValue);
            Registers[R_RDX] = HIDWORD(MsrValue);
            break;
        case VMX_EXIT_WRMSR:
            MsrValue = MAKEQWORD(Registers[R_RAX], Registers[R_RDX]);
            __writemsr(Registers[R_RCX], MsrValue);
            break;
        case VMX_EXIT_MOV_CRX:
            numCR = ExitQualification & 0b1111;
            opType = (ExitQualification >> 6) & 1;
            accType = (ExitQualification & 0b110000) >> 4;
            reg = (ExitQualification >> 8) & 0b1111;
            if (numCR == 3 && opType == 0) {
                if (accType == 1) {  // mov reg,cr3
                    VMREAD_ERR_RET(GUEST_CR3, &cr3);
                    Registers[reg] = cr3;
                } else if (accType == 0) {  // mov cr3,reg
                    cr3 = Registers[reg];
                    VMWRITE_ERR_RET(GUEST_CR3, cr3);
                }
            }
            break;
        case VMX_EXIT_XSETBV:
            _xsetbv(Registers[R_RCX], MAKEQWORD(Registers[R_RAX], Registers[R_RDX]));
            break;
        case VMX_EXIT_INVD:
            __wbinvd();
            break;
        case VMX_EXIT_XCPT_OR_NMI:
            break;
        default:
            print("未知的 VM_EIXT 原因：0x%x", ExitReason);
            break;
    }

    __vmx_vmwrite(GUEST_RIP, GuestRIP + ExitInstruetionLength);

    return VOID();
}


BOOLEAN SimpleVT::Initialize() {
    if (!CheakVTSupported()) {
        return FALSE;
    }
    if (!CheakVTEnable()) {
        return FALSE;
    }

    m_VMXRegion = (ULONG_PTR*)MmAllocateNonCachedMemory(PAGE_SIZE);
    if (m_VMXRegion) {
        RtlSecureZeroMemory(m_VMXRegion, PAGE_SIZE);
    }

    m_VMCSRegion = (ULONG_PTR*)MmAllocateNonCachedMemory(PAGE_SIZE);
    if (m_VMCSRegion) {
        RtlSecureZeroMemory(m_VMCSRegion, PAGE_SIZE);
    }

    m_MsrBitmapRegion = (UINT8*)MmAllocateNonCachedMemory(PAGE_SIZE);
    if (m_MsrBitmapRegion) {
        RtlSecureZeroMemory(m_MsrBitmapRegion, PAGE_SIZE);
    }

    m_VMXRootStackRegion = (ULONG_PTR)MmAllocateNonCachedMemory(3 * PAGE_SIZE);
    if (m_VMXRootStackRegion) {
        SetVMExitHandler((ULONG_PTR)Asm_VMExitHandler, m_VMXRootStackRegion + 0x2000);
    }

    m_VMXOn = FALSE;

    InitVMCS();

    __writeds(0x28 | 0x3);
    __writees(0x28 | 0x3);
    __writefs(0x28 | 0x3);

    return TRUE;
}


VOID SimpleVT::UnInitialize() {
    if (m_VMXOn) {
        __vmx_off();
    }
    if (m_VMXRegion) {
        MmFreeNonCachedMemory(m_VMXRegion, PAGE_SIZE);
    }
    if (m_VMCSRegion) {
        MmFreeNonCachedMemory(m_VMCSRegion, PAGE_SIZE);
    }
    if (m_MsrBitmapRegion) {
        MmFreeNonCachedMemory(m_MsrBitmapRegion, PAGE_SIZE);
    }
    if (m_VMXRootStackRegion) {
        MmFreeNonCachedMemory((PVOID)m_VMXRootStackRegion, 3 * PAGE_SIZE);
    }
}


BOOLEAN SimpleVT::Install() {
    print("[%d]准备启动虚拟化", m_CPU);

    m_VMXOn = TRUE;
    vmxlaunch();  // 如果这句话执行成功, 就不会返回

    print("[%d]不应该执行到这里", m_CPU);

    if (m_VMXOn) {
        __vmx_off();
        m_VMXOn = FALSE;
    }

    return TRUE;
}


BOOLEAN SimpleVT::UnInstall() {
    return TRUE;
    if (m_VMXOn) {
        Asm_VmxCall('BYE');
        m_VMXOn = FALSE;
    }
    if (m_EPT) {
        MmFreeContiguousMemory(m_EPT);
    }
    if (m_VMXRegion) {
        MmFreeNonCachedMemory(m_VMXRegion, PAGE_SIZE);
    }
    if (m_VMCSRegion) {
        MmFreeNonCachedMemory(m_VMCSRegion, PAGE_SIZE);
    }
    if (m_MsrBitmapRegion) {
        MmFreeNonCachedMemory(m_MsrBitmapRegion, PAGE_SIZE);
    }
    if (m_VMXRootStackRegion) {
        MmFreeNonCachedMemory(&m_VMXRootStackRegion, PAGE_SIZE * 3);
    }

    return TRUE;
}


BOOLEAN SimpleVT::CheakVTSupported() {
    int ctx[4] = {0};
    __cpuidex(ctx, 1, 0);

    if ((ctx[2] & CPUID_1_ECX_VMX) == 0) {
        return FALSE;
    }
    return TRUE;
}


BOOLEAN SimpleVT::CheakVTEnable() {
    ULONG_PTR msr;
    msr = __readmsr(IA32_FEATURE_CONTROL_CODE);
    if ((msr & FEATURE_CONTROL_VMXON_ENABLED_OUTSIDE_SMX) == 0) {
        return FALSE;
    }

    return TRUE;
}

VOID SimpleVT::SetVMExitHandler(ULONG_PTR HandlerEntryPoint, ULONG_PTR HandlerStack) {
    m_HostState.rip = HandlerEntryPoint;
    m_HostState.rsp = ROUNDUP(HandlerStack, PAGE_SIZE);
    return VOID();
}


VOID SimpleVT::GdtEntryToVmcsFormat(
    ULONG selector,
    ULONG_PTR* base,
    ULONG_PTR* limit,
    ULONG_PTR* rights
) {

    GDT gdtr;
    PKGDTENTRY64 gdtEntry;
    *base = *limit = *rights = 0;
    if (selector == 0 || (selector & SELECTOR_TABLE_INDEX) != 0) {
        *rights = 0x10000;  // unusable
        return;
    }
    __sgdt(&gdtr);
    gdtEntry = (PKGDTENTRY64)(gdtr.ulBase + (selector & ~(0x3)));

    *limit = __segmentlimit(selector);
    *base = ((gdtEntry->Bytes.BaseHigh << 24) | (gdtEntry->Bytes.BaseMiddle << 16) | (gdtEntry->BaseLow)) & 0xFFFFFFFF;
    *base |= ((gdtEntry->Bits.Type & 0x10) == 0) ? ((uintptr_t)gdtEntry->BaseUpper << 32) : 0;
    *rights = (gdtEntry->Bytes.Flags1) | (gdtEntry->Bytes.Flags2 << 8);
    *rights |= (gdtEntry->Bits.Present) ? 0 : 0x10000;
    return VOID();
}

VOID SimpleVT::InitVMCS() {
    ULONG_PTR base, limit, rights;
    // Guest 状态
    StackPointer = (ULONG_PTR)Asm_StackPointer();
    // DbgBreakPoint();
    ReturnAddress = (ULONG_PTR)Asm_NextInstructionPointer();

    if (m_VMXOn) {
        DbgPrint("[%d]虚拟化【正在运行】!\n", m_CPU);
        return;
    }


    m_VMXRegionPhysAddr = MmGetPhysicalAddress(m_VMXRegion).QuadPart;
    m_VMCSRegionPhysAddr = MmGetPhysicalAddress(m_VMCSRegion).QuadPart;
    m_MsrBitmapRegionPhysAddr = MmGetPhysicalAddress(m_MsrBitmapRegion).QuadPart;
    print("[%d]VMX 虚拟地址 = %p,物理地址 = %p", m_CPU, m_VMXRegion, m_VMXRegionPhysAddr);
    print("[%d]VMCS 虚拟地址 = %p,物理地址 = %p", m_CPU, m_VMCSRegion, m_VMCSRegionPhysAddr);
    print("[%d]MsrBitmap 虚拟地址 = %p,物理地址 = %p", m_CPU, m_MsrBitmapRegion, m_MsrBitmapRegionPhysAddr);

    // Check Feature
    m_VmxBasic = __readmsr(IA32_VMX_BASIC_MSR_CODE);
    m_VmxFeatureControl = __readmsr(IA32_FEATURE_CONTROL_CODE);

    // 填充版本号
    *(PULONG32)m_VMXRegion = (ULONG32)m_VmxBasic;
    *(PULONG32)m_VMCSRegion = (ULONG32)m_VmxBasic;

    // 启用V的配置
    // 初始化 Guest
    m_GuestState.cs = __readcs();
    m_GuestState.ss = __readss();
    m_GuestState.es = __reades();
    m_GuestState.ds = __readds();
    m_GuestState.fs = __readfs();
    m_GuestState.gs = __readgs();

    m_GuestState.ldtr = __sldt();
    m_GuestState.tr = __str();
    m_GuestState.rflags = __readeflags();

    m_GuestState.rip = ReturnAddress;
    m_GuestState.rsp = StackPointer;

    __sgdt(&(m_GuestState.gdt));
    __sidt(&(m_GuestState.idt));

    m_GuestState.cr3 = __readcr3();
    m_GuestState.cr0 = ((__readcr0() & __readmsr(IA32_VMX_CR0_FIXED1)) | __readmsr(IA32_VMX_CR0_FIXED0));
    m_GuestState.cr4 = ((__readcr4() & __readmsr(IA32_VMX_CR4_FIXED1)) | __readmsr(IA32_VMX_CR4_FIXED0));
    m_GuestState.dr7 = __readdr(7);
    m_GuestState.msr_debugctl = __readmsr(IA32_DEBUGCTL);

    m_GuestState.msr_sysenter_cs = __readmsr(IA32_SYSENTER_CS);
    m_GuestState.msr_sysenter_eip = __readmsr(IA32_SYSENTER_EIP);
    m_GuestState.msr_sysenter_esp = __readmsr(IA32_SYSENTER_ESP);

    // 初始化Host
    __writecr0(m_GuestState.cr0);
    __writecr4(m_GuestState.cr4);

    m_HostState.cr0 = __readcr0();
    m_HostState.cr3 = __readcr3();
    m_HostState.cr4 = __readcr4();

    m_HostState.cs = __readcs() & 0xF8;
    m_HostState.ds = __readds() & 0xF8;
    m_HostState.ss = __readss() & 0xF8;
    m_HostState.es = __reades() & 0xF8;
    m_HostState.fs = __readfs() & 0xF8;
    m_HostState.gs = __readgs() & 0xF8;
    m_HostState.tr = __str();

    m_HostState.msr_sysenter_cs = __readmsr(IA32_SYSENTER_CS);
    m_HostState.msr_sysenter_eip = __readmsr(IA32_SYSENTER_EIP);
    m_HostState.msr_sysenter_esp = __readmsr(IA32_SYSENTER_ESP);

    __sgdt(&(m_HostState.gdt));
    __sidt(&(m_HostState.idt));

#ifdef ENABLE_EPT
    InitializeEPT();
#endif  // ENABLE_EPT

    // Setup VMX
    VMERR_RET(vmxon(&m_VMXRegionPhysAddr), "vmx on");
    print("vmxon 启动成功");
    m_VMXOn = TRUE;

    VMERR_RET(vmxclear(&m_VMCSRegionPhysAddr), "vmx clear");
    VMERR_RET(vmxptrld(&m_VMCSRegionPhysAddr), "vmx ptrld");
    print("Vmcs 装载成功");

    // Setup VMCS
    VMWRITE_ERR_RET(VMCS_LINK_POINTER, 0xFFFFFFFFFFFFFFFFL);

#ifdef ENABLE_EPT
    m_EPTP.AsUlonglong = 0;
    m_EPTP.u.PageWalkLength = 3;
    m_EPTP.u.Type = MTRR_TYPE_WB;
    m_EPTP.u.PageFrameNumber = MmGetPhysicalAddress(m_EPT->PML4T).QuadPart / PAGE_SIZE;

    VMWRITE_ERR_RET(EPT_POINTER, m_EPTP.AsUlonglong);
    VMWRITE_ERR_RET(VIRTUAL_PROCESSOR_ID, 1);
#endif  // ENABLE_EPT

    // VM执行控制域 Msr
    VMWRITE_ERR_RET(MSR_BITMAP, m_MsrBitmapRegionPhysAddr);  // 位图

#ifdef ENABLE_EPT

    VMWRITE_ERR_RET(
        SECONDARY_VM_EXEC_CONTROL,
        VmxAdjustMsr(
            __readmsr(IA32_VMX_PROCBASED_CTLS2),
            SECONDARY_EXEC_XSAVES | SECONDARY_EXEC_ENABLE_EPT |
                SECONDARY_EXEC_ENABLE_RDTSCP | SECONDARY_EXEC_ENABLE_VPID
        )
    );

#else
    // 下面的代码开启 RDTSC 事件
    VMWRITE_ERR_RET(
        SECONDARY_VM_EXEC_CONTROL,
        VmxAdjustMsr(
            __readmsr(IA32_VMX_PROCBASED_CTLS2),
            SECONDARY_EXEC_XSAVES | SECONDARY_EXEC_ENABLE_RDTSCP
        )
    );

#endif  // ENABLE_EPT

    VMWRITE_ERR_RET(
        PIN_BASED_VM_EXEC_CONTROL,
        VmxAdjustMsr(__readmsr(MSR_IA32_VMX_TRUE_PINBASED_CTLS), 0)
    );  // 不监控虚拟机

    VMWRITE_ERR_RET(
        CPU_BASED_VM_EXEC_CONTROL,
        VmxAdjustMsr(__readmsr(MSR_IA32_VMX_TRUE_PROCBASED_CTLS),
                     CPU_BASED_ACTIVATE_SECONDARY_CONTROLS | CPU_BASED_ACTIVATE_MSR_BITMAP)
    );  // 不监控虚拟机

    // VMExit和VMEntry控制域
    VMWRITE_ERR_RET(
        VM_EXIT_CONTROLS,
        VmxAdjustMsr(
            __readmsr(MSR_IA32_VMX_TRUE_EXIT_CTLS),
            VM_EXIT_IA32E_MODE | VM_EXIT_ACK_INTR_ON_EXIT
        )
    );
    VMWRITE_ERR_RET(
        VM_ENTRY_CONTROLS,
        VmxAdjustMsr(
            __readmsr(MSR_IA32_VMX_TRUE_ENTRY_CTLS),
            VM_ENTRY_IA32E_MODE
        )
    );

    // 客户机状态
    GdtEntryToVmcsFormat(m_GuestState.cs, &base, &limit, &rights);
    VMWRITE_ERR_RET(GUEST_CS_SELECTOR, m_GuestState.cs);
    VMWRITE_ERR_RET(GUEST_CS_LIMIT, limit);
    VMWRITE_ERR_RET(GUEST_CS_AR_BYTES, rights);
    VMWRITE_ERR_RET(GUEST_CS_BASE, base);

    GdtEntryToVmcsFormat(m_GuestState.ds, &base, &limit, &rights);
    VMWRITE_ERR_RET(GUEST_DS_SELECTOR, m_GuestState.ds);
    VMWRITE_ERR_RET(GUEST_DS_LIMIT, limit);
    VMWRITE_ERR_RET(GUEST_DS_AR_BYTES, rights);
    VMWRITE_ERR_RET(GUEST_DS_BASE, base);

    GdtEntryToVmcsFormat(m_GuestState.ss, &base, &limit, &rights);
    VMWRITE_ERR_RET(GUEST_SS_SELECTOR, m_GuestState.ss);
    VMWRITE_ERR_RET(GUEST_SS_LIMIT, limit);
    VMWRITE_ERR_RET(GUEST_SS_AR_BYTES, rights);
    VMWRITE_ERR_RET(GUEST_SS_BASE, base);

    GdtEntryToVmcsFormat(m_GuestState.es, &base, &limit, &rights);
    VMWRITE_ERR_RET(GUEST_ES_SELECTOR, m_GuestState.es);
    VMWRITE_ERR_RET(GUEST_ES_LIMIT, limit);
    VMWRITE_ERR_RET(GUEST_ES_AR_BYTES, rights);
    VMWRITE_ERR_RET(GUEST_ES_BASE, base);

    GdtEntryToVmcsFormat(m_GuestState.fs, &base, &limit, &rights);
    VMWRITE_ERR_RET(GUEST_FS_SELECTOR, m_GuestState.fs);
    VMWRITE_ERR_RET(GUEST_FS_LIMIT, limit);
    VMWRITE_ERR_RET(GUEST_FS_AR_BYTES, rights);
    VMWRITE_ERR_RET(GUEST_FS_BASE, base);

    GdtEntryToVmcsFormat(m_GuestState.gs, &base, &limit, &rights);
    base = __readmsr(MSR_GS_BASE);
    VMWRITE_ERR_RET(GUEST_FS_SELECTOR, m_GuestState.fs);
    VMWRITE_ERR_RET(GUEST_FS_LIMIT, limit);
    VMWRITE_ERR_RET(GUEST_FS_AR_BYTES, rights);
    VMWRITE_ERR_RET(GUEST_FS_BASE, base);
    m_HostState.fsbase = base;

    VMWRITE_ERR_RET(GUEST_GS_SELECTOR, m_GuestState.gs);
    VMWRITE_ERR_RET(GUEST_GS_LIMIT, limit);
    VMWRITE_ERR_RET(GUEST_GS_AR_BYTES, rights);
    VMWRITE_ERR_RET(GUEST_GS_BASE, base);
    m_HostState.gsbase = base;

    GdtEntryToVmcsFormat(m_GuestState.tr, &base, &limit, &rights);
    VMWRITE_ERR_RET(GUEST_TR_SELECTOR, m_GuestState.tr);
    VMWRITE_ERR_RET(GUEST_TR_LIMIT, limit);
    VMWRITE_ERR_RET(GUEST_TR_AR_BYTES, rights);
    VMWRITE_ERR_RET(GUEST_TR_BASE, base);
    m_HostState.trbase = base;

    GdtEntryToVmcsFormat(m_GuestState.ldtr, &base, &limit, &rights);
    VMWRITE_ERR_RET(GUEST_LDTR_SELECTOR, m_GuestState.ldtr);
    VMWRITE_ERR_RET(GUEST_LDTR_LIMIT, limit);
    VMWRITE_ERR_RET(GUEST_LDTR_AR_BYTES, rights);
    VMWRITE_ERR_RET(GUEST_LDTR_BASE, base);

    VMWRITE_ERR_RET(GUEST_GDTR_BASE, m_GuestState.gdt.ulBase);
    VMWRITE_ERR_RET(GUEST_GDTR_LIMIT, m_GuestState.gdt.wLimit);

    VMWRITE_ERR_RET(GUEST_IDTR_BASE, m_GuestState.idt.ulBase);
    VMWRITE_ERR_RET(GUEST_IDTR_LIMIT, m_GuestState.idt.wLimit);

    // GUEST CR
    VMWRITE_ERR_RET(GUEST_CR0, m_GuestState.cr0);
    VMWRITE_ERR_RET(GUEST_CR3, m_GuestState.cr3);
    VMWRITE_ERR_RET(GUEST_CR4, m_GuestState.cr4);
    VMWRITE_ERR_RET(CR0_READ_SHADOW, m_GuestState.cr0);
    VMWRITE_ERR_RET(CR4_READ_SHADOW, m_GuestState.cr4);

    VMWRITE_ERR_RET(GUEST_DR7, m_GuestState.dr7);
    VMWRITE_ERR_RET(GUEST_IA32_DEBUGCTL, m_GuestState.msr_debugctl);

    VMWRITE_ERR_RET(GUEST_RSP, m_GuestState.rsp);
    VMWRITE_ERR_RET(GUEST_RIP, m_GuestState.rip);
    VMWRITE_ERR_RET(GUEST_RFLAGS, m_GuestState.rflags);


    // 宿主机状态
    VMWRITE_ERR_RET(HOST_CS_SELECTOR, m_HostState.cs);
    VMWRITE_ERR_RET(HOST_DS_SELECTOR, m_HostState.ds);
    VMWRITE_ERR_RET(HOST_SS_SELECTOR, m_HostState.ss);
    VMWRITE_ERR_RET(HOST_ES_SELECTOR, m_HostState.es);

    VMWRITE_ERR_RET(HOST_FS_BASE, m_HostState.fsbase);
    VMWRITE_ERR_RET(HOST_FS_SELECTOR, m_HostState.fs);

    VMWRITE_ERR_RET(HOST_GS_BASE, m_HostState.gsbase);
    VMWRITE_ERR_RET(HOST_GS_SELECTOR, m_HostState.gs);

    VMWRITE_ERR_RET(HOST_TR_BASE, m_HostState.trbase);
    VMWRITE_ERR_RET(HOST_TR_SELECTOR, m_HostState.tr);

    VMWRITE_ERR_RET(HOST_GDTR_BASE, m_HostState.gdt.ulBase);
    VMWRITE_ERR_RET(HOST_IDTR_BASE, m_HostState.idt.ulBase);

    // HOST CR0
    VMWRITE_ERR_RET(HOST_CR0, m_HostState.cr0);
    VMWRITE_ERR_RET(HOST_CR3, m_HostState.cr3);
    VMWRITE_ERR_RET(HOST_CR4, m_HostState.cr4);

    VMWRITE_ERR_RET(HOST_RSP, m_HostState.rsp);
    VMWRITE_ERR_RET(HOST_RIP, m_HostState.rip);

    // 初始化VMCS完毕
    return;
}

VOID SimpleVT::InitializeEPT() {
    PHYSICAL_ADDRESS highest;
    MTRR_CAPABILITIES mtrrCapabilities;


    highest.QuadPart = 0xFFFFFFFFFFFFFFFF;
    MTRR_VARIABLE_BASE mtrrBase;
    MTRR_VARIABLE_MASK mtrrMask;

    SHV_MTRR_RANGE mtrrData[16] = {};
    unsigned int bit = 0;

    ULONG_PTR LargePageAddress = 0;
    ULONG_PTR CandidateMemoryType = 0;

    m_EPT = (PVMX_EPT)MmAllocateContiguousMemory(sizeof(VMX_EPT), highest);
    if (!m_EPT) {
        return;
    }

    // 读取范围
    RtlSecureZeroMemory(m_EPT, sizeof(VMX_EPT));
    mtrrCapabilities.AsUlonglong = __readmsr(MTRR_MSR_CAPABILITIES);

    for (int i = 0; i < mtrrCapabilities.u.VarCnt; i++) {
        mtrrBase.AsUlonglong = __readmsr(MTRR_MSR_VARIABLE_BASE + i * 2);
        mtrrMask.AsUlonglong = __readmsr(MTRR_MSR_VARIABLE_MASK + i * 2);

        mtrrData[i].Type = mtrrBase.u.Type;
        mtrrData[i].Enabled = mtrrMask.u.Enabled;

        if (mtrrData[i].Enabled != FALSE) {
            // 设置基地址
            mtrrData[i].PhysicalAddressMin = mtrrBase.u.PhysBase * MTRR_PAGE_SIZE;
            _BitScanForward64((ULONG*)&bit, mtrrMask.u.PhysMask * MTRR_PAGE_SIZE);
            mtrrData[i].PhysicalAddressMax = mtrrData[i].PhysicalAddressMin + (1ULL << bit) - 1;
        }
    }
    // 填充EPT内容

    m_EPT->PML4T[0].u.Read = 1;
    m_EPT->PML4T[0].u.Write = 1;
    m_EPT->PML4T[0].u.Execute = 1;
    m_EPT->PML4T[0].u.PageFrameNumber = MmGetPhysicalAddress(m_EPT->PDPT).QuadPart / PAGE_SIZE;
    // print("[%d]  mtrrCapabilities.AsUlonglong = 0x%016llX", m_CPU, m_EPT->PML4E[0].u.PageFrameNumber);

    for (int i = 0; i < PDPTE_ENTRY_COUNT; i++) {
        // 设置PDE表的页面数量
        m_EPT->PDPT[0].u.Read = 1;
        m_EPT->PDPT[0].u.Write = 1;
        m_EPT->PDPT[0].u.Execute = 1;
        m_EPT->PDPT[0].u.PageFrameNumber = MmGetPhysicalAddress(&m_EPT->PDT[i][0]).QuadPart / PAGE_SIZE;
    }

    for (size_t i = 0; i < PDPTE_ENTRY_COUNT; i++) {
        for (size_t j = 0; j < PDE_ENTRY_COUNT; j++) {
            m_EPT->PDT[i][j].u.Read = 1;
            m_EPT->PDT[i][j].u.Write = 1;
            m_EPT->PDT[i][j].u.Execute = 1;
            m_EPT->PDT[i][j].u.Large = (i * 512) + j;

            LargePageAddress = m_EPT->PDT[i][j].u.PageFrameNumber * _2MB;

            CandidateMemoryType = MTRR_TYPE_WB;

            for (size_t k = 0; k < sizeof(mtrrData) / sizeof(mtrrData[0]); k++) {
                // 检测内存是否启用
                if (mtrrData[k].Enabled != FALSE) {
                    // 检查大页面物理地址的边界，如果单物理页面为4KB，则改写入口为2MB
                    if (((LargePageAddress + _2MB) >= mtrrData[k].PhysicalAddressMin) &&
                        (LargePageAddress <= mtrrData[k].PhysicalAddressMax)) {
                        CandidateMemoryType = mtrrData[k].Type;
                    }
                }
            }

            m_EPT->PDT[i][j].u.Type = CandidateMemoryType;
        }
    }
}