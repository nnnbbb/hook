#include <ntddk.h>;


EXTERN_C USHORT RegGetEs();

EXTERN_C USHORT RegGetCs();

EXTERN_C USHORT RegGetSs();

EXTERN_C USHORT RegGetDs();

EXTERN_C USHORT RegGetFs();

EXTERN_C USHORT RegGetGs();

EXTERN_C USHORT RegGetDr0();

EXTERN_C USHORT RegGetDr1();

EXTERN_C USHORT RegGetDr2();

EXTERN_C USHORT RegGetDr3();

EXTERN_C USHORT RegGetDr4();

EXTERN_C USHORT RegGetGdt();

EXTERN_C ULONG64 GetGdtBase();
