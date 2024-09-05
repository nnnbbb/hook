#include <iostream>
#include <windows.h>
#include <string>
#include <tlhelp32.h>

// 根据进程名得到进程PID
DWORD GetPidByName(const WCHAR* name) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 pe32 = {sizeof(PROCESSENTRY32)};
    DWORD pid = 0;

    if (Process32First(snapshot, &pe32)) {
        do {
            WCHAR* szName = pe32.szExeFile;
            //printf("szName = %ls\n", szName);
            if (wcscmp(szName, name) == 0) {
                pid = pe32.th32ProcessID;
                break;
            }
        } while (Process32Next(snapshot, &pe32));
    }
    CloseHandle(snapshot);
    return pid;
}

// 全屏爆炸攻击
void FullScreenAttack() {
    __asm {
        pushad
        mov ecx,dword ptr ds:[0x00755EAC]
        mov ecx,[ecx+0x868]
        push 0xFFFFFFFF
        push 0  // 植物id  
        mov eax,0  // y
        push 0  // x
        push ecx
        mov esi,0x00418D70
        call esi
        popad
    }
}

// 注入代码
BOOL InjectCode(DWORD dwProcId, LPVOID mFunc) {
    HANDLE hProcess, hThread;
    LPVOID mFuncAddr, ParamAddr;
    DWORD NumberOfByte;

    // 打开当前进程
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcId);
    if (hProcess == NULL) {
        return FALSE;
    }

    printf("[*] 打开目标进程 \n");

    // 分配内存空间
    mFuncAddr = VirtualAllocEx(hProcess, NULL, 128, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (mFuncAddr == NULL) {
        return FALSE;
    }

    printf("[*] 分配内存空间 \n");

    // 写出数据
    if (!WriteProcessMemory(hProcess, mFuncAddr, mFunc, 128, &NumberOfByte)) {
        return FALSE;
    }

    // 创建远程线程
    hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)mFuncAddr, 0, 0, &NumberOfByte);
    if (hThread == NULL) {
        return FALSE;
    }

    printf("[*] 创建远程线程 \n");

    // 等待线程执行结束
    WaitForSingleObject(hThread, INFINITE);

    // 释放内存空间
    // BOOL virtual_flag = VirtualFreeEx(hProcess, mFuncAddr, 128, MEM_RELEASE);
    // if (virtual_flag == FALSE) {
    //    return FALSE;
    //}

    // 关闭并返回
    CloseHandle(hThread);
    CloseHandle(hProcess);

    return TRUE;
}
int PvzHook() {
    HWND Hwnd = FindWindowA(NULL, "Plants vs. Zombies");
    DWORD Pid = 0;
    if (Hwnd == NULL) {
        MessageBoxA(0, "游戏未运行！", "提示", MB_OK);
        return 0;
    }
    GetWindowThreadProcessId(Hwnd, &Pid);

    BOOL ref = InjectCode(Pid, FullScreenAttack);
    if (ref == TRUE) {
        printf("[+] 代码注入完成 \n");
    }
}
void hookFn() {
    printf("11");
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
    printf("[*] mFuncAddr %p\n", mFuncAddr);
    if (mFuncAddr == NULL) {
        return FALSE;
    }
    DWORD NumberOfByte;
    WriteProcessMemory(hProcess, mFuncAddr, hookFn, 128, &NumberOfByte);
    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)mFuncAddr, 0, 0, &NumberOfByte);
    //WaitForSingleObject(hThread, INFINITE);

    system("pause");
    /*
    1. hook地址jmp到自己的代码执行 ebp==00197D00 把 [ebp+8] 的值给某个变量
    2. 创建线程 监听这个变量是否被赋值  被赋值了就取到背包打开关闭的基地址
    */
    return 0;
}