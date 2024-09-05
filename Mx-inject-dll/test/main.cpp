#include <iostream>
#include <windows.h>
#include <string>
#include <tlhelp32.h>
#include "Stop-Threads.h"
#include "speedhack.h"

// 根据进程名得到进程PID
DWORD GetPidByName(const WCHAR* name) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 pe32 = {sizeof(PROCESSENTRY32)};
    DWORD pid = 0;

    if (Process32First(snapshot, &pe32)) {
        do {
            WCHAR* szName = pe32.szExeFile;
            // printf("szName = %ls\n", szName);
            if (wcscmp(szName, name) == 0) {
                pid = pe32.th32ProcessID;
                break;
            }
        } while (Process32Next(snapshot, &pe32));
    }
    CloseHandle(snapshot);
    return pid;
}

PVOID g_SaveAddr = NULL;

DWORD R4(HANDLE TargetProcess, LPCVOID TargetAddress) {
    DWORD RealSize = 0;
    DWORD ReadBuffer = 0;

    ReadProcessMemory(
        TargetProcess,
        TargetAddress,  // 目标进程地址  //此地址是游戏的 是目标进程
        &ReadBuffer,    // 待写入数值的地址 //当前进程的地址
        4,              // 写入4个字节 DWORD 无符号整数 sizeof(int)=4  sizeof(飞机数量)=4
        &RealSize       // 实际写入的字节数量 保存的地址 LPDWORD //DWORD*
    );
    return ReadBuffer;
}

VOID W4(HANDLE TargetProcess, LPVOID TargetAddress, DWORD* Value) {

    DWORD RealSize = 0;
    WriteProcessMemory(
        TargetProcess,
        TargetAddress,  // 目标进程地址  //此地址是游戏的 是目标进程
        Value,          // 待写入数值的地址 //当前进程的地址
        4,              // 写入4个字节 DWORD 无符号整数 sizeof(int)=4  sizeof(飞机数量)=4
        &RealSize
    );
}

DWORD WINAPI MainThread(LPVOID lpParam) {

    return 0;
}


DWORD WINAPI ThreadFunc() {
    DWORD Pid = GetPidByName(L"mxmain.dll");
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, Pid);

    while (true) {
        if (g_SaveAddr) {
            // [[[09CF0000]+18]+24]+1B1

            DWORD VirtualAddress1 = R4(hProcess, (PVOID)g_SaveAddr);
            DWORD VirtualAddress2 = R4(hProcess, (PVOID)(VirtualAddress1 + 0x18));
            DWORD VirtualAddress3 = R4(hProcess, (PVOID)(VirtualAddress2 + 0x24));
            DWORD VirtualAddress4 = VirtualAddress3 + 0x1B1;
            DWORD value = R4(hProcess, (PVOID)VirtualAddress4);  // 打开 0x1000001 关闭 0x1000000
            // printf("[*] value = 0x%x\n", value);

            if (value) {
                StopThreads();
            }
            DWORD v = 1;
            // W4(hProcess, (PVOID)VirtualAddress4, &v);
        }
        Sleep(2000);
    }


    return 0;
}

int main(int argc, char* argv[]) {
    // 得到进程PID
    DWORD Pid = GetPidByName(L"mxmain.dll");
    printf("Pid = %d\n", Pid);
    // 打开当前进程
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, Pid);
    if (hProcess == NULL) {
        return FALSE;
    }

    printf("[*] 打开目标进程 \n");

    // 分配内存空间
    PVOID mFuncAddr = VirtualAllocEx(hProcess, NULL, 128, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    // 保存基址的地址
    PVOID SaveAddr = VirtualAllocEx(hProcess, NULL, 4, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    g_SaveAddr = SaveAddr;
    printf("[*] mFuncAddr %p\n", mFuncAddr);
    printf("[*] SaveAddr %p\n", SaveAddr);
    if (mFuncAddr == NULL) {
        return FALSE;
    }
    char JmpHookCode[] = {0xe9, 0x00, 0x00, 0x00, 0x00};
    char HookCode[] = {
        /*  0 */ 0x8B, 0x75, 0x08, 0x85, /* 3  */
        /*  4 */ 0xF6, 0x60, 0x9C, 0xB9, /* 7  */
        /*  8 */ 0x00, 0x7D, 0x19, 0x00, /* 11 */
        /* 12 */ 0x39, 0xCD, 0x75, 0x09, /* 15 */
        /* 16 */ 0x8B, 0x55, 0x08, 0x89, /* 19 */
        /* 20 */ 0x15,
        /* 开始21 保存基址的地址*/ 0x00, 0x00, 0x00, 0x00, /* 结束24 保存基址的地址*/
        /*25*/ 0x9D, 0x61, 0xE9,
        /* 开始28 跳回去的地址  */ 0x00, 0x00, 0x00, 0x00 /* 跳回去的地址*/
    };
    // 将保存基址的地址 的地址写入hook code
    memcpy(&HookCode[21], &SaveAddr, 4);

    // 要Hook的基地址
    DWORD BaseAddress = 0x00EC1814;

    //                   要跳转的目标地址 - 跳转的起始地址 - (jmp指令1个字节 + 地址4个字节)
    DWORD JmpToOffset = (DWORD)mFuncAddr - BaseAddress - 5;
    printf("[*] JmpToOffset 0x%x\n", JmpToOffset);
    // 跳转回去的偏移计算
    DWORD JmpBackOffset = (BaseAddress + 5) - ((DWORD)mFuncAddr + 27 + 5);

    // hook 跳转到自己的代码段执行
    memcpy(&JmpHookCode[1], &JmpToOffset, 4);
    WriteProcessMemory(hProcess, (char*)BaseAddress, JmpHookCode, sizeof(JmpHookCode), NULL);

    // 写入代码段
    memcpy(&HookCode[28], &JmpBackOffset, 4);
    WriteProcessMemory(hProcess, (char*)mFuncAddr, HookCode, sizeof(HookCode), NULL);

    // DWORD NumberOfByte;
    // WriteProcessMemory(hProcess, mFuncAddr, hookFn, 128, &NumberOfByte);
    // HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)mFuncAddr, 0, 0, &NumberOfByte);
    HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadFunc, 0, 0, NULL);  // 创建线程
    WaitForSingleObject(hThread, INFINITE);

    system("pause");
    /*
    1. hook地址jmp到自己的代码执行 ebp==00197D00 把 [ebp+8] 的值给某个变量
    2. 创建线程 监听这个变量是否被赋值  被赋值了就取到背包打开关闭的基地址
    */
    return 0;
}