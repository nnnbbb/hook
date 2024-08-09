#pragma once

#include <ntifs.h>
#include <ntddk.h>


extern "C" NTSYSAPI VOID NTAPI KeGenericCallDpc(IN PKDEFERRED_ROUTINE Routine, IN PVOID Context);
extern "C" NTSYSAPI VOID NTAPI KeSignalCallDpcDone(IN PVOID SystemArgument1);
extern "C" NTSYSAPI BOOLEAN NTAPI KeSignalCallDpcSynchronize(IN PVOID SystemArgument2);
