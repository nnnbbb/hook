#pragma once
#include <Windows.h>

typedef struct _PACKINFO {
    DWORD newOEP;
    DWORD oldOEP;
} PACKINFO, *PPACKINFO;

// EXTERN_C
extern "C" _declspec(dllexport) PACKINFO g_PackInfo;