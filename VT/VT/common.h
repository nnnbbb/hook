#pragma once
#include <ntifs.h>
#include <ntstrsafe.h>
#include <intrin.h>

#include "WinApi.h"
#include "CPU.h"
#include "VMX.h"

void print(const char* format, ...);

bool CheckBios();
bool CheckCpuid();
bool CheckCr4();
void IsSupportVT(
    _In_ struct _KDPC* Dpc,
    _In_opt_ PVOID DeferredContext,
    _In_opt_ PVOID SystemArgument1,
    _In_opt_ PVOID SystemArgument2
);

typedef struct {
    USHORT sel;
    USHORT attributes;
    ULONG32 limit;
    ULONG64 base;
} SEGMENT_SELECTOR;

typedef struct {
    USHORT LimitLow;
    USHORT BaseLow;
    UCHAR BaseMid;
    UCHAR AttributesLow;
    struct {
        UCHAR LimitHigh : 4;
        UCHAR AttributesHigh : 4;
    };
    UCHAR BaseHigh;
} SEGMENT_DESCRIPTOR, *PSEGMENT_DESCRIPTOR;