#include <iostream>
#include <windows.h>
#include <string>
#include <tlhelp32.h>

// 根据进程名得到进程PID
DWORD GetPidByName(const char* name) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 pe32 = {sizeof(PROCESSENTRY32)};
    DWORD pid = 0;

    if (Process32First(snapshot, &pe32)) {
        do {
            if (_stricmp((char*)pe32.szExeFile, name) == 0) {
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
        push 0 // 植物id  
        mov eax,0 // y
        push 0    // x
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
    //BOOL virtual_flag = VirtualFreeEx(hProcess, mFuncAddr, 128, MEM_RELEASE);
    //if (virtual_flag == FALSE) {
    //    return FALSE;
    //}

    // 关闭并返回
    CloseHandle(hThread);
    CloseHandle(hProcess);

    return TRUE;
}

int main(int argc, char* argv[]) {
    // 得到进程PID
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

    system("pause");
    return 0;
}