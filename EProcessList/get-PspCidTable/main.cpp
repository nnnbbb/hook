#include <ntifs.h>
#include <ntstrsafe.h>
#include <windef.h>

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

ULONG GetPspCidTable() {
    /*
    0: kd> uf PsLookupProcessByProcessId
       nt!PsLookupProcessByProcessId:
       859f47e0 8bff            mov     edi,edi
       859f47e2 55              push    ebp
       859f47e3 8bec            mov     ebp,esp
       859f47e5 51              push    ecx
       859f47e6 53              push    ebx
       859f47e7 56              push    esi
       859f47e8 648b3524010000  mov     esi,dword ptr fs:[124h]
       859f47ef 57              push    edi
       859f47f0 66ff8e3e010000  dec     word ptr [esi+13Eh]
       859f47f7 90              nop
       859f47f8 8b4d08          mov     ecx,dword ptr [ebp+8]
       859f47fb b203            mov     dl,3
       859f47fd e8 2e 03 00 00       call    nt!PspReferenceCidTableEntry (!!!! 859f4b30 !!!!)  859f4b30 - 5 - 859f47fd
    = 032e 859f4802 8bf8            mov     edi,eax 859f4804 85ff            test    edi,edi 859f4806 7424            je
    nt!PsLookupProcessByProcessId+0x4c (859f482c)  Branch

       859f4808 e8 9314d1ff      call    nt!PsGetCurrentServerSilo (85705ca0)

       85705ca0 - 5 - 859f4808

   */
    ULONG FnAddress = (ULONG)PsLookupProcessByProcessId;
    print("PsLookupProcessByProcessId addr = %p \n", FnAddress);
    ULONG offset = FnAddress + 0x1d;
    print("offset = %p \n", *(ULONG*)(offset + 1));
    ULONG PspReferenceCidTableEntryAddress = offset + *(ULONG*)(offset + 1) + 5;
    print("PspReferenceCidTableEntry Jmp address = %p \n", PspReferenceCidTableEntryAddress);
    /*
    3: kd> uf PspReferenceCidTableEntry
        nt!PspReferenceCidTableEntry:
        859f4b30 8bff            mov     edi,edi
        859f4b32 55              push    ebp
        859f4b33 8bec            mov     ebp,esp
        859f4b35 83ec0c          sub     esp,0Ch
        859f4b38 a1 ac de 90 85      mov     eax,dword ptr [nt!PspCidTable (8590deac)]
        859f4b3d c745f400000000  mov     dword ptr [ebp-0Ch],0
        859f4b44 c745f800000000  mov     dword ptr [ebp-8],0
        859f4b4b 53              push    ebx
        859f4b4c 8ada            mov     bl,dl
        859f4b4e 56              push    esi
        859f4b4f 57              push    edi
        859f4b50 f7c1fc070000    test    ecx,7FCh
        859f4b56 0f84e0000000    je      nt!PspReferenceCidTableEntry+0x10c (859f4c3c)  Branch
    */
    ULONG PspCidTable = *(ULONG*)(PspReferenceCidTableEntryAddress + 0x8 + 0x1);
    return PspCidTable;
}

/* 解析一级表
BaseAddr：一级表的基地址
index1：第几个一级表
index2：第几个二级表
*/
VOID parse_table_1(ULONG BaseAddr, INT index1, INT index2) {
    // 遍历一级表（每个表项大小 16 ），表大小 4k，所以遍历 4096/16 = 526 次
    PEPROCESS eprocess = NULL;
    PETHREAD ethread = NULL;
    INT i_id = 0;
    for (INT i = 0; i < 256; i++) {
        if (!MmIsAddressValid((PVOID)(BaseAddr + i * 16))) {
            print("非法地址= %p \n", BaseAddr + i * 16);
            continue;
        }

        ULONG ul_recode = *(PULONG)(BaseAddr + i * 16);

        // 解密
        ULONG ul_decode = (LONG)ul_recode >> 0x10;
        ul_decode &= 0xfffffffffffffff0;

        // 判断是进程还是线程
        i_id = i * 4 + 1024 * index1 + 512 * index2 * 1024;
        if (PsLookupProcessByProcessId((HANDLE)i_id, &eprocess) == STATUS_SUCCESS) {
            ULONG currentProcessName = (ULONG)eprocess + 0x1ac;  // ImageFileName
            print("Found process %s \r\n", currentProcessName);

            // print("进程PID: %d | ID: %d | 内存地址: %p | 对象: %p \n", i_id, i, BaseAddr + i * 0x10, ul_decode);
        }
        // else if (PsLookupThreadByThreadId((HANDLE)i_id, &ethread) == STATUS_SUCCESS) {
        //    print("线程TID: %d | ID: %d | 内存地址: %p | 对象: %p \n", i_id, i, BaseAddr + i * 0x10, ul_decode);
        //}
    }
}

