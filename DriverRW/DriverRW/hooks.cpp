#pragma once
#include "get-module.util.h"
#include "common.h"

// https://github.com/winsiderss/systeminformer/blob/master/phnt/include/ntexapi.h
NTSYSCALLAPI
NTSTATUS
NTAPI
NtConvertBetweenAuxiliaryCounterAndPerformanceCounter(
    _In_ BOOLEAN ConvertAuxiliaryToPerformanceCounter,
    _In_ PLARGE_INTEGER PerformanceOrAuxiliaryCounterValue,
    _Out_ PLARGE_INTEGER ConvertedValue,
    _Out_opt_ PLARGE_INTEGER ConversionError
);

pv g_OriginalAddress = nullptr;
pv* g_DataPtr = nullptr;


typedef struct _HOOK_DATA {
    DWORD Magic;
    int ControlCode;
} HOOK_DATA, *PHOOK_DATA;

typedef INT64 (*NtConvertBetweenAuxiliaryCounterAndPerformanceCounterWarpFnPtr)(PVOID, PVOID, PVOID);

INT64 HookMessageControl(PVOID UserMessage, PVOID Status, PVOID v13) {
    print("[*] NtConvertBetweenAuxiliaryCounterAndPerformanceCounter was called!\n");
    NtConvertBetweenAuxiliaryCounterAndPerformanceCounterWarpFnPtr OriginalFunction = (NtConvertBetweenAuxiliaryCounterAndPerformanceCounterWarpFnPtr)g_OriginalAddress;
    if (!ExGetPreviousMode() == UserMode) {
        print("[*] kernel mode request , retruning...\n");
        return OriginalFunction(UserMessage, Status, v13);
    }

    PHOOK_DATA UserData = (PHOOK_DATA)UserMessage;
    if (UserData->Magic == 0x77FF77FF) {
        print("[*] Received control number %d from client!\n", UserData->ControlCode);
        return NULL;
    } else {
        print("[*] magic didnt match , calling original function...\n");
        return OriginalFunction(UserMessage, Status, v13);
    }
}
BOOLEAN HookMessage(pv& Nt) {
    // NtConvertBetweenAuxiliaryCounterAndPerformanceCounter Pattern
    const char* Pattern = "41 56 48 83 EC 40 49 8B D9 49 8B F0 40 8A F9 65 48 8B 04 25 88 01 00 00 80 B8 32 02 00 00 00 0F 84 A5 00 00 00";
    ul64 NtPerformanceCounterAddr = (ul64)FindPatternSect(Nt, "PAGE", Pattern);
    NtPerformanceCounterAddr -= 0xf;
    print("NtPerformanceCounter = 0x%016llx\r\n", NtPerformanceCounterAddr);

    /*
    PAGE:0000000140917B40                            NtConvertBetweenAuxiliaryCounterAndPerformanceCounter proc near
    ......
    PAGE:0000000140917BD1 4C 8D 44 24 28                             lea     r8, [rsp+48h+var_20]
    PAGE:0000000140917BD6 48 8D 54 24 20                             lea     rdx, [rsp+48h+var_28]
    PAGE:0000000140917BDB 49 8B CE                                   mov     rcx, r14
    PAGE:0000000140917BDE 40 84 FF                                   test    dil, dil
    17BE1 距离 17B40 有 A1的偏移
    PAGE:0000000140917BE1 48 8B 05 80 CA B0 FF                       mov     rax, cs:off_140424668
                                   ^<----从这个位置开始
    */
    ul64 Offset = NtPerformanceCounterAddr + 0xA1;
    ul64 OffsetValue = *(ul32*)(Offset + 3) | 0xffffffff00000000;

    ul64 FinalFnAddress = Offset + OffsetValue + 7;
    print("FinalAddress address = 0x%016llX \n", FinalFnAddress);

    g_DataPtr = (pv*)(ul64*)FinalFnAddress;
    if (!MmIsAddressValid(g_DataPtr)) {
        print("MmIsAddressValid err \n");
        return FALSE;
    }

    g_OriginalAddress = *g_DataPtr;

    InterlockedExchange64((volatile LONG64*)g_DataPtr, (LONG64)HookMessageControl);
    return TRUE;
}

BOOLEAN Hooks() {
    PVOID addr = GetModuleBase("ntoskrnl.exe");
    pv halDll = 0;
    pv Nt = 0;
    getKernelModuleByName("hal.dll", &halDll, 0);
    getKernelModuleByName("ntoskrnl.exe", &Nt, 0);
    // NtConvertBetweenAuxiliaryCounterAndPerformanceCounter
    if (!HookMessage(Nt)) {
        return FALSE;
    }

    return TRUE;
}