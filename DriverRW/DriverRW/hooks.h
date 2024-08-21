#pragma once

typedef enum _HOOK_DATA_CONTROL_CODE {
    ReadMemory = 0x1,
} HOOK_DATA_CONTROL_CODE;

typedef struct _HOOK_DATA {
    DWORD Magic;
    HOOK_DATA_CONTROL_CODE ControlCode;
    ULONG64 Body;
    ULONG64 BodySize;
    ULONG64 Result;
} HOOK_DATA, *PHOOK_DATA;

BOOLEAN Hooks();

BOOLEAN UnHooks();