/* 解析二级表
BaseAddr：二级表基地址
index2：第几个二级表
*/
VOID parse_table_2(ULONG BaseAddr, INT index2) {
    // 遍历二级表（每个表项大小 8）,表大小 4k，所以遍历 4096/8 = 512 次
    ULONG ul_baseAddr_1 = 0;
    for (INT i = 0; i < 512; i++) {
        if (!MmIsAddressValid((PVOID)(BaseAddr + i * 8))) {
            // print("非法二级表指针（1）:%p \n", BaseAddr + i * 8);
            continue;
        }
        if (!MmIsAddressValid((PVOID) * (PULONG)(BaseAddr + i * 8))) {
            // print("非法二级表指针（2）:%p \n", BaseAddr + i * 8);
            continue;
        }
        ul_baseAddr_1 = *(PULONG)(BaseAddr + i * 8);
        parse_table_1(ul_baseAddr_1, i, index2);
    }
}

/* 解析三级表
BaseAddr：三级表基地址
*/
VOID parse_table_3(ULONG BaseAddr) {
    // 遍历三级表（每个表项大小 8）,表大小 4k，所以遍历 4096/8 = 512 次
    ULONG ul_baseAddr_2 = 0;
    for (INT i = 0; i < 512; i++) {
        if (!MmIsAddressValid((PVOID64)(BaseAddr + i * 8))) {
            continue;
        }
        if (!MmIsAddressValid((PVOID64) * (PULONG)(BaseAddr + i * 8))) {
            continue;
        }
        ul_baseAddr_2 = *(PULONG)(BaseAddr + i * 8);
        parse_table_2(ul_baseAddr_2, i);
    }
}

void EnumProcessByPspCidTable() {
    ULONG PspCidTable = GetPspCidTable();

    if (!MmIsAddressValid((PVOID)PspCidTable)) {
        return;
    }
    print("PspCidTableAddress = 0x%x\r\n", PspCidTable);
    ULONG PsCidTableAddress = *(ULONG*)PspCidTable;
    print("PsCidTable=%x\r\n", PsCidTableAddress);
    /*
    2: kd> dt _HANDLE_TABLE
       +0x000 NextHandleNeedingPool : Uint4B
       +0x004 ExtraInfoPages   : Int4B
       +0x008 TableCode        : Uint4B
       +0x00c QuotaProcess     : Ptr32 _EPROCESS
       +0x010 HandleTableList  : _LIST_ENTRY
       +0x018 UniqueProcessId  : Uint4B
       +0x01c Flags            : Uint4B
       +0x01c StrictFIFO       : Pos 0, 1 Bit
       +0x01c EnableHandleExceptions : Pos 1, 1 Bit

    */

    ULONG ul_tableCode = *(ULONG*)((ULONG)PsCidTableAddress + 8);
    print("ul_tableCode = %p \n", ul_tableCode);

    // 取低 2位（二级制11 = 3）
    INT i_low2 = ul_tableCode & 3;
    print("i_low2 = %X \n", i_low2);

    // 一级表
    if (i_low2 == 0) {
        // TableCode 低 2位抹零（二级制11 = 3）
        parse_table_1(ul_tableCode & (~3), 0, 0);
    }
    // 二级表
    else if (i_low2 == 1) {
        // TableCode 低 2位抹零（二级制11 = 3）
        parse_table_2(ul_tableCode & (~3), 0);
    }
    // 三级表
    else if (i_low2 == 2) {
        // TableCode 低 2位抹零（二级制11 = 3）
        parse_table_3(ul_tableCode & (~3));
    } else {
        print("LyShark提示: 错误,非法! ");
    }
}

