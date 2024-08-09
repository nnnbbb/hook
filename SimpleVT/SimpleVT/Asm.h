#pragma once

EXTERN_C ULONG_PTR Asm_StackPointer();
EXTERN_C ULONG_PTR Asm_NextInstructionPointer();

EXTERN_C ULONG_PTR __readcs();
EXTERN_C ULONG_PTR __readss();
EXTERN_C ULONG_PTR __reades();
EXTERN_C ULONG_PTR __readds();
EXTERN_C ULONG_PTR __readfs();
EXTERN_C ULONG_PTR __readgs();
EXTERN_C ULONG_PTR __sldt();
EXTERN_C ULONG_PTR __str();
EXTERN_C ULONG_PTR __sgdt(PGDT gdtr);

EXTERN_C void __invd(void);
EXTERN_C void __writeds(ULONG_PTR DS);
EXTERN_C void __writees(ULONG_PTR ES);
EXTERN_C void __writefs(ULONG_PTR FS);

EXTERN_C void Asm_VMExitHandler();
EXTERN_C void Asm_VmxCall(ULONG64 uCallNumber);
EXTERN_C void Asm_AfterVMXOff(ULONG64 JmpESP, ULONG64 JmpEIP);