void DriverUnload(IN PDRIVER_OBJECT DriverObject) {
    UNREFERENCED_PARAMETER(DriverObject);

    print("debug驱动卸载\r\n");
}
extern "C" NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING path) {
    // DbgBreakPoint();
    UNREFERENCED_PARAMETER(path);
    DriverObject->DriverUnload = DriverUnload;

    EnumProcessByPspCidTable();
    return STATUS_SUCCESS;
}

//// 获取 PspCidTable
// BOOLEAN get_PspCidTable(ULONG* tableAddr) {
//    // 获取 PsLookupProcessByProcessId 地址
//    UNICODE_STRING uc_funcName;
//    RtlInitUnicodeString(&uc_funcName, L"PsLookupProcessByProcessId");
//    ULONG ul_funcAddr = (ULONG)MmGetSystemRoutineAddress(&uc_funcName);
//    if (ul_funcAddr == NULL) {
//        return FALSE;
//    }
//    print("PsLookupProcessByProcessId addr = %p \n", ul_funcAddr);
//
//    // 前 40 字节有 call（PspReferenceCidTableEntry）
//    /*
//    0: kd> uf PsLookupProcessByProcessId
//        nt!PsLookupProcessByProcessId:
//        fffff802`0841cfe0 48895c2418      mov     qword ptr [rsp+18h],rbx
//        fffff802`0841cfe5 56              push    rsi
//        fffff802`0841cfe6 4883ec20        sub     rsp,20h
//        fffff802`0841cfea 48897c2438      mov     qword ptr [rsp+38h],rdi
//        fffff802`0841cfef 488bf2          mov     rsi,rdx
//        fffff802`0841cff2 65488b3c2588010000 mov   rdi,qword ptr gs:[188h]
//        fffff802`0841cffb 66ff8fe6010000  dec     word ptr [rdi+1E6h]
//        fffff802`0841d002 b203            mov     dl,3
//        fffff802`0841d004 e8 87000000      call    nt!PspReferenceCidTableEntry (fffff802`0841d090)
//        fffff802`0841d009 488bd8          mov     rbx,rax
//        fffff802`0841d00c 4885c0          test    rax,rax
//        fffff802`0841d00f 7435            je      nt!PsLookupProcessByProcessId+0x66 (fffff802`0841d046)  Branch
//
//    0: kd> uf PsLookupProcessByProcessId
//        nt!PsLookupProcessByProcessId:
//        859f47e0 8bff            mov     edi,edi
//        859f47e2 55              push    ebp
//        859f47e3 8bec            mov     ebp,esp
//        859f47e5 51              push    ecx
//        859f47e6 53              push    ebx
//        859f47e7 56              push    esi
//        859f47e8 648b3524010000  mov     esi,dword ptr fs:[124h]
//        859f47ef 57              push    edi
//        859f47f0 66ff8e3e010000  dec     word ptr [esi+13Eh]
//        859f47f7 90              nop
//        859f47f8 8b4d08          mov     ecx,dword ptr [ebp+8]
//        859f47fb b203            mov     dl,3
//        859f47fd e8 2e030000      call    nt!PspReferenceCidTableEntry (859f4b30)
//        859f4802 8bf8            mov     edi,eax
//        859f4804 85ff            test    edi,edi
//        859f4806 7424            je      nt!PsLookupProcessByProcessId+0x4c (859f482c)  Branch
//    */
//    ULONG ul_entry = 0;
//    for (INT i = 0; i < 100; i++) {
//        // fffff802`0841d004 e8 87 00 00 00      call    nt!PspReferenceCidTableEntry (fffff802`0841d090)
//        // 859f47fd          e8 2e 03 00 00      call    nt!PspReferenceCidTableEntry (859f4b30)
//
//        UCHAR newFuncAddr = *(PUCHAR)(ul_funcAddr + i);
//
//        if (newFuncAddr == 0xe8) {
//            print("UCHAR value: 0x%x\n", newFuncAddr);
//            ul_entry = ul_funcAddr + i;
//            break;
//        }
//    }
//    print("ul_entry = %p \n", ul_entry);
//
//    if (ul_entry != 0) {
//        // 解析 call 地址
//        ULONG i_callCode = *(ULONG*)(ul_entry + 1);
//        ULONG i_callCode2 = *(ULONG*)(ul_entry + 2);
//        print("i_callCode = %p  address = 0x%x\n", i_callCode, ul_entry + 1);
//        print("i_callCode2 = %p  address = 0x%x\n", i_callCode2, ul_entry + 2);
//        ULONG ul_callJmp = ul_entry + i_callCode + 5;
//        print("PspReferenceCidTableEntry Jmp address = %p \n", ul_callJmp);
//
//        // 来到 call（PspReferenceCidTableEntry） 内找 PspCidTable
//        /*
//        0: kd> uf PspReferenceCidTableEntry
//            nt!PspReferenceCidTableEntry+0x115:
//            fffff802`0841d1a5 48 8b 0d 84 73 f5 ff  mov     rcx,qword ptr [nt!PspCidTable (fffff802`08374530)]
//            fffff802`0841d1ac b801000000      mov     eax,1
//            fffff802`0841d1b1 f0480fc107      lock xadd qword ptr [rdi],rax
//            fffff802`0841d1b6 4883c130        add     rcx,30h
//            fffff802`0841d1ba f0830c2400      lock or dword ptr [rsp],0
//            fffff802`0841d1bf 48833900        cmp     qword ptr [rcx],0
//            fffff802`0841d1c3 0f843fffffff    je      nt!PspReferenceCidTableEntry+0x78 (fffff802`0841d108)  Branch
//
//        3: kd> uf PspReferenceCidTableEntry
//            nt!PspReferenceCidTableEntry:
//            859f4b30 8bff            mov     edi,edi
//            859f4b32 55              push    ebp
//            859f4b33 8bec            mov     ebp,esp
//            859f4b35 83ec0c          sub     esp,0Ch
//            859f4b38 a1 ac de 90 85      mov     eax,dword ptr [nt!PspCidTable (8590deac)]
//        */
//        for (INT i = 0; i < 0x120; i++) {
//            // fffff802`0841d1a5 48 8b 0d 84 73 f5 ff  mov     rcx,qword ptr [nt!PspCidTable (fffff802`08374530)]
//            if (*(PUCHAR)(ul_callJmp + i) == 0xa1 && *(PUCHAR)(ul_callJmp + i + 1) == 0xac &&
//                *(PUCHAR)(ul_callJmp + i + 2) == 0xde) {
//                // 解析 mov 地址
//                INT i_movCode = *(INT*)(ul_callJmp + i + 1);
//                print("i_movCode = %p \n", i_movCode);
//                ULONG ul_movJmp = ul_callJmp + i_movCode + 5;
//                print("ul_movJmp = %p \n", ul_movJmp);
//
//                // 得到 PspCidTable
//                *tableAddr = ul_movJmp;
//                return TRUE;
//            }
//        }
//    }
//    return FALSE;
//}
//
// VOID UnDriver(PDRIVER_OBJECT driver) { DbgPrint(("Uninstall Driver Is OK \n")); }
//
// extern "C" NTSTATUS DriverEntry(IN PDRIVER_OBJECT Driver, PUNICODE_STRING RegistryPath) {
//    DbgPrint(("hello lyshark \n"));
//
//    ULONG tableAddr = 0;
//
//    get_PspCidTable(&tableAddr);
//
//    print("PspCidTable Address = %p \n", tableAddr);
//
//    Driver->DriverUnload = UnDriver;
//    return STATUS_SUCCESS;
